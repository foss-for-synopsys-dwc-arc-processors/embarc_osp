source /global/etc/modules.sh
module load mwdt
module load architect
export NSIM_HOME=/slowfs/de02dwt2p011/tools/nsim/bleeding-edge/latest/Linux/nSIM_64
export NSIM_HOME=/SCRATCH/jingru/nsim/Tools/nsim/main/dev/build/out/nSIM_64
export PATH="${NSIM_HOME}/bin:$PATH"
export LD_LIBRARY_PATH="${NSIM_HOME}/lib:$LD_LIBRARY_PATH"
module load gcc/8.2.0
module add vcs verdi
export NCAM_CALIBRATION_ROOT=/SCRATCH/jingru/calibration/ncam-calibration
#ARCHITECT=/SCRATCH/jingru/ncam/calibration-tests/rv/tom/architect
BLEED=/slowfs/de02dwt2p011/tools/nsim/bleeding-edge
export LM_LICENSE_FILE=26585@de02snpslmd:26585@de02arclmd1:27000@nl20rh01
OBJ= ; M=32
#export LD_LIBRARY_PATH=$ARCHITECT/lib/linux$M:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$BLEED/latest/Linux/nSIM$OBJ/lib:$LD_LIBRARY_PATH
export PATH="/depot/Python/Python-3.7.0/bin/:${PATH}"
export LD_LIBRARY_PATH="/depot/Python/Python-3.7.0/lib/:${LD_LIBRARY_PATH:-}"
export PATH=/SCRATCH/jingru/ncam/calibration-tests/rv/test-out:$PATH
export GNU_ROOT=/u/svc-arcoss_auto/gnu_builds/toolchain-daily-build/latest/riscv64-unknown-elf/riscv64-unknown-elf/bin
export PATH=/u/svc-arcoss_auto/gnu_builds/toolchain-daily-build/latest/riscv64-unknown-elf/riscv64-unknown-elf/bin:$PATH
# export PATH=/remote/de02sgnfs00032/arc_lldb/svc-arcdbg/eng_builds/latest/standalone/linux/ARC/MetaWare/arc/bin:$PATH
export CAMGRIND_ROOT=/SCRATCH/jingru/calibration/camgrind
export PATH=/SCRATCH/jingru/calibration/calibration-tests/rhx100:$PATH
export PATH=$PATH:~/.local/bin
module load mwdt
#62da0cc67256111ac7436fb3eb9f059c04abd4bc
#export PIPEMON_FILE_SIZE=40960000
# ln -s /u/svc-arcoss_auto/gnu_builds/toolchain-daily-build/latest/riscv64-unknown-elf/riscv64-unknown-elf/bin/riscv64-elf-objdump elfdump
# ln -s /u/svc-arcoss_auto/gnu_builds/toolchain-daily-build/latest/riscv64-unknown-elf/riscv64-unknown-elf/bin/riscv64-elf-objdump objdump
# ln -s /u/svc-arcoss_auto/gnu_builds/toolchain-daily-build/latest/riscv64-unknown-elf/riscv64-unknown-elf/bin/riscv64-elf-addr2line addr2line
# python /SCRATCH/jingru/calibration/ncam-calibration/scripts/handle_results/diff.py --left-trace /SCRATCH/jingru/calibration/calibration-tests/rhx100/trc/nsim.ip_pktcheckb1m_lite.out.log --left-sim ncam --right-trace /SCRATCH/jingru/calibration/calibration-tests/rhx100/trc/nsim.ip_pktcheckb1m_lite1.out.log --right-sim ncam --binary /SCRATCH/jingru/calibration/ncam-calibration/AV5/RHX100/bmark/stage2/semihosting/gcc/eembc/networking/bin_lite/ip_pktcheckb1m_lite.out
# sed -i '/10001d04/,$!d' autcor00data_1_ncam.txt
# sed -i '/10001d04/,$!d' autcor00data_1_vcs.txt
# sed -i '/10001d2c/,$d' autcor00data_1_ncam.txt
# sed -i '/10001d2c/,$d' autcor00data_1_vcs.txt
# scp jingru@arcdev9:/remote/us01sgnfs00702/ncam-calibration/NCAM_TOOLS/trace/RHX100/ncam/tcpbulk_lite/trace.txt ncamtcpbulk_lite.txt
# scp jingru@arcdev9:/remote/us01sgnfs00702/ncam-calibration/NCAM_TOOLS/trace/RHX100/vcs/tcpbulk_lite/trace.txt vcstcpbulk_lite.txt
# python /SCRATCH/jingru/calibration/ncam-calibration/scripts/calibrationlib/handle_results/diff.py --left-trace /SCRATCH/jingru/calibration/calibration-tests/rhx100/autcor00data_1_ncam.txt --right-trace /SCRATCH/jingru/calibration/calibration-tests/rhx100/autcor00data_1_vcs.txt --right-sim xcam --binary /SCRATCH/jingru/calibration/ncam-calibration/AV5/RHX100/bmark/stage2/semihosting/gcc/eembc/telecom/bin/autcor00data_1.out --frequency 400 --left-sim ncam
