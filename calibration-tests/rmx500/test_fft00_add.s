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
.org   ICACHE_ADDR
_start:
    initregs
    li   a0, LOOP_CNT
    li   x17, 0x73
    li a1, LOOP_CNT
    li a5, 0x800816f0
    li a4, 0x801816f8
    li a6, 0x40000
    li s1, 0x800ffae0
.L3:
    li t6, 0x800ffae2
    nop
    nop
    nop
    sh      t2,0(t6)
    lhu     t6,0(s1)
    add     a5,a5,t0
    sext.h  a5, a5
    add   t5,t5,t6
    nop
    nop
    li t6, 0x800ffae4
    sh      t2,0(t6)
    lhu     t6,0(s1)
    add     a5,a5,t0
    sext.h  a5, a5
    add   t5,t5,t6
    nop
    nop
    add  a2, a2, a3
    addi a2, a2, 1
    bne  a2, a1, .L3

end:
    j exit


