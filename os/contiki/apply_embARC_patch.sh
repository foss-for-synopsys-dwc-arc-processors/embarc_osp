#!/usr/bin/env bash

PREV_DIR=$(pwd)
SCRIPT_DIR=$(dirname $0)

exit_ok=0
cd ${SCRIPT_DIR}

#### Apply TinyDTLS patches
echo "Try to apply TinyDTLS patches for Contiki"
patch_script="./apps/tinydtls/apply_embARC_patch.sh"
filetype=$(file ${patch_script} | grep -q CRLF && echo DOS || echo UNIX)
if [[ ${filetype} == DOS ]] ; then
	echo "Convert ${patch_script} from DOS to UNIX"
	dos2unix ${patch_script}
fi
chmod +x ${patch_script} && ${patch_script}
if [[ $? -eq 0 ]] ; then
	echo "Patch os/contiki/apps/tinydtls successfully"
else
	echo "Patch os/contiki/apps/tinydtls failed"
	exit_ok=1
fi

cd ${PREV_DIR}
exit ${exit_ok}