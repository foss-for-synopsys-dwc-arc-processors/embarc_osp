/*
;; riscv64-elf-gcc -march=rv32i_zicsr -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld test.s -o test.out
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
_start:
    initregs
    li   a0, LOOP_CNT
    li sp, 0x200f5c
    li gp, 0x10200af0
    li t3, 0x102012b0
    li a1, 16



    li t1, 0x280000
    li a3, 0
    li a0, 0x20
    nop
    nop
    nop
    nop
    nop
.L3:
    nop
    nop
    sw      t1,(t1)
    sw      t1,64(t1)
    sw      t1,128(t1)
    sw      t1,192(t1)
    sw      t1,300(t1)
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    sw      t1,(t1)
    lw      s0,(t1)
    addi    t2, s0, 1
    nop
    nop
    lw      s0,32(t1)
    lw      s3,64(t1)
    addi    t2, s3, 1
    nop
    nop
    lw      s0,96(t1)
    lw      s3,128(t1)
    lw      s1,160(t1)
    addi    t2, s1, 1
    nop
    nop
    lw      s0,196(t1)
    lw      s3,228(t1)
    lw      s1,260(t1)
    lw      s4,292(t1)
    addi    t2, s4, 1
    addi a2, a2, 1
    addi t1, t1, 330
    bne  a2, a0, .L3

end:
    j exit


