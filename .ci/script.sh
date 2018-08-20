#!/bin/sh
die() {
    echo " *** ERROR: " $*
    exit 1
}

set -x

TOOLCHAIN_CACHE_FOLDER=".cache/toolchain"
ARC_DEV_GNU_ROOT="/u/arcgnu_verif/gnu_builds"
ARC_DEV_MW_ROOT="/u/relauto/.TOOLS_ROOT/ToolsCommon/MWDT_eng/"

if [ "${TOOLCHAIN}" == "gnu" ]; then
    ARC_DEV_TOOL_ROOT="${ARC_DEV_GNU_ROOT}""/""${TOOLCHAIN_VER}""/elf32_le_linux"
else
    ARC_DEV_TOOL_ROOT="${ARC_DEV_MW_ROOT}""/mwdt_""${TOOLCHAIN_VER}""/linux/ARC"
fi
[ $TRAVIS == true ] && {
    if [ "${TOOLCHAIN}" == "gnu" ]; then

        python .ci/toolchain.py "${TOOLCHAIN_VER}" || die
        if [ -d $TOOLCHAIN_CACHE_FOLDER ] ;then
            if [ -d $TOOLCHAIN_CACHE_FOLDER/$TOOLCHAIN_VER ] ; then
                ARC_DEV_TOOL_ROOT="${TOOLCHAIN_CACHE_FOLDER}""/""${TOOLCHAIN_VER}"
            fi
        fi

    fi
}

if [ $TOOLCHAIN != sphinx ] ; then
    if [ -d $ARC_DEV_TOOL_ROOT ] ; then
        bash .ci/linux_env_set_arc.sh -t $TOOLCHAIN -r $ARC_DEV_TOOL_ROOT || die
        source arctool.env || die
    else
        echo "The toolchain path does not exist " 
        die
    fi
fi
git checkout -- . || die
bash apply_embARC_patch.sh || die
cd .ci || die
if [ $TOOLCHAIN == sphinx ] ; then
    bash deploy_doc.sh || die
else
    BUILD_OPTS="OSP_ROOT=${OSP_ROOT} TOOLCHAIN=${TOOLCHAIN} BOARD=${BOARD} BD_VER=${BD_VER} CUR_CORE=${CUR_CORE} TOOLCHAIN_VER=${TOOLCHAIN_VER} EXAMPLES=${EXAMPLES} EXPECTED=${EXPECTED}"
    python build.py ${BUILD_OPTS} || die
fi