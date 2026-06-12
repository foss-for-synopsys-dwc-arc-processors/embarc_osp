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


li a1, 0x0
li s1, 0xffff
li s2, 0x1000000
li a4, 0x0

.L1:
    nop
    mv x1, a4
    div a2, a4, a1  /* 0 / 0 ~ 0xffff > 0*/
    mv a3, a2
    addi a1, a1, 1
    bne  s1, a1, .L1

li a1, 0xfff0000
li s1, 0x10000001
li s2, 0x1000000
li a4, 0x0

.L2:
    nop
    nop
    mv x1, a4
    div a2, a4, a1  /* 0 / 0xfff0000 ~ 0x10000001*/
    mv a3, a2
    addi a1, a1, 1
    bne  s1, a1, .L2

li a1, 0xffff0000
li s1, 0xffffffff
li s2, 0x1000000
li a4, 0x0

.L3:
    nop
    nop
    mv x1, a4
    div a2, a4, a1  /* 0 / 0xfff0000 ~ 0x10000001*/
    mv a3, a2
    addi a1, a1, 1
    bne  s1, a1, .L3

li a1, 0x0
li s1, 0xffff
li s2, 0x1000000
li a4, 0x0

.L4:
    nop
    mv x1, a4
    div a2, a1, a4  /* 0 / 0 ~ 0xffff > 0*/
    mv a3, a2
    addi a1, a1, 1
    bne  s1, a1, .L4

li a1, 0xfff0000
li s1, 0x10000001
li s2, 0x1000000
li a4, 0x0


.L5:
    nop
    nop
    mv x1, a4
    div a2, a1, a4  /* 0 / 0xfff0000 ~ 0x10000001*/
    mv a3, a2
    addi a1, a1, 1
    bne  s1, a1, .L5

li a1, 0xffff0000
li s1, 0xffffffff
li s2, 0x1000000
li a4, 0x0
/* latency 5 .*/
.L6:
    nop
    nop
    mv x1, a4
    div a2, a1, a4  /* 0 / 0xfff0000 ~ 0x10000001*/
    mv a3, a2
    addi a1, a1, 1
    bne  s1, a1, .L6
end:
    j exit
