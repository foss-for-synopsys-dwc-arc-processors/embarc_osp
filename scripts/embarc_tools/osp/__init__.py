from __future__ import print_function, unicode_literals
import re
import os
from ..utils import show_progress, hide_progress, getcwd, pquery, popen


REGEX_REPO_URL = r'^(git\://|file\://|ssh\://|https?\://|)(([^/:@]+)(\:([^/:@]+))?@)?([^/:]{3,})(\:\d+)?[:/](.+?)(\.git|\.hg|\/?)$'
REGEX_URL_REF = r'^(.*/([\w.+-]+)(?:\.\w+)?)/?(?:#(.*))?$'
REGEX_LOCAL_REF = r'^([\w.+-][\w./+-]*?)/?(?:#(.*))?$'
GIT_CMD = 'git'
SCMS = {}


def formaturl(url, format="default"):
    url = "%s" % url
    m = re.match(REGEX_REPO_URL, url)
    if m and m.group(1) == '':   # no protocol specified, probably ssh string like "git@github.com:xxx/osp.git"
        url = 'ssh://%s%s%s/%s' % (m.group(2) or 'git@', m.group(6), m.group(7) or '', m.group(8))  # convert to common ssh URL-like format
        m = re.match(REGEX_REPO_URL, url)

    if m:
        if format == "ssh":
            url = 'ssh://%s%s%s/%s' % (m.group(2) or 'git@', m.group(6), m.group(7) or '', m.group(8))
        elif format == "http":
            url = 'http://%s%s%s/%s' % (m.group(2) if (m.group(2) and (m.group(5) or m.group(3) != 'git')) else '', m.group(6), m.group(7) or '', m.group(8))
        elif format == "https":
            url = 'https://%s%s%s/%s' % (m.group(2) if (m.group(2) and (m.group(5) or m.group(3) != 'git')) else '', m.group(6), m.group(7) or '', m.group(8))
    return url


def scm(name):
    def _scm(cls):
        SCMS[name] = cls()
        return cls
    return _scm


def staticclass(cls):
    for k, v in cls.__dict__.items():
        if hasattr(v, '__call__') and not k.startswith('__'):
            setattr(cls, k, staticmethod(v))
    return cls


class ProcessException(Exception):
    pass


