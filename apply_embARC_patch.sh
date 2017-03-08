#!/usr/bin/env bash

patches_required_folders="library,middleware,os"

patches_required_folder_list=(${patches_required_folders//,/ })

PREV_DIR=$(pwd)
SCRIPT_DIR=$(dirname $0)

if which dos2unix >/dev/null 2>&1; then :; else
    echo "You need dos2unix in your path" >&2
    exit 1
fi

cd ${SCRIPT_DIR}

echo "Start to download required source code and apply patches for embARC"
echo ""

exit_ok=0
for patch_folder in ${patches_required_folder_list[@]}
do
	patch_scripts=$(find $patch_folder -maxdepth 2 -name "apply_embARC_patch.sh")
	for script in ${patch_scripts[@]}
	do
		patch_dir=$(dirname ${script})
		echo "+++++Try to patch ${patch_dir}+++++"
		echo "Run script: chmod +x ${script} && ${script}"
		filetype=$(file ${script} | grep -q CRLF && echo DOS || echo UNIX)
		if [[ ${filetype} == DOS ]] ; then
			echo "Convert ${script} from DOS to UNIX"
			dos2unix ${script}
		fi
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
fi
echo "Patch job ended"
cd ${PREV_DIR}
exit ${exit_ok}