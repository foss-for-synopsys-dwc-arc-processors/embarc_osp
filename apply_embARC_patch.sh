#!/usr/bin/env bash
dir="${BASH_SOURCE[0]}"

if uname | grep -q "MINGW"; then
    win_build=1
    pwd_opt="-W"
else
    win_build=0
    pwd_opt=""
fi

# identify OS source tree root directory
export EMBARC_ROOT=$( builtin cd "$( dirname "$dir" )" > /dev/null && pwd ${pwd_opt})
unset pwd_opt

patches_required_folders="library,middleware,os"

patches_required_folder_list=(${patches_required_folders//,/ })

PREV_DIR=$(pwd)
SCRIPT_DIR=$(dirname $0)

cd ${SCRIPT_DIR}

echo "Start to download required source code and apply patches for embARC"
echo ""

exit_ok=0
for patch_folder in ${patches_required_folder_list[@]}
do
	patch_scripts=$(find ${EMBARC_ROOT}/${patch_folder} -maxdepth 2 -name "apply_embARC_patch.sh")
	for script in ${patch_scripts[@]}
	do
		patch_dir=$(dirname ${script})
		echo "+++++Try to patch ${patch_dir}+++++"
		chmod +x ${script} && ${script}
		if [[ $? -eq 0 ]] ; then
			echo "-----Patch ${patch_dir} successfully-----"
		else
			echo "-----Patch ${patch_dir} failed-----"
			exit_ok=1
		fi
		echo ""
	done
done
echo ""
if [[ ${exit_ok} -eq 0 ]] ; then
	echo "Apply patches for embARC successfully"
else
	echo "Apply patches for embARC failed"
	exit ${exit_ok}
fi
echo "Patch job ended"
cd ${PREV_DIR}
