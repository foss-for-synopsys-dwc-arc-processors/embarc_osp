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
git fetch --depth 1 origin refs/tags/${REPO_COMMIT}:refs/tags/${REPO_COMMIT} || {
	echo "Fetch tag failed, try default branch"
	default_branch=$(git remote show ${REPO_LINK} | sed -n "s/^.*HEAD branch: \(.*\)$/\1/p")
	git fetch origin ${default_branch}
}

git checkout -b embARC ${REPO_COMMIT}
echo "Try to apply patch for this repo"
git am *.patch

exit_proc $?