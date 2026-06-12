/*
;; riscv64-elf-gcc  -march=rv32ic_zicsr_zifencei_zihintpause_a_zca_m_zcb_zcmp_zcmt_zba_zbb_zbc_zbs_zicond_zicbom_zicbop_zihintntl -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
*/

.include "utils.s"

.equ LOOP_CNT     ,16
.equ ICACHE_ADDR  ,0x001100

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

  mv  a2, zero
  csrr    a5,0x7c8     #_arcv_cache_enable
  lui     a4,0x11
  addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
  or      a5,a5,a4
  csrw    0x7c8,a5
  initregs
  li a5, 0x1002cc8c
  li   t1, LOOP_CNT

.L1:
    add     t6,t3,t4
    lhu     a4,-12(a5)
    add     a0,t6,a1
    add     t2,a0,t0
    # lhu     t6,-8(a5)
    # lhu     t3,-10(a5)
    add     s0,t2,t5
    nop
    lhu     a1,-38(a5)
    lhu     t5,-36(a5)
    add     a7,s0,a4
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
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    addi a5, a5, 0x20
    addi a3, a3, 1
    bne  a3, t1, .L1

end:
    j exit