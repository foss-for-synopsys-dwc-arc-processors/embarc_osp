#!/bin/bash

export LM_LICENSE_FILE=26585@de02snpslmd:26585@de02arclmd1:27000@nl20rh01

top_dir=${PWD}
tcf_dir=tcf
trc_dir=trc
sim_dir=/slowfs/de02dwt2p011/tools/nsim/bleeding-edge/latest/Linux/nSIM_64/bin/
export NSIM_HOME=/SCRATCH/jingru/nsim/Tools/nsim/main/dev/build/out/nSIM_64

# export NSIM_HOME=/slowfs/de02dwt2p011/tools/nsim/bleeding-edge/latest/Linux/nSIM_64
export PATH=$NSIM_HOME/bin:$PATH
export LD_LIBRARY_PATH=$NSIM_HOME/lib:$LD_LIBRARY_PATH
NSIM=${sim_dir}/nsimdrv
TCF=/SCRATCH/jingru/calibration/ncam-calibration/AV3/HS5XD/tcf/arc.tcf
TRC=$(mktemp ${top_dir}/${trc_dir}/nsim.XXXXXXXXXX)

if [ $# -eq 0 ] ; then
  ELF=$(find bmark/sanity -name '*.out' | grep sanity.out)
else
  ELF=$(cd "$(dirname "$@")" && pwd -P)/$(basename "$@")
fi
OUT=$(dirname ${TRC})/nsim.$(basename ${ELF})
# Avoid hijacking of arguments
unset NSIM_ARGUMENTS

source /global/etc/modules.sh
 module purge
 module load gcc/8.2.0
 module load architect
 module load mwdt
rm -f ${OUT}.log
 # -iccm0_base=0 -iccm0_size=0x40000              \
{
( echo -ne '\n' | mdb64                                  \
  -cl -nsim -nohostlink_while_running -tcf=${TCF}                          \
  -on=nsim_has_l0_dcache \
  -prop=nsim_l0_dcache_size=2048       \
  -prop=nsim_l0_dcache_assoc=1         \
  -prop=nsim_isa_eco=6                 \
  -prop=nsim_system_cfg=12,15,1,128     \
  -on=nsim_isa_dual_issue_option       \
  -off=nsim_isa_is_hw_pf_enabled        \
  -off=nsim_print-sys-arch             \
  -on=trace                            \
  -off=nsim_trace-64bit-pc             \
  -on=trace-ncam                      \
  -off=trace-multi-line                \
  -off=nsim_hostlink                    \
  -on=nsim_isa_unaligned_option        \
  -on=nsim_ncam_experimental_option    \
  -on=nsim_ncam_profiling              \
  -prop=nsim_props_output=properties.props \
  -on=nsim_print_stats_on_exit         \
  -prop=nsim_table_format=txt          \
  -prop=nsim_table_directory=log       \
  -prop=nsim_isa_has_interrupts=1      \
  -prop=nsim_isa_number_of_interrupts=5 \
  -prop=nsim_isa_lpb_num=0 \
  -cmd='hwc on' \
  -cmd='read /SCRATCH/jingru/ncam/ncam-calibration/scripts/support/hwc.mdb' \
  -cmd=run \
  -cmd='display hwmet' \
  -cmd=exit \
  -- $ELF
)
} 2>&1 > ${OUT}
# -- MOVE TRACE
#mv -f ${TRC} ${OUT}
echo cat ${OUT}
