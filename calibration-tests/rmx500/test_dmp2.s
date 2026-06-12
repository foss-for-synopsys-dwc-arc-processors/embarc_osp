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
    li a1, LOOP_CNT
    li a5, 0x800816f0
    li a4, 0x801816f8
    li a6, 0x40000
    li s1, 0x8000bbd4
    li sp, 0x800ff680
    sw s1, (sp)
.L3:
    nop
    nop
    nop
    nop
    
    lw      s1,0(sp)
    lw      t0,-16(sp)
    lw      s6,0(sp)
    sh2add  s4,t2,s1
    sh2add  t4,t2,t0
    lw      s2,0(s4)
    nop
    nop
    nop
    nop
    nop
    nop
    li a5, 0x800fec60
    li t0, 0x800fe2b7
    sw      t1,0(a5)
    sb      t4,1(t0)
    nop
    nop
    lbu     t5,-1(t0)
    lw      s7,-12(a5)
    mv    a4,s11
    addi    s6,a3,-3
    sw      s3,-4(a5)
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    sw      s3,40(a5)
    lw      s7,-11(t0)
    mv      s3, s7
    nop
    nop
    nop
    nop
    nop
    sw      s3,40(a5)
    nop
    lw      s7,-11(t0)
    mv      s3, s7
    nop
    nop
    nop
    nop
    nop
    sw      s3,40(a5)
    nop
    nop
    lw      s7,-11(t0)
    mv      s3, s7
    nop
    nop
    nop
    nop
    nop
    sw      s3,40(a5)
    nop
    nop
    nop
    lw      s7,-11(t0)
    mv      s3, s7
    nop
    nop
    nop
    nop
    nop
    sw      s3,40(a5)
    nop
    nop
    nop
    nop
    lw      s7,-11(t0)
    mv      s3, s7
    li t2, 0x8003c930
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    sb      t3,-4(t2)
    sub     s6,t6,t5
    sb      s6,-3(t2)
    sub     a1,s5,t5
    sb      a1,-2(t2)
    sb      t5,-1(t2)
    li a1, LOOP_CNT
    add  a2, a2, a3
    addi a2, a2, 1
    bne  a2, a1, .L3

end:
    j exit


