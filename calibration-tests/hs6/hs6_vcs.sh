#!/bin/bash

arch=32

top_dir=${PWD}
tcf_dir=tcf
trc_dir=trc

MDB=mdb${arch}
TCF=/SCRATCH/jingru/calibration/ncam-calibration/AV3/HS6X/tcf/arc.tcf
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

# -- COPY KEY RASCAL FILES
find /SCRATCH/jingru/calibration/ncam-calibration/AV3/HS6X/vcs -name 'rascal*' -print0 | xargs -0 -I _ cp _ ${exec_dir}
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
 ln -s /SCRATCH/jingru/calibration/ncam-calibration/AV3/HS6X/vcs/board.simv
 ln -s /SCRATCH/jingru/calibration/ncam-calibration/AV3/HS6X/vcs/board.simv.daidir
 # -- Link RASCAL trace file into temporary execution folder
 ln -s ${TRC} trace.txt
 ln -s ${TRC} combined_trace.txt

 # -- MODULES
 . ${MODULESHOME}/init/sh
 module purge
 module load architect/2023.03
 module load mwdt/2023.03

 echo -ne '\n' | $MDB  \
  -cl \
  -rascal \
  -nohostlink_while_running \
  -off=dual_issue  \
  -off=gverbose \
  -off=halt_on_prepare\
  -off=prefer_soft_bp \
  -off=query_target_for_fast_download \
  -on=rascal_counters \
  -on=ras_pipemon_trace  \
  -cmd="evaluate \$put_bank_reg(1,0x8,0x6)" \
  -cmd='emem 0xc0002000 2' \
  -cmd='evaluate $put_bank_reg(1,0x4f,0x00)' \
  -cmd="set PIPEMON_FILE_SIZE=4096" \
  -cmd='hwc on' \
  -cmd='evaluate \$put_bank_reg(1,0x255,0x1)' \
  -cmd='/SCRATCH/jingru/calibration/ncam-calibration/scripts/support/hwc.mdb' \
  -cmd=run \
  -cmd='display hwmet' \
  -cmd=exit \
  -- $ELF
 popd
) > ${OUT}.log
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
