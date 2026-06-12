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
    li   x25, 0xe05
    li   x16, 0xe
    li   x17, 0x73
    li a4, 0x80080044
    li a5, LOOP_CNT
    li sp, 0x8007ff5c
.L1:
    nop
    li sp, 0x00002000
    lw s6, 4(sp)
    add a6, s6, a6
    nop
    nop
    nop
    nop
    li gp, 0x80000800
    li sp, 0x8007ff5c
    sb      s6,-163(gp)
    sb      a6,-167(gp)
    sb      a6,-165(gp)
    lw      a6,28(sp)
    lbu     a0,-176(gp)
    sb      s2,-169(gp)
    sb      s2,-171(gp)
    lbu     a1,-179(gp)
    sb      a6,-172(gp)
    lbu     a3,-191(gp)
    lbu     s4,-199(gp)
    nop
    nop
    nop
    nop
    sw      t2,-36(a4)
    sw      t0,-32(a4)
    sw      t6,-28(a4)
    sw      t5,-24(a4)
    sw      t4,-20(a4)
    lw      t0, (a4)
    mv      t2,t0
    lw      t0, (a4)
    lw      t2, -36(a4)
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    lw      t2,-36(a4)
    lw      t0,-32(a4)
    lw      t6,-28(a4)
    lw      t5,-24(a4)
    lw      t4,-20(a4)
    nop
    nop
    nop
    nop
    nop
    sw      t2,-36(a4)
    lw      t4,-20(a4)
    mv      t2, t4
    sw      t0,-32(a4)
    sw      t6,-28(a4)
    addi a1,a1,0
    addi a2, a2, 1

    initregs
    li   a0, LOOP_CNT
    li   x17, 0x73
    li a2, LOOP_CNT
.L2:
    nop
    li sp, 0x00002000
    lw s6, 4(sp)
    add a6, s6, a6
    nop
    nop
    nop
    li sp, 0x800fff78
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
    nop
    nop
    nop
    nop
    nop
    nop
    addi a1,a1,0
    addi a3, a3, 1
    sw  a5,28(sp)
    lw  a1,28(sp)
    beq a4, a1, .L5
    nop
    nop
    nop
    nop
.L5:
    nop
    bne  a2, a3, .L2
    # bne  a2, a0, .L1

    initregs
    li   a0, LOOP_CNT
    li   x17, 0x73
    li a1, LOOP_CNT
    li a5, 0x800016f0
    li a4, 0x801016f8
    li a6, 0x40000
.L3:
    lw      a3,0(a5)
    sw      a3,0(a4)
    lw      a7,4(a5)
    sw      a7,4(a4)
    lw      t3,8(a5)
    sw      t3,8(a4)
    lw      t4,12(a5)
    sw      t4,12(a4)
    lw      t5,16(a5)
    lw      t6,20(a5)
    sw      t6,20(a4)
    sw      t5,16(a4)
    lw      a0,24(a5)
    sw      a0,24(a4)
    lw      t0,28(a5)
    sw      t0,28(a4)
    lw      t1,32(a5)
    lw      t2,36(a5)
    sw      t2,36(a4)
    sw      t1,32(a4)
    lw      a3,40(a5)
    sw      a3,40(a4)
    lw      a7,44(a5)
    sw      a7,44(a4)
    lw      t4,48(a5)
    lw      t5,52(a5)
    sw      t5,52(a4)
    sw      t4,48(a4)
    mv      t3,a5
    mv      t6,a4
    lw      a0,56(t3)
    sw      a0,56(t6)
    lw      t0,60(t3)
    add    a5,a5,a6
    add    a4,a4,a6
    sw      t0,60(t6)
    addi a2, a2, 1
    bne  a2, a1, .L3

end:
    j exit