@scm('git')
@staticclass
class Git(object):
    name = 'git'
    default_branch = 'master'
    ignore_file = os.path.join('.git', 'info', 'exclude')

    def init(path=None, very_verbose=False):
        popen([GIT_CMD, 'init'] + ([path] if path else []) + ([] if very_verbose else ['-q']))

    def cleanup(very_verbose=False):
        print("Cleaning up Git index")
        pquery([GIT_CMD, 'checkout', '--detach', 'HEAD'] + ([] if very_verbose else ['-q']))  # detach head so local branches are deletable
        branches = []
        lines = pquery([GIT_CMD, 'branch']).strip().splitlines()  # fetch all local branches
        for line in lines:
            if re.match(r'^\*?\s+\((.+)\)$', line):
                continue
            line = re.sub(r'\s+', '', line)
            branches.append(line)

        for branch in branches:  # delete all local branches so the new repo clone is not polluted
            pquery([GIT_CMD, 'branch', '-D', branch])

    def clone(url, name=None, depth=None, protocol=None, very_verbose=True, verbose=False):
        if very_verbose:
            popen([GIT_CMD, 'clone', formaturl(url, protocol), name] + (['--depth', depth] if depth else []) + (['-v'] if very_verbose else ([] if verbose else ['-q'])))
        else:
            pquery([GIT_CMD, 'clone', '--progress', formaturl(url, protocol), name] + (['--depth', depth] if depth else []), output_callback=Git.action_progress)
            hide_progress()

    def add(dest, very_verbose=False):
        print("Adding reference " + dest)
        try:
            popen([GIT_CMD, 'add', dest] + (['-v'] if very_verbose else []))
        except ProcessException:
            pass

    def remove(dest, very_verbose=False):
        print("Removing reference " + dest)
        try:
            pquery([GIT_CMD, 'rm', '-f', dest] + ([] if very_verbose else ['-q']))
        except ProcessException:
            pass

    def commit(msg=None, very_verbose=False, verbose=False):
        popen([GIT_CMD, 'commit', '-a'] + (['-m', msg] if msg else []) + (['-v'] if very_verbose else ([] if verbose else ['-q'])))

    def publish(all_refs=None, very_verbose=False, verbose=False):
        if all_refs:
            popen([GIT_CMD, 'push', '--all'] + (['-v'] if very_verbose else ([] if verbose else ['-q'])))
        else:
            remote = Git.getremote()
            branch = Git.getbranch()
            if remote and branch:
                popen([GIT_CMD, 'push', remote, branch] + (['-v'] if very_verbose else ([] if verbose else ['-q'])))
            else:
                err = "Unable to publish outgoing changes for \"%s\" in \"%s\".\n" % (os.path.basename(getcwd()), getcwd())
                if not remote:
                    print(err + "The local repository is not associated with a remote one.", 1)
                if not branch:
                    print(err + "Working set is not on a branch.", 1)

    def fetch(very_verbose=False, verbose=False):
        print("Fetching revisions from remote repository to \"%s\"" % os.path.basename(getcwd()))
        popen([GIT_CMD, 'fetch', '--all', '--tags'] + (['-v'] if very_verbose else ([] if verbose else ['-q'])))

    def discard(clean_files=False, very_verbose=False):
        print("Discarding local changes in \"%s\"" % os.path.basename(getcwd()))
        pquery([GIT_CMD, 'reset', 'HEAD'] + ([] if very_verbose else ['-q']))  # unmarks files for commit
        pquery([GIT_CMD, 'checkout', '.'] + ([] if very_verbose else ['-q']))  # undo  modified files
        pquery([GIT_CMD, 'clean', '-fd'] + (['-x'] if clean_files else []) + (['-q'] if very_verbose else ['-q']))  # cleans up untracked files and folders

    def merge(dest, very_verbose=False, verbose=False):
        print("Merging \"%s\" with \"%s\"" % (os.path.basename(getcwd()), dest))
        popen([GIT_CMD, 'merge', dest] + (['-v'] if very_verbose else ([] if verbose else ['-q'])))

    def checkout(rev, clean=False, very_verbose=False):
        if not rev:
            return
        print("Checkout \"%s\" in %s" % (rev, os.path.basename(getcwd())))
        branch = None
        refs = Git.getbranches(rev)
        for ref in refs:  # re-associate with a local or remote branch (rev is the same)
            m = re.match(r'^(.*?)\/(.*?)$', ref)
            if m and m.group(2) != "HEAD":  # matches origin/<branch> and isn't HEAD ref
                if not os.path.exists(os.path.join('.git', 'refs', 'heads', m.group(2))):  # okay only if local branch with that name doesn't exist (git will checkout the origin/<branch> in that case)
                    branch = m.group(2)
            elif ref != "HEAD":
                branch = ref  # matches local branch and isn't HEAD ref

            if branch:
                print("Revision \"%s\" matches a branch \"%s\" reference. Re-associating with branch" % (rev, branch))
                popen([GIT_CMD, 'checkout', branch] + ([] if very_verbose else ['-q']))
                break

        if not branch:
            popen([GIT_CMD, 'checkout', rev] + (['-f'] if clean else []) + ([] if very_verbose else ['-q']))

    def update(rev=None, clean=False, clean_files=False, is_local=False):
        if not is_local:
            Git.fetch()
        if clean:
            Git.discard(clean_files)
        if rev:
            Git.checkout(rev, clean)
        else:
            remote = Git.getremote()
            branch = Git.getbranch()
            if remote and branch:
                try:
                    Git.merge('%s/%s' % (remote, branch))
                except ProcessException:
                    pass
            else:
                err = "Unable to update \"%s\" in \"%s\"." % (os.path.basename(getcwd()), getcwd())
                if not remote:
                    print(err + " The local repository is not associated with a remote one.")
                if not branch:
                    print(err + " Working set is not on a branch.")

    def status(very_verbose=False):
        return pquery([GIT_CMD, 'status', '-s'] + (['-v'] if very_verbose else []))

    def dirty():
        return pquery([GIT_CMD, 'status', '-uno', '--porcelain'])

    def untracked():
        return pquery([GIT_CMD, 'ls-files', '--others', '--exclude-standard']).splitlines()

    def outgoing():
        # Get default remote
        remote = Git.getremote()
        if not remote:
            return -1
        # Get current branch
        branch = Git.getbranch()
        if not branch:
            # Default to "master" in detached mode
            branch = "master"
        # Check if local branch exists. If not, then just carry on
        try:
            pquery([GIT_CMD, 'rev-parse', '%s' % branch])
        except ProcessException:
            return 0
        # Check if remote branch exists. If not, then it's a new branch
        try:
            if not pquery([GIT_CMD, 'rev-parse', '%s/%s' % (remote, branch)]):
                return 1
        except ProcessException:
            return 1
        # Check for outgoing commits for the same remote branch only if it exists locally and remotely
        return 1 if pquery([GIT_CMD, 'log', '%s/%s..%s' % (remote, branch, branch)]) else 0

    # Checks whether current working tree is detached
    def isdetached():
        return True if Git.getbranch() == "" else False

    # Finds default remote
    def getremote():
        remote = None
        remotes = Git.getremotes('push')
        for r in remotes:
            remote = r[0]
            # Prefer origin which is Git's default remote when cloning
            if r[0] == "origin":
                break
        return remote

    # Finds all associated remotes for the specified remote type
    def getremotes(rtype='fetch'):
        result = []
        remotes = pquery([GIT_CMD, 'remote', '-v']).strip().splitlines()
        for remote in remotes:
            remote = re.split(r'\s', remote)
            t = re.sub('[()]', '', remote[2])
            if not rtype or rtype == t:
                result.append([remote[0], remote[1], t])
        return result

    def seturl(url):
        print("Setting url to \"%s\" in %s" % (url, getcwd()))
        return pquery([GIT_CMD, 'remote', 'set-url', 'origin', url]).strip()

    def geturl():
        url = ""
        remotes = Git.getremotes()
        for remote in remotes:
            url = remote[1]
            if remote[0] == "origin":  # Prefer origin URL
                break
        return formaturl(url)

    def getrev():
        return pquery([GIT_CMD, 'rev-parse', 'HEAD']).strip()

    # Gets current branch or returns empty string if detached
    def getbranch(rev='HEAD'):
        try:
            branch = pquery([GIT_CMD, 'rev-parse', '--symbolic-full-name', '--abbrev-ref', rev]).strip()
        except ProcessException:
            branch = "master"
        return branch if branch != "HEAD" else ""

    # Get all refs
    def getrefs():
        try:
            return pquery([GIT_CMD, 'show-ref', '--dereference']).strip().splitlines()
        except ProcessException:
            return []

    # Finds branches (local or remote). Will match rev if specified
    def getbranches(rev=None, ret_rev=False):
        result = []
        refs = Git.getrefs()
        for ref in refs:
            m = re.match(r'^(.+)\s+refs\/(heads|remotes)\/(.+)$', ref)
            if m and (not rev or m.group(1).startswith(rev)):
                result.append(m.group(1) if ret_rev else m.group(3))
        return result

    # Finds tags. Will match rev if specified
    def gettags():
        tags = []
        refs = Git.getrefs()
        for ref in refs:
            m = re.match(r'^(.+)\s+refs\/tags\/(.+)$', ref)
            if m:
                t = m.group(2)
                if re.match(r'^(.+)\^\{\}$', t):  # detect tag "pointer"
                    t = re.sub(r'\^\{\}$', '', t)  # remove "pointer" chars, e.g. some-tag^{}
                    for tag in tags:
                        if tag[1] == t:
                            tags.remove(tag)
                tags.append([m.group(1), t])
        return tags

    # Finds branches a rev belongs to
    def revbranches(rev):
        branches = []
        lines = pquery([GIT_CMD, 'branch', '-a', '--contains'] + ([rev] if rev else [])).strip().splitlines()
        for line in lines:
            if re.match(r'^\*?\s+\((.+)\)$', line):
                continue
            line = re.sub(r'\s+', '', line)
            branches.append(line)
        return branches

    def ignores():
        try:
            ignore_file_parent_directory = os.path.dirname(Git.ignore_file)
            if not os.path.exists(ignore_file_parent_directory):
                os.mkdir(ignore_file_parent_directory)

            with open(Git.ignore_file, 'w') as f:
                f.write('\n'.join(ignores) + '\n')
        except IOError:
            print("Unable to write ignore file in \"%s\"" % os.path.join(getcwd(), Git.ignore_file), 1)

    def ignore(dest):
        try:
            with open(Git.ignore_file) as f:
                exists = dest in f.read().splitlines()
        except IOError:
            exists = False

        if not exists:
            try:
                ignore_file_parent_directory = os.path.dirname(Git.ignore_file)
                if not os.path.exists(ignore_file_parent_directory):
                    os.mkdir(ignore_file_parent_directory)

                with open(Git.ignore_file, 'a') as f:
                    f.write(dest.replace("\\", "/") + '\n')
            except IOError:
                print("Unable to write ignore file in \"%s\"" % os.path.join(getcwd(), Git.ignore_file), 1)

    def unignore(dest):
        try:
            with open(Git.ignore_file) as f:
                lines = f.read().splitlines()
        except IOError:
            lines = []

        if dest in lines:
            lines.remove(dest)
            try:
                ignore_file_parent_directory = os.path.dirname(Git.ignore_file)
                if not os.path.exists(ignore_file_parent_directory):
                    os.mkdir(ignore_file_parent_directory)

                with open(Git.ignore_file, 'w') as f:
                    f.write('\n'.join(lines) + '\n')
            except IOError:
                print("Unable to write ignore file in \"%s\"" % os.path.join(getcwd(), Git.ignore_file), 1)

    def action_progress(line, sep):
        m = re.match(r'(\w+).+?\s+(\d+)/(\d+)\s+.*?', line)
        if m:
            if m.group(1) == "manifests":
                show_progress('Downloading', (float(m.group(2)) / float(m.group(3))) * 20)
            if m.group(1) == "files":
                show_progress('Downloading', (float(m.group(2)) / float(m.group(3))) * 100)


ignores = [
    # Version control folders
    ".hg",
    ".git",
    ".svn",
    ".CVS",
    ".cvs",

    # Version control fallout
    "*.orig",

    # mbed Tools
    "BUILD",
    ".build",
    ".export",

    # Online IDE caches
    ".msub",
    ".meta",
    ".ctags*",

    # uVision project files
    "*.uvproj",
    "*.uvopt",

    # Eclipse project files
    "*.project",
    "*.cproject",
    "*.launch",

    # IAR project files
    "*.ewp",
    "*.eww",

    # GCC make
    "/Makefile",
    "Debug",

    # HTML files
    "*.htm",

    # Settings files
    "*.settings",

    # Python
    "*.py[cod]",
    "# subrepo ignores",
]
