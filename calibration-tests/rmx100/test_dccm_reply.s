/*
;; riscv64-elf-gcc -march=rv32i -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
*/

.include "utils.s"

.equ LOOP_CNT     ,4
.equ ICACHE_ADDR  ,0x000678

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
    li a5, 0x800008f9
    li a4, 0x801816f8
    li a6, 0x40000
    li s1, 0x8000bbd4
    li sp, 0x800ff680
    sw s1, (sp)
    li t2, 0x8003c928
.L1:
    addi    t2, t2, 8
    addi a2, a2, 1
    sb      t5,3(t2)         /*                      st 62 -> [8003c933]*/
    sub     t3,a0,t5         /*  w0: t3 <- 00000000*/
    sb      t3,0(t2)         /*                      st 0 -> [8003c930]*/
    sub     s6,t6,t5         /*  w0: s6 <- 00000011*/
    sb      s6,1(t2)         /*                      st 11 -> [8003c931]*/
    sub     a1,s5,t5         /*  w0: a1 <- 00000054*/
    sb      a1,2(t2)         /*                      st 54 -> [8003c932]*/
    addi    t2, t2, 8
    li t6, 0xb1
    li s5, 0xd1
    li t5, 0xa0
    mv t3, zero
    sb      t3,-4(t2)
    sub     s6,t6,t5
    sb      s6,-3(t2)
    sub     a3,s5,t5
    sb      a3,-2(t2)
    sb      t5,-1(t2)         /** multi-match, replay, need re-fetch, and unaligned fetch */
    li t0, 0x800ffe24
    li t3, 0x80004954
    nop
    nop
    nop
    nop
    nop
    nop
    
    lw      a0,0(t0)
    lw      t5,32(t0)
    lw     a4,0(t3) 
    lw      a6,4(t3)          /* 2 lsq load pop at same time, stall 2 cycle */
    bne  a2, a1, .L1

end:
    j exit


