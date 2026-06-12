/*
;; riscv64-elf-gcc -march=rv32i -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
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
.org   ICACHE_ADDR
_start:
    initregs
    li   a4, LOOP_CNT
    la a7, _test2
.L1:
    nop
    nop
    nop
    jalr x1, 0(a7)
    bne  a1, a4, .L1
li a1, 0
.L2:
    nop
    nop
    nop
    jal x1, _test2
    bne  a1, a4, .L2
    j exit

.global _test1
.section .icache, "ax"
.type _test1, @function
_test1:
 nop
 nop
 addi a1, a1, 0
 add a6, a5, a3
 addi a1, a1, 0
 addi a1, a1, 1
 jr x2 /* rd != link, rs1 == link, pop, return */

.global _test1
.org 0x4000
.section .icache, "ax"
.type _test2, @function
_test2:
 nop
 nop
 nop
 nop
 nop
 addi a1, a1, 1
 jr x1 /* rd != link, rs1 != link */
 