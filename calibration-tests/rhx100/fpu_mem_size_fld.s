/*
;; riscv64-elf-gcc  -march=rv32ic_zicsr_zifencei_zihintpause_a_zca_m_zcb_zcmp_zcmt_zba_zbb_zbc_zbs_zicond_zicbom_zicbop_zihintntl -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld test.s -o test.out
*/

.include "utils.s"

.equ LOOP_CNT     ,16
.equ ICACHE_ADDR  ,0x001100
.equ kFSInitial,1

.section ".text.init"
.globl   _vectors
.type    _vectors, @function
_vectors:
Reset:
   jal    _start
   ebreak


.globl _start
.text
_start:
  # -- Change FPU sate from Off to Initial
  csrr           t0, mstatus
  la             t1,kFSInitial << 13
  or             t0,t0,t1
  csrw           mstatus, t0

  mv  a2, zero
  csrr    a5,0x7c8     #_arcv_cache_enable
  lui     a4,0x11
  addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
  or      a5,a5,a4
  csrw    0x7c8,a5
  initregs
  li a5, 0x1000067e
  li   t2, LOOP_CNT

.L1:

    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    flw            ft1,(a5)
    flw            ft2,64(a5)
    flw            ft5,98(a5)
    fmv.x.w        x1,ft5
    fmv.x.w        x1,ft5
    addi a5, a5, 256
    addi a3, a3, 1
    bne  a3, t2, .L1

  initregs
  li a5, 0x1002067c
  li   t2, LOOP_CNT

.L2:

    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    fld            ft1,(a5)
    fld            ft2,64(a5)
    fld            ft5,98(a5)
    fadd.d         ft6,ft5,ft2
    fmv.x.w        x1,ft5
    fmv.x.w        x1,ft5
    addi a5, a5, 256
    addi a3, a3, 1
    bne  a3, t2, .L2

end:
    j exit
