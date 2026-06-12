#!/bin/bash

arch=32

top_dir=${PWD}
tcf_dir=tcf
trc_dir=trc

# TCF=/SCRATCH/jingru/ncam/ncam-calibration/AV3/HS6X/tcf/arc.tcf
TRC=$(mktemp ${top_dir}/${trc_dir}/vcs.XXXXXXXXXX)

# -- ELF binary
if [ $# -eq 0 ] ; then
  ELF=${top_dir}/$(find bmark/sanity -name '*.out' | grep sanity.out)
else
  ELF=$(cd "$(dirname "$@")" && pwd -P)/$(basename "$@")
fi
OUT=$(dirname ${TRC})/vcs.$(basename ${ELF})

# -- EXEC
exec_dir=$(mktemp -d ${top_dir}/.mdb.vcs.XXXXXXXXXX)
VCS_ROOT=/SCRATCH/jingru/calibration/calibration-tests/rmx100/ifx_mini_rattler.txt0/build
VCS_ROOT=/SCRATCH/jingru/calibration/ncam-calibration/AV5/RMX100/vcs
# -- COPY KEY RASCAL FILES
find $VCS_ROOT -name 'rascal*' -print0 | xargs -0 -I _ cp _ ${exec_dir}
# -- RUN SIMULATION IN SUB SHELL
rm -f ${OUT}.log
(
 pushd () {
  command pushd "$@" > /dev/null
 }
 popd ()  {
  command popd "$@" > /dev/null
 }

 pushd ${exec_dir}
 # -- Link VCS into temporary execution folder
 ln -s $VCS_ROOT/board.simv
 ln -s $VCS_ROOT/board.simv.daidir
  ln -s $VCS_ROOT/board.json
 ln -s $VCS_ROOT/.preload
 # -- Link RASCAL trace file into temporary execution folder
 ln -s ${TRC} trace.txt
 ln -s ${TRC} combined_trace.txt
export PIPEMON_ENABLE_PIPELINE=1
 # -- MODULES
 . ${MODULESHOME}/init/sh
 module purge
 module load architect
 module load mwdt
  export PATH=/remote/de02sgnfs00032/arc_lldb/svc-arcdbg/eng_builds/latest/standalone/linux/ARC/MetaWare/arc/bin:$PATH
 lldbac -o "platform connect board.json" -o "process launch -s" -o c -o exit -- $ELF
 popd
)
if test -d ${exec_dir} ; then rm -rf ${exec_dir}; fi

# -- CONVERT VCS TRACE
rm -f ${OUT}
#${CNV} < ${TRC} > ${OUT}
#${CNV} < ${TRC} > ${OUT}
#rm -f ${TRC}
cat ${OUT}.log >> ${OUT}
cat $TRC >> $OUT
rm -rf $TRC
echo cat ${OUT}

