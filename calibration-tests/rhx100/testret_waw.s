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
    li a3, 32

    mv  a2, zero
    csrr    a5,0x7c8     #_arcv_cache_enable
    lui     a4,0x11
    addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
    or      a5,a5,a4
    csrw    0x7c8,a5

    li      a4,32
    lui     gp,0x201
    addi    gp,gp,1092
    li      a2,6
    li a5, 0x000003c5
    li t1, 0x200000
.L3:
    lw      s0,1916(gp)
    addi    t3,s0,3
    blt     gp,t3, end
    addi    t3,gp,1992
    lw      t2,48(t3)
    lw      t0,36(t3)
    blt     gp,t0, end
    sh2add  t0,a5,t1
    lw      s2,36(t0)
    nop
    nop

    jal x1, _test1

    nop
    bne  a1, a3, .L3
end:
    j exit




.global _test1
.section .icache, "ax"
.org 0x11e00
.type _test1, @function
_test1:
 lw      s0,48(t3)
 nop
 nop
 addi a1, a1, 1
 jr x1
