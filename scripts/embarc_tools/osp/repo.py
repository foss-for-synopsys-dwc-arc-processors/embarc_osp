from __future__ import print_function, unicode_literals
try:
    # Python 2
    basestring = (unicode, str)
    from urlparse import urlparse
    from urllib import quote
except NameError:
    # Python 3
    basestring = str
    from urllib.parse import urlparse, quote
import os
import re
import shutil
from ..osp import (formaturl, REGEX_LOCAL_REF, REGEX_URL_REF, SCMS, ProcessException)
from ..notify import print_string
from ..utils import (getcwd, cd, rmtree_readonly, cwd_root, relpath)


class Repo(object):
    is_local = False
    name = None
    path = None
    url = None
    rev = None
    cache = None
    scm = None

    @classmethod
    def fromurl(cls, url, path=None):
        repo = cls()
        m_local = re.match(REGEX_LOCAL_REF, url.strip().replace('\\', '/'))
        m_repo_ref = re.match(REGEX_URL_REF, url.strip().replace('\\', '/'))
        if m_local:
            repo.name = os.path.basename(path or m_local.group(1))
            repo.path = os.path.abspath(path or os.path.join(getcwd(), m_local.group(1)))
            repo.url = m_local.group(1)
            repo.rev = m_local.group(2)
            repo.is_local = True
        elif m_repo_ref:
            repo.name = re.sub(r'\.(git|hg)/?$', '', os.path.basename(path or m_repo_ref.group(2)))
            repo.path = os.path.abspath(path or os.path.join(getcwd(), repo.name))
            repo.url = formaturl(m_repo_ref.group(1))
            repo.rev = m_repo_ref.group(3)

            if repo.rev and repo.rev != 'latest' and repo.rev != 'tip' and not re.match(r'^([a-fA-F0-9]{6,40})$', repo.rev):
                print('[embARC] Invalid revision (%s)' % repo.rev)
        else:
            print('[embARC] Invalid repository (%s)' % url.strip())
        return repo

    @classmethod
    def fromlib(cls, lib=None):  # get url from .lib
        with open(lib) as f:
            ref = f.read(200)

        m_local = re.match(REGEX_LOCAL_REF, ref.strip().replace('\\', '/'))
        m_repo_ref = re.match(REGEX_URL_REF, ref.strip().replace('\\', '/'))
        if not (m_local or m_repo_ref):
            print(
                "[embARC] File \"%s\" in \"%s\" uses a non-standard .lib file extension, which is not compatible with the mbed build tools.\n" % (os.path.basename(lib), os.path.split(lib)[0]))
            return False
        else:
            return cls.fromurl(ref, lib[:-4])

    @classmethod
    def isrepo(cls, path=None):  # a folder is a git repo
        name = ".git"
        if os.path.isdir(os.path.join(path, '.' + name)):
            return True
        return False

    @classmethod
    def findparent(cls, path=None):
        path = os.path.abspath(path or getcwd())

        while cd(path):
            if Repo.isrepo(path):
                return path

            tpath = path
            path = os.path.split(path)[0]
            if tpath == path:
                break

        return None

    @classmethod
    def pathtype(cls, path=None):
        path = os.path.abspath(path or getcwd())

        depth = 0
        while cd(path):
            tpath = path
            path = Repo.findparent(path)
            if path:
                depth += 1
                path = os.path.split(path)[0]
                if tpath == path:       # Reached root.
                    break
            else:
                break

        return "directory" if depth == 0 else "repo"

    def revtype(self, rev=None, fmt=3):
        if rev is None or (not rev):
            output = ('latest' if fmt & 1 else '') + (' revision in the current branch' if fmt & 2 else '')
        elif re.match(r'^([a-fA-F0-9]{6,40})$', rev) or re.match(r'^([0-9]+)$', rev):
            revtags = self.gettags(rev) if rev else []
            output = ('rev ' if fmt & 1 else '') + (('#' + rev[:12] + ((' (tag' + ('s' if len(revtags) > 1 else '') + ': ' + ', '.join(revtags[0:2]) + ')') if revtags else '')) if fmt & 2 and rev else '')
        else:
            output = ('branch/tag' if fmt & 1 else '') + (' "' + rev + '"' if fmt & 2 else '')

        return re.sub(r' \(', ', ', re.sub(r'\)', '', output)) if fmt & 4 else output

    @classmethod
    def isurl(cls, url):
        return re.match(REGEX_URL_REF, url.strip().replace('\\', '/'))

    @classmethod
    def isinsecure(cls, url):
        up = urlparse(url)
        return (not up) or (up.scheme and up.scheme not in ['http', 'https', 'ssh', 'git']) or (up.port and int(up.port) not in [22, 80, 443])

    @property
    def lib(self):
        return self.path + '.lib'

    @property
    def fullurl(self):
        if self.url:
            return (self.url.rstrip('/') + '/' +
                    (('#') + self.rev if self.rev else ''))

    def sync(self):
        self.url = None
        self.rev = None
        if os.path.isdir(self.path):
            try:
                self.scm = self.getscm()
            except ProcessException:
                pass

            try:
                self.url = self.geturl()
                if not self.url:
                    self.is_local = True
                    ppath = self.findparent(os.path.split(self.path)[0])
                    self.url = relpath(ppath, self.path).replace("\\", "/") if ppath else os.path.basename(self.path)
            except ProcessException:
                pass

            try:
                self.rev = self.getrev()
            except ProcessException:
                pass

            try:
                self.libs = list(self.getlibs())  # .bld .lib repo
            except ProcessException:
                pass

    def getscm(self):
        for name, scm in SCMS.items():
            if os.path.isdir(os.path.join(self.path, '.' + name)):
                return scm

    def gettags(self, rev=None):
        tags = self.scm.gettags() if self.scm else []
        if rev:
            return [tag[1] for tag in tags if tag[0].startswith(rev)]
        else:
            return tags

    # Pass backend SCM commands and parameters if SCM exists
    def __wrap_scm(self, method):
        def __scm_call(*args, **kwargs):
            if self.scm and hasattr(self.scm, method) and callable(getattr(self.scm, method)):
                with cd(self.path):
                    return getattr(self.scm, method)(*args, **kwargs)
        return __scm_call

    def __getattr__(self, attr):
        if attr in ['geturl', 'getrev', 'add', 'remove', 'ignores', 'ignore', 'unignore',
                    'status', 'dirty', 'commit', 'outgoing', 'publish', 'checkout', 'update',
                    'isdetached']:
            wrapper = self.__wrap_scm(attr)
            self.__dict__[attr] = wrapper
            return wrapper
        else:
            raise AttributeError("Repo instance doesn't have attribute '%s'" % attr)

    def remove(self, dest, *args, **kwargs):
        if os.path.isfile(dest):
            try:
                os.remove(dest)
            except OSError:
                pass
        return self.scm.remove(dest, *args, **kwargs)

    def clone(self, url, path, rev=None, depth=None, protocol=None, offline=False, **kwargs):
        # Sorted so repositories that match urls are attempted first
        for scm in SCMS.values():

            main = True
            cache = self.get_cache(url, scm.name)
            # Try to clone with cache ref first
            if cache and not os.path.isdir(path):
                print("[embARC] Found matching cached repository in \"%s\"" % cache)
                try:
                    if os.path.split(path)[0] and not os.path.isdir(os.path.split(path)[0]):
                        os.makedirs(os.path.split(path)[0])

                    print("[embARC] Carbon copy from \"%s\" to \"%s\"" % (cache, path))
                    shutil.copytree(cache, path)

                    with cd(path):
                        scm.seturl(formaturl(url, protocol))
                        scm.cleanup()
                        print("[embARC] Update cached copy from remote repository")
                        if not rev:
                            rev = scm.default_branch
                        scm.update(rev, True, is_local=offline)
                        main = False
                except (ProcessException, IOError):
                    print("[embARC] Discarding cached repository")
                    if os.path.isdir(path):
                        rmtree_readonly(path)

            # Main clone routine if the clone with cache ref failed (might occur if cache ref is dirty)
            if main:
                if offline:
                    continue
                try:
                    scm.clone(url, path, depth=depth, protocol=protocol, **kwargs)
                except ProcessException:
                    if os.path.isdir(path):
                        rmtree_readonly(path)
                    continue

            self.scm = scm
            self.url = url
            self.path = os.path.abspath(path)
            self.ignores()
            self.set_cache(url)
            return True

        if offline:
            print("[embARC] Unable to clone repository \"%s\" in offline mode ('--offline' used)." % url)
            if os.path.isdir(path):
                rmtree_readonly(path)

        return False

    def getlibs(self):
        for root, dirs, files in os.walk(self.path):
            dirs[:] = [d for d in dirs if not d.startswith('.')]
            files[:] = [f for f in files if not f.startswith('.')]

            for f in files:
                if f.endswith('.lib'):
                    repo = Repo.fromlib(os.path.join(root, f))
                    if repo:
                        yield repo
                    if f[:-4] in dirs:
                        dirs.remove(f[:-4])

    def write(self):
        up = urlparse(self.url)
        if up.hostname:
            url = up._replace(netloc=up.hostname + (':' + str(up.port) if up.port else '')).geturl()  # strip auth string
        else:
            url = self.url  # use local repo "urls" as is

        if os.path.isfile(self.lib):
            with open(self.lib) as f:
                lib_repo = Repo.fromurl(f.read().strip())
                if (formaturl(lib_repo.url, 'https') == formaturl(url, 'https') and  # match URLs in common schema (https)
                        (lib_repo.rev == self.rev or                         # match revs, even if rev is None (valid for repos with no revisions)
                         (lib_repo.rev and self.rev and
                          lib_repo.rev == self.rev[0:len(lib_repo.rev)]))):  # match long and short rev formats
                    return

        if up.hostname == "github.com":
            # in public_scm_services:
            # Safely convert repo URL to https schema if this is a public SCM service (github/butbucket), supporting all schemas.
            # This allows anonymous cloning of public repos without having to have ssh keys and associated accounts at github/bitbucket/etc.
            # Without this anonymous users will get clone errors with ssh repository links even if the repository is public.
            # See https://help.github.com/articles/which-remote-url-should-i-use/
            url = formaturl(url, 'https')

        ref = url.rstrip('/') + '/' + ('#' + self.rev if self.rev else '')
        print("[embARC] Updating reference \"%s\" -> \"%s\"" % (relpath(cwd_root, self.path) if cwd_root != self.path else self.name, ref))
        with open(self.lib, 'w') as f:
            f.write(ref + "\n")

    def rm_untracked(self):
        # untracked some file
        untracked = self.scm.untracked()
        for f in untracked:
            if re.match(r'(.+)\.(lib)$', f) and os.path.isfile(f):
                print("[embARC] Remove untracked library reference \"%s\"" % f)
                os.remove(f)

    def url2cachedir(self, url):
        up = urlparse(formaturl(url, 'https'))
        if self.cache and up and up.netloc:
            return os.path.join(self.cache, quote(up.netloc), quote(re.sub(r'^/', '', up.path)))

    def get_cache(self, url, scm):
        cpath = self.url2cachedir(url)
        if cpath and os.path.isdir(os.path.join(cpath, '.' + scm)):
            return cpath

    def set_cache(self, url):
        cpath = self.url2cachedir(url)
        if cpath and os.path.isdir(self.path):
            try:
                if not os.path.isdir(cpath):
                    os.makedirs(cpath)
                scm_dir = '.' + self.scm.name
                if os.path.isdir(os.path.join(cpath, scm_dir)):
                    rmtree_readonly(os.path.join(cpath, scm_dir))
                shutil.copytree(os.path.join(self.path, scm_dir), os.path.join(cpath, scm_dir))
            except Exception:
                print("[embARC] Unable to cache \"%s\" to \"%s\"" % (self.path, cpath))
        return False

    def can_update(self, clean, clean_deps):
        err = None
        if (self.is_local or self.url is None) and not clean_deps:
            err = (
                "Preserving local library \"%s\" in \"%s\".\nPlease publish this library to a remote URL to be able to restore it at any time."
                "You can use --ignore switch to ignore all local libraries and update only the published ones.\n"
                "You can also use --clean-deps switch to remove all local libraries. WARNING: This action cannot be undone." % (self.name, self.path))
        elif not clean and self.dirty():
            err = (
                "Uncommitted changes in \"%s\" in \"%s\".\nPlease discard or stash them first and then retry update.\n"
                "You can also use --clean switch to discard all uncommitted changes. WARNING: This action cannot be undone." % (self.name, self.path))
        elif not clean_deps and self.outgoing():
            err = (
                "Unpublished changes in \"%s\" in \"%s\".\nPlease publish them first using the \"publish\" command.\n"
                "You can also use --clean-deps to discard all local commits and replace the library with the one included in this revision. WARNING: This action cannot be undone." % (self.name, self.path))

        return (False, err) if err else (True, "OK")

    def check_repo(self, show_warning=None):
        err = None
        if not os.path.isdir(self.path):
            err = (
                "Library reference \"%s\" points to non-existing library in \"%s\"\n"
                "You can use \"embarc deploy\" to import the missing libraries.\n"
                "You can also use \"embarc sync\" to synchronize and remove all invalid library references." % (os.path.basename(self.lib), self.path))
        elif not self.isrepo(self.path):
            err = (
                "Library reference \"%s\" points to a folder \"%s\", which is not a valid repository.\n"
                "You can remove the conflicting folder manually and use \"embarc deploy\" to import the missing libraries\n"
                "You can also remove library reference \"%s\" and use \"embarc sync\" again." % (os.path.basename(self.lib), self.path, self.lib))

        if err:
            if show_warning:
                print_string(err, level="warning")
            else:
                print_string(err, level="error")
            return False
        return True
