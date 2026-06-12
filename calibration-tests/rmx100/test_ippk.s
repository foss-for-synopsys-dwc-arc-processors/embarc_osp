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
    addi  sp, sp,-32
    sw  s3,12(sp)
    sw  s2,16(sp)
    sw  s1,20(sp) 
    sw  s0,24(sp)
    slli    s1,t0,1
    /* This instruction moves a0 into r1s’ and a1 into r2s’. r1s’ and r2s’ must be different. 
    mv a0 -> s0(x8),
    mv a1 -> s2 (x18)
    */
    cm.mvsa01      x8,x18             
    addi a2,a2,1
    addi a1, a1, 1
    bne  a1, a0, .L1
end:
    j exit
