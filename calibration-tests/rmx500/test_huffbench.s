/*
;; riscv64-elf-gcc -march=rv32i -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
*/

.include "utils.s"

.equ LOOP_CNT     ,64
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
    li   a2,0x204358
.L1:
    li  t0, 0x5
    addi a1, a1, 1
    addi a2, a2, 1
    bgeu    a2,a1,.L2
    nop
    nop
.L2:
    beq     a1,a2,end
    beq     a2,a0,end
    nop
    nop
    bne  a1, a0, .L1

   
end:
    j exit




