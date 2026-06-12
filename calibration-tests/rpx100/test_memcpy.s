/*
;; riscv64-elf-gcc -march=rv32i -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
*/

.include "utils.s"

.equ LOOP_CNT     ,4
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
    initregs
    li   a0, LOOP_CNT
    li sp, 0x200f5c
    li gp, 0x10200af0
    li t3, 0x102012b0
    li a1, 16

  mv  a2, zero
  csrr    a5,0x7c8     #_arcv_cache_enable
  lui     a4,0x11
  addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
  or      a5,a5,a4
  csrw    0x7c8,a5

    li t1, 0x10020000
    li a3, 0
    li a1, 0x20
    li a5, 0x1004f7e0
    li a4, 0x1004f7e8
    nop
    nop
    nop
    nop
    nop
.L3:
    csrr t0, mcycle
    lw      t3,-52(a5)
    sw      t3,-52(a4)
    lw      t6,-48(a5)
    lw      t5,-44(a5)
    sw      t6,-48(a4)
    sw      t5,-44(a4)
    lw      a0,-40(a5)
    sw      a0,-40(a4)
    lw      t2,-36(a5)
    csrr t1, mcycle
    sub  t1, t1, t0
    nop
    nop
    nop
    nop
    addi a5, a5, 0x40
    addi a4, a4, 0x40

    addi a2, a2, 1
    bne  a2, a1, .L3
end:
    j exit


