.include "utils.s"

.equ LOOP_CNT     ,32
.equ ICACHE_ADDR  ,0x0010100

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
    li   a4, 0x270f34
    li   a5, 1
    sw   a5, (a4)
    li   x16, 0x4
    li   a2,0x204358
    li   sp,0x800fffbc
.L1:
    li  t0, 0x5
    addi a1, a1, 1
    addi a2, a2, 1
.L2:
    j .L3
    nop
    nop
.L3:
    bne  a1, a0, .L1

   
end:
    j exit