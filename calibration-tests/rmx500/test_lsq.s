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
    li   sp, 0x831ff960
    li a1, LOOP_CNT
.L3:
    nop
    nop
    nop
    nop
    nop
    
    lw      a2,24(sp)
    lw      a3,28(sp)
    lb      s1,20(sp)
    sw      zero,72(sp)
    sw      zero,76(sp)
    sw      zero,80(sp)
    nop
    nop
    nop
    add  a2, a2, a3
    addi a4, a4, 1
    bne  a4, a1, .L3

end:
    j exit


