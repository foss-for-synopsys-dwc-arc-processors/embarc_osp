/*
;; riscv64-elf-gcc -march=rv32i -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
*/

.include "utils.s"

.equ LOOP_CNT     ,16

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
    li s10, 0xcccccccd
    li a5, 0xccccd000
    li x2, LOOP_CNT
    li x10, 0x200010

.L1:
    mul x3, x1, x2
    sw  x3, (x10)
    mul x4, x1, x2
    add x5, x4, x4
    addi x1, x1, 1
    bne  x1, x2, .L1
end:
    j exit