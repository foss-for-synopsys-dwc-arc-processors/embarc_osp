#!/usr/bin/env bash

## REPO CONFIGURATION
REPO_LINK="https://github.com/ARMmbed/mbedtls.git"
REPO_COMMIT="mbedtls-2.16.0"
SRC_DIRS="library,include"

PREV_DIR=$(pwd)
SCRIPT_DIR=$(dirname $0)

cd ${SCRIPT_DIR}

SRC_DIRS_LIST=(${SRC_DIRS//,/ })

exit_proc ()
{
	cd ${PREV_DIR}
	if [ $1 == 0 ] ; then
		echo "Patches are applied"
	else
		echo "Patches are not applied"
	fi
	exit $1
}

if which git >/dev/null 2>&1; then :; else
    echo "You need git in your path" >&2
    exit_proc 1
fi


exist_flag=true

for srcdir in ${SRC_DIRS_LIST[@]}
do
	if [ ! -e "${srcdir}" ] ; then
		exist_flag=false
	fi
done

if ${exist_flag} ; then
	echo "Required source directories already existed"
	echo "If compiling job is still failing, please try to restore original files of this release"
	exit_proc 1
fi

if [ -e ".git" ] ; then
	echo "Git repo existed, we will remove it and add a new one"
	rm -rf .git
fi
echo "Clone repo from ${REPO_LINK} at commit or tag ${REPO_COMMIT}"
git init .
git remote add origin ${REPO_LINK}
git fetch origin -t
git_branch=($(git branch -a|grep 'remotes'|sed -e 's/^[[:space:]]*remotes\///'))
if [ "x${git_branch[0]}" == "x" ] ; then
	echo "Fetch tag failed, just fetch all the source code"
	git fetch origin
	git_branch=($(git branch -a|grep 'remotes'|sed -e 's/^[[:space:]]*remotes\///'))
fi
if [ "x${git_branch[0]}" == "x" ] ; then
	echo "No branch found in the remote repo, patch failed"
	rm -rf .git
	exit_proc 1
fi
git checkout -b master ${git_branch[0]}
git log --pretty -1 ${REPO_COMMIT}
if [[ ! $? -eq 0 ]] ; then
	echo "No commit related to ${REPO_COMMIT}, try to fetch more from remote"
	git fetch --depth 3000
fi
git checkout -b embARC ${REPO_COMMIT}
echo "Try to apply patch for this repo"
git am *.patch

exit_proc $?