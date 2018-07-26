#!/bin/sh

die() {
    echo " *** ERROR: " $*
    exit 1
}

set -x

[ $TRAVIS_OS_NAME != linux ] || {
    export PATH=/tmp/arc_gnu_${GNU_VER}_prebuilt_elf32_le_linux_install/bin:$PATH || die
    git checkout -- . || die
    bash apply_embARC_patch.sh || die
    cd .travis || die

    [ $TOOLCHAIN != sphinx -o $BOARD != none -o $OSP_ROOT != none -o $GNU_VER != none ] || {
        bash deploy_doc.sh || die
    }
    [ $TOOLCHAIN == sphinx -o $BOARD != emsk -o $OSP_ROOT == none -o $GNU_VER == none ] || {

        python build.py "OSP_ROOT=${OSP_ROOT} TOOLCHAIN=${TOOLCHAIN} BOARD=${BOARD} BD_VER=${BD_VER} CUR_CORE=${CUR_CORE} GNU_VER=${GNU_VER}" || die
    }
}
