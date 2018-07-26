#!/bin/sh

die() {
    echo " *** ERROR: " $*
    exit 1
}

set -x


[ $TRAVIS_OS_NAME != linux ] || {
    sudo apt-get update || die
    sudo apt-get install lib32z1 || die
    sudo apt-get install dos2unix || die
    sudo apt-get install doxygen || die
    sudo pip install --upgrade pip || die
    sudo pip install git+https://github.com/sphinx-doc/sphinx || die
    sudo pip install breathe || die
    sudo pip install recommonmark || die
    sudo pip install sphinx_rtd_theme || die
    pip install PrettyTable || die
    pip install colorama || die
}
