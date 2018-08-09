#!/bin/sh

die() {
    echo " *** ERROR: " $*
    exit 1
}

set -x

[ $TRAVIS_OS_NAME != linux ] || {
    bash -c "$STATUS" pending "Local $NAME testing is in progress" || die
    export PATH=/tmp/arc_gnu_${GNU_VER}_prebuilt_elf32_le_linux_install/bin:$PATH || die
    git checkout -- . || die
    bash apply_embARC_patch.sh || die
    cd .travis || die

    if [ $TOOLCHAIN == sphinx ] ; then
        bash deploy_doc.sh || die
    else
        BUILD_OPTS="OSP_ROOT=${OSP_ROOT} TOOLCHAIN=${TOOLCHAIN} BOARD=${BOARD} BD_VER=${BD_VER} CUR_CORE=${CUR_CORE} GNU_VER=${GNU_VER} EXAMPLES=${EXAMPLES} EXPECTED=${EXPECTED}"
        python build.py ${BUILD_OPTS} || die
    fi

}
