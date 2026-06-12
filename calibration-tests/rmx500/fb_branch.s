

 /*
;; riscv64-elf-gcc -march=rv32i -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
*/

.include "utils.s"

.equ LOOP_CNT     ,32
.equ ICACHE_ADDR  ,0x0040

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
  mv  a2, zero
  csrr    a5,0x7c8     #_arcv_cache_enable
  lui     a4,0x11
  addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
  or      a5,a5,a4
  csrw    0x7c8,a5
    li s0, LOOP_CNT
    
    li a4, 0
.L1:
    
    nop
    nop
    li a2, 1
    nop
    addi a3, a3, 1
    bltu  a3, s0, .L1
    bnez a2, .L2
    jal x1, _test2
.L2:
    li a3, 0
    addi a4, a4, 1
    nop
    nop
    nop
    nop
    nop
    bne  s0, a4, .L1
end:
    j exit

.global _test1
.org 0x4000
.section .icache, "ax"
.type _test2, @function
_test2:
 nop
 nop
 nop
 nop
 nop
 nop
 jr x1 /* rd != link, rs1 != link */