/*
;; riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
*/

.include "utils.s"

.equ LOOP_CNT     ,5
.equ ICACHE_ADDR  ,0x00010000

.section ".text.init"
.globl   _vectors
.type    _vectors, @function
_vectors:
Reset:
   jal    _start
   ebreak


.globl _start
.text
/*.org   ICACHE_ADDR*/
_start:
    li sp,0x10008000
    li  x3, LOOP_CNT
    li x5, 0x40
    li x6, 1
.L1:
    li ra,0x1000037a
    nop
    mul sp, sp, x6
    cm.push {ra, s0-s2},-64
    sub sp,sp,x5
    nop
    nop
    mul sp, sp, x6
    cm.pop {ra, s0-s2},64
    nop
    nop
    cm.pop {ra, s0-s2},64
    mv s0, x3
    nop
    nop
    cm.pop {ra, s0-s2},64
    mv s3, ra
    nop
    nop
    cm.pop {ra, s0-s2},64
    li x6,1
    addi x4,x4,1
    bne  x4,x3, .L1
    li x2, 0 
end:
    j exit

