#!/bin/sh -eux

readonly _DIR="$(readlink -e $(dirname $(readlink -e $0)))"

die() {
    echo " *** ERROR: " $*
    exit 1
}

_get_repo_revision() {
    if [[ -d $1 ]]; then
        cd $1
        repo_hash_commit=$(git rev-parse HEAD)
        echo -n ${repo_hash_commit}
        cd $OLDPWD
    else
        echo "[Error] $1 doesn't exist"
    fi
}

prepare_env() {
    out_dir="${_DIR}/out"
    logs_dir="${_DIR}/logs"
    test_log="$logs_dir/test.log"

    mkdir -p $logs_dir $out_dir
    : >$test_log

    TOOLCHAIN_CACHE_FOLDER=".cache/toolchain"

    [ "${TRAVIS}" == "true" ] && {
        if [ "${TOOLCHAIN}" == "gnu" ] ; then
            python .ci/toolchain.py -v $TOOLCHAIN_VER -c $TOOLCHAIN_CACHE_FOLDER  || die
            if [ -d $TOOLCHAIN_CACHE_FOLDER ] ; then
                if [ -d $TOOLCHAIN_CACHE_FOLDER/$TOOLCHAIN_VER ] ; then
                    ARC_DEV_TOOL_ROOT="${TOOLCHAIN_CACHE_FOLDER}/${TOOLCHAIN_VER}"
                fi
            fi
        else
            die "Toolchain ${TOOLCHAIN} not supported in travis ci"
        fi
    }

    if [ -d $ARC_DEV_TOOL_ROOT ] ; then
        REAL_ARC_DEV_ROOT=$(readlink -f ${ARC_DEV_TOOL_ROOT})
		export ARC_GNU_ROOT=${REAL_ARC_DEV_ROOT}
		export PATH=${REAL_ARC_DEV_ROOT}/bin:${PATH}
        arc-elf32-gcc -v || die "ARC GNU toolchain is not installed correctly"
    else
        die "The toolchain path ${ARC_DEV_TOOL_ROOT} does not exist "
    fi
    git checkout -- . || die

    echo "Using ${TOOLCHAIN}-${TOOLCHAIN_VER}" || die
    . apply_embARC_patch.sh
}

run_test() {
    local target_flags=""
    if [[ ${BD_VER:-} ]]; then
        target_flags="$target_flags --platform-version $BD_VER"
    fi
    export PYTHONUNBUFFERED=1
    {
        hostname
        python ./scripts/test.py --platform $BOARD --toolchain $TOOLCHAIN $target_flags --build-only --report-dir logs_dir || {
            if [[ $? == 124 ]]; then
                echo "twister timeout"
            fi
            true
        }
    } 2>&1 | tee -a $test_log
}

parse_logs() {
    # get rid of MAC \r and change it to UNIX \n
    sed -i 's/\r/\n/g' $test_log
    # use sed remove ANSI color codes. \x1B is the escape special character, \[ is the second character of the escape sequence, [0-9] is the color value(s) regex
    sed -i -r 's/\x1B\[([0-9]{1,3}(;[0-9]{1,2})?)?[mGK]//g' $test_log
    failed=$(cat $test_log | sed -n 's/^.*, \([0-9]\+\) .*failed.*$/\1 /p')
    embarc_osp_repo_commit=$(_get_repo_revision ${_DIR})
    COMMENT=""
    if ! [[ $failed == 0 ]]; then
        comment_url="https://api.github.com/repos/$TRAVIS_REPO_SLUG/commits/$embarc_osp_repo_commit/comments"
        for line in $(grep "build status:" $test_log | sed -E 's/^.* (.*) failed.*$/\1/'); do
            COMMENT="$COMMENT<br>$line"
        done
        curl --user "$EMBARC_BOT" --request POST https://api.github.com/repos/$TRAVIS_REPO_SLUG/commits/$embarc_osp_repo_commit/comments \
        --data '{"body":"'${COMMENT}'"}'
    fi
}

main() {
    prepare_env
    run_test
    parse_logs
}

main
