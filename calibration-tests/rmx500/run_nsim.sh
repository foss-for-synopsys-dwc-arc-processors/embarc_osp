#!/bin/bash

export LM_LICENSE_FILE=26585@de02snpslmd:26585@de02arclmd1:27000@nl20rh01

top_dir=${PWD}
tcf_dir=tcf
trc_dir=trc
HOST=`hostname`
sim_dir=/slowfs/de02dwt2p011/tools/nsim/bleeding-edge/latest/Linux/nSIM_64/bin 
sim_dir=/SCRATCH/jingru/nsim/Tools/nsim/main/dev/build/out/nSIM_64/bin/
NSIM=${sim_dir}/nsimdrv
TCF=${top_dir}/${tcf_dir}/arc.tcf
TRC=$(mktemp ${top_dir}/${trc_dir}/nsim.XXXXXXXXXX)
# -- Benchmark default is CoreMark
if [ $# -eq 0 ] ; then
  ELF=$(find bmark/sanity -name '*.elf' | grep sanity.elf)
else
  ELF=$(cd "$(dirname "$@")" && pwd -P)/$(basename "$@")
fi
OUT=$(dirname ${TRC})/nsim.$(basename ${ELF})
# sim_dir=/SCRATCH/jingru/nsim/Tools/nsim/main/dev/build/out/nSIM_64/bin/
# Avoid hijacking of arguments
unset NSIM_ARGUMENTS

# TRACING OPTIONS
#  -on=trace                       ... enable functional trace
#  -on=trace-ncam                  ... enable NCAM trace
#
# PROFILING OPTIONS
#  -on=ncam_profiling             ... enable advanced NCAM profiling
#  -on=nsim_print_stats_on_exit   ... print profiling summary

# * NOTE:
#     - TCF IS PASSED AS THE FIRST ARGUMENT
#     - OTHER OPTIONS ARE PASSED NEXT (THAT WAY YOU CAN OVERRIDE TCF DEFAULTS)

# -- RUN SIMULATION IN SUB SHELL

rm -f ${OUT}.log
(
  $NSIM                                  \
  -tcf=/SCRATCH/jingru/calibration/ncam-calibration/AV5/RMX500/tcf/arc.tcf   -p nsim_isa_ext=-Zcmp.-Zcmt.d \
  -p nsim_semihosting=1 -on=ncam_profiling -on=trace -on=trace-ncam -on=cycles -on=nsim_ncam_experimental_option -off=invalid_instruction_interrupt    \
  -p nsim_system_cfg=0,400,1,64    \
  -p nsim_isa_dccm_ecc_option=1    \
  -p nsim_isa_rbd_option=0    \
  -p nsim_isa_dccm_mem_cycles=1 -p nsim_isa_adv_fusion_option=0  \
  -- $ELF -i1
) > ${OUT}.log 2>&1
#  > ${OUT}.log
# -p nsim_isa_ext=-all.i.zicsr.zifencei.zihintpause.a.c.m.u.zcb.zcmp.zcmt.zba.zbb.zbs.zicond.zicbom.zicbop.
# -p nsim_isa_ext=G.C.V.xsnpsudsp.Zb.Zc
# -- MOVE TRACE
mv -f ${TRC} ${OUT}
echo cat ${OUT}.log


