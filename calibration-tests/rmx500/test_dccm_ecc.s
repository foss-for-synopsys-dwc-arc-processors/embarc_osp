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
    li gp, 0x80000af0
    li t3, 0x800012b0
.L1:
    nop
    nop
     sb      s6,1731(gp) 
     sb      s3,1773(gp)
     sb      s8,1744(gp)
     sb      s11,1778(gp)
     sb      s2,1770(gp)
     sb      s0,1737(gp)
     sb      s0,5(t3)
     sb      s0,1738(gp)
     sw      zero,1884(gp)
     sb      s5,1739(gp)
     sb      s0,1728(gp)
     sb      s0,1730(gp)
     sb      s4,1740(gp)
     sb      s0,1745(gp)

    addi a2, a2, 1
    bne  a2, a0, .L1

end:
    j exit


