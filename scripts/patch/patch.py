import os
import argparse
import shutil
import re
import gc
import git
from git import Repo
from git.util import rmtree
from git.remote import Remote
from embarc_tools.utils import cd, getcwd
import sys
'''
# EMBARC_ROOT = "C:/Users/jingru/Documents/gitlab/embarc_osp" ## get from
when generate patch, use
git format-patch xxx --add-header="repo_link:https://github.com/ARMmbed/mbedtls.git repo_commit:mbedtls-2.4.1"
'''

def parse_arguments():
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--embarc-root", required=True, help="Specify the embARC_osp directory")
    patches_required_roots = ["library", "middleware", "os"]
    parser.add_argument("--patch-roots", action='append', default=patches_required_roots, help="Directories to search for patch files."
        "All .patch files in these directories will be processed.")
    return parser.parse_args()


class Patch:
    def __init__(self, root, files=None):
        self.required_files = ["library", "include"]
        if isinstance(files, list) and files:
            self.required_files = files.extend(self.required_files)
        self.url, self.commit = self.repo_info(root)
        self.repo = None
        # self.repo.git = self.repo.git
        

    def repo_info(self, root):
        patch_file = None
        for file in os.listdir(root):
            if os.path.splitext(file)[-1] == ".patch":
                patch_file = os.path.join(root, file)
                break
        if patch_file:
            pattern = r"^repo_link:(.*) repo_commit:(.*)"
            with open(patch_file) as f:
                lines = f.read().splitlines()
                for line in lines:
                    m = re.match(pattern, line, re.M|re.I)
                    if m:
                        repo_link = m.group(1)
                        repo_commit = m.group(2)
                        return repo_link, repo_commit
        return False, False


    def need_patch(self):
        result = False
        if not self.required_files:
            return True
        for file in self.required_files:
            if not os.path.exists(file):
                result = True
                break
        return result


    def check_remote_branch(self):
        branchs_str = self.repo.git.branch("-a")
        branchs_list = branchs_str.split("\n")
        remote_branch = str()
        pattern = r"^\s*remotes\/"
        for branch in branchs_list:
            remote = re.compile(pattern).findall(branch)
            if remote:
                remote_branch = branch.replace(remote[0] , "")
            if remote_branch:
                break
        return remote_branch


    def check_commit(self):
        result = self.repo.git.log("--pretty", "-1", self.commit)
        if not result:
            self.repo.git.fetch(depth=3000)


    def fetch_repo(self, url, branch):
        if os.path.exists(".git"):
            gc.collect()
            repo = git.Repo(getcwd())
            repo.git.clear_cache()
            rmtree(repo.git_dir)
        self.repo = git.Repo.init(getcwd())
        r = Remote(self.repo, name="origin")
        if r in Remote.list_items(self.repo):
            self.repo.delete_remote(r)
        self.repo.git.remote("add", "origin", url)
        self.repo.git.fetch("origin", "-t")
        remote_branch = self.check_remote_branch()
        if not remote_branch:
            print("Fetch tag failed, just fetch all the source code")
            self.repo.git.fetch("origin")
            remote_branch = self.check_remote_branch()
            if not remote_branch:
                print("No branch found in the remote repo, patch failed")
                shutil.rmtree(".git")
                return False
        self.repo.git.checkout(remote_branch, "--force", b="master")
        print("Branch 'master' set up to track remote branch '{}' from 'origin'".format(remote_branch))
        self.check_commit()
        self.repo.git.checkout(self.commit, "--force", b=branch)
        print("Switched to a new branch '{}'".format(branch))


    def patch(self, branch="embARC"):
        """Apply patch to a repo
        When get repo url and repo commit from .patch file, it will fetch
        tags from one repositories according to url, and create a new 
        branch to apply patch. Or it will apply patch to current branch.
        @param branch A new branch to apply patches.

        """
        if self.need_patch():
            if self.url and self.commit:
                self.fetch_repo(self.url, branch)
            for file in os.listdir(getcwd()):
                if os.path.splitext(file)[-1] == ".patch":
                    patch_file = os.path.join(getcwd(), file)
                    print("Try to apply patch {} for this repo".format(file))
                    self.repo.git.am(patch_file) 
            print("-----Patch successfully-----")   
        return True


def get_patch_root(path=None):
    result = list()
    if not path:
        path = getcwd()
    if os.path.exists(path):
        for root, dirs, files in os.walk(path):
            for file in files:
                if os.path.splitext(file)[-1] == ".patch":
                    result.append(root)
                    break
    return result


def main():
    options = parse_arguments()
    patches_required_roots = list()
    EMBARC_ROOT = os.path.abspath(options.embarc_root)
    if not os.path.exists(EMBARC_ROOT):
        print("Please Specify the embARC_osp directory with --embarc-root option")
        return
    # sys.path.insert(0, os.path.join(EMBARC_ROOT, "scripts/"))
    for patch_root in options.patch_roots:
        patch_root = os.path.join(EMBARC_ROOT, patch_root)
        patch_root_list = get_patch_root(patch_root)
        if patch_root_list:
            patches_required_roots.extend(patch_root_list)
    for patch_root in patches_required_roots:
        with cd(patch_root):
            patch = Patch(patch_root)
            patch.patch()


if __name__ == "__main__":
    main()
