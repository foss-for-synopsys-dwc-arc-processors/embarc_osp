# -------------------------------------------------------------------------------
# /SCRATCH/nsim_tools_rv/toolchain/riscv32-embecosm-centos7-gcc12.1.0/bin/riscv32-unknown-elf-gcc -march=rv32g fpu2fdiv.s -nostdlib -T ../../../../../link/link.ld -o fpu2fdiv.elf
# -------------------------------------------------------------------------------
# This tests latency between fpu and fdiv

.equ LOOPTIMES, 10
.equ kFSInitial,1
.equ DCCM_SEG,0x00200000

.include "../../utils.s"

.macro delay
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
.endm

.global _start
_start:
  initregs
  # -- Change FPU sate from Off to Initial
  csrr           t0, mstatus
  la             t1,kFSInitial << 13
  or             t0,t0,t1
  csrw           mstatus, t0


  # a loop equavalent to "for (int t0 = 0; t0 < 10; t0++)"
  li             t0,0
  li             t1,LOOPTIMES

  li             t2,2
  li             t3,5
  li             t4,10
  li             t5,3
  fcvt.s.w       ft0,t2 # initialize ft0 to 2
  fcvt.s.w       ft1,t3 # initialize ft1 to 5
  fcvt.s.w       ft2,t4 # initialize ft2 to 10
  fcvt.s.w       ft3,t5 # initialize ft3 to 9

  li             s1,DCCM_SEG
  li             t6,0x6161f1f1
  sw             t6,(s1)
  fcvt.s.w       ft7,t6 # initialize ft7 to 0x6161f1f1

  loop_head:
  bge            t0,t1,loop_end
  addi           t0,t0,1           # increment loop counter
  nop                              # add some nop to eliminate possible noise

  # test fpu instructions with fdiv (fdiv.s)
  # producer : FADD (3 stalls)
  fadd.s         ft5,ft0,ft1
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  fsub.s         ft5,ft0,ft1
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  # producer : FMUL (3 stalls)
  fmul.s         ft5,ft0,ft1
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  # producer : FMADD (3 stalls) 
  fmadd.s        ft5,ft0,ft1,ft3
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  fmsub.s        ft5,ft0,ft1,ft3
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  fnmadd.s       ft5,ft0,ft1,ft3
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  fnmsub.s       ft5,ft0,ft1,ft3
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  # producer : FDIV (depends on operands)
  fdiv.s         ft5,ft2,ft1
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  fsqrt.s        ft5,ft3
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  # producer : FSMPL (1 stall)
  fmin.s         ft5,ft0,ft1
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  fmax.s         ft5,ft0,ft1
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  fsgnj.s        ft5,ft0,ft1
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  fsgnjn.s       ft5,ft0,ft1
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  fsgnjx.s       ft5,ft0,ft1
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  # producer : FMV.P (1 stall)
  fmv.w.x        ft5,t6
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  # producer : FCVT (2 stalls)
  fcvt.s.w       ft5,t6
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  fcvt.s.wu      ft5,t6
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay

  # producer : FLD (0 stalls)
  flw            ft5,(s1)
  fdiv.s         ft6,ft7,ft5
  fmv.x.w        x1,ft6
  delay 

  nop                           # add some nop to eliminate possible noise
  nop
  j loop_head
  loop_end:

  #-- check FPU state
  csrr t0, mstatus
  j exit

