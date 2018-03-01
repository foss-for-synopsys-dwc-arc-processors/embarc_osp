#!/bin/sh

die() {
	echo " *** ERROR: " $*
	exit 1
}

set -x

cd /tmp || die

[ $TRAVIS_OS_NAME != linux ] || {
    sudo apt-get update || die
    sudo apt-get install lib32z1 || die
    sudo apt-get install dos2unix || die
    wget https://github.com/foss-for-synopsys-dwc-arc-processors/toolchain/releases/download/arc-2017.09-release/arc_gnu_2017.09_prebuilt_elf32_le_linux_install.tar.gz || die
    tar xzf arc_gnu_2017.09_prebuilt_elf32_le_linux_install.tar.gz || die
    export PATH=/tmp/arc_gnu_2017.09_prebuilt_elf32_le_linux_install/bin:$PATH || die
    arc-elf32-gcc --version || die
}
