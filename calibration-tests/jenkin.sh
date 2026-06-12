#!/bin/bash
set -o errexit -o pipefail -o nounset

ARCJENKINS_SSH_PORT=$(curl --insecure -Lv https://de02-arcjenkins2/login 2>&1 | grep -Po 'X-SSH-Endpoint.*:\K[0-9]+') || error "Can not get ARCJenkins ssh port"
USER=svc-dwc-sim  # need to add public key of nsim gitlab runner to https://de02-arcjenkins2/user/svc-dwc-sim/configure  SSH Public Keys
ARCJENKINS_URL='de02-arcjenkins2'
JOB_NAME='ncam-calibration-jingru/calibration'


platforms_to_test="vpx5_bosch_noecc
vpx2_vfpu_full
vpx3_vfpu_full
vpx5_int_full_v2
vpx5_vfpu_full_v2
vpx5_nikon
HS5XD
HS6X
RMX100
RHX100"

RECIPIENT_LIST="${CI_COMMIT_AUTHOR:-ncam-calibration@synopsys.com}"
#  -p NSIM_MR=${CI_MERGE_REQUEST_IID} send message to MR need token
params="-p PLATFORMS='${platforms_to_test}' -p NSIM_HOME= -p NSIM_BRANCH=${CI_COMMIT_REF_NAME}"
benchmarks_to_test="eembc
vdsp
embench
infineon"
compilers_to_test="gcc
ccac"

params+=" -p BENCHMARKS='${benchmarks_to_test}' -p COMPILERS='${compilers_to_test}' -p COMMENT='calibration with nSIM branch ${CI_COMMIT_REF_NAME}'"
params+=" -p TEST_BRANCH=master -p CUSTOM_OPTIONS='-j 4 --simulation ncam'"
params+=" -p RECIPIENT_LIST=${RECIPIENT_LIST}"

ssh -o "StrictHostKeyChecking no" -l $USER -p $ARCJENKINS_SSH_PORT $ARCJENKINS_URL build $JOB_NAME "$params" && echo -e "\nCommand is sent to ARCJenkins"
