/*
;; riscv64-elf-gcc -march=rv32i -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
*/

.include "utils.s"

.equ LOOP_CNT     ,32
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
    li a2, LOOP_CNT
.L1:
    nop
    li sp, 0x00002000
    lw s6, 4(sp)
    add a6, s6, a6
    nop
    nop
    nop
    li sp, 0xc00fff78
    li s0, 0x800051e0
    li s3, 0x80004f78
    sw  s4,72(sp)
    sw  s2,80(sp)
    sw  s1,84(sp)
    sw  ra,92(sp)
    lw    a4,36(s0)
    sw  s0,24(sp)
    sw  a5,28(sp)
    sw      t1,-136(s3)
    addi a1,a1,0
    addi a3, a3, 1
    bne  a2, a3, .L1
end:
    j exit
