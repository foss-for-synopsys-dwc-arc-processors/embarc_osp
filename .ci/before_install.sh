#!/bin/sh

die() {
    echo " *** ERROR: " $*
    exit 1
}

if [ "${TRAVIS}" == "true" ] ; then
    [ "${TRAVIS_OS_NAME}" != "linux" ] || {
        if [ "$STATUS" != "" ] && [ "$NAME" != "" ] ; then
            bash -c "$STATUS" pending "Local $NAME testing is in progress" || \
            die "Not able to post status to github, did you set EMBARC_BOT variable in travis ci setting page"
        fi
        sudo apt-get update || die
        sudo apt-get install lib32z1 || die
        sudo apt-get install dos2unix || die
        sudo apt-get install doxygen || die
    }
fi

{
    pip install --upgrade pip || die
    [ "${TRAVIS}" == "true" ] && {
        pip install Sphinx || die
        pip install breathe || die
        pip install recommonmark || die
        pip install sphinx_rtd_theme || die   
    }
    pip install GitPython || die
    pip install embarc_cli || die
    pip install configparser || die
    pip install requests || die
    pip install XlsxWriter || die
}
