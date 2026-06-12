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
li s1, 0xffffffff
li s2, 0x20000000
li a4, 0x10000000

.L1:
    nop
    .L2:
        nop
        nop
        div a2, a4, a1  /* 0x10000000~ 0x20000000/ 0 ~ 0xffffffff*/
        mv a3, a2
        addi a1, a1, 1
        bne  s1, a1, .L2

    div a2, a4, a1 
    mv a3, a2
    li a1, 0
    nop
    nop
    nop
    addi a4, a4, 1
    bne  s2, a4, .L1

    div a2, a4, a1 
    mv a3, a2
end:
    j exit
