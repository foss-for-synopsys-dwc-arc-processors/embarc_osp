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
_start:
    initregs


    
    li sp, 0x200000
    li t2, 0x200f5c
    li   a3, LOOP_CNT
    li  s2, 0x500
    sw   s2,(sp)
    sw   s2,(t2)
    nop
    nop
    nop
    nop
    nop
.L1:
     lw      t6,(sp)
     amoswap.w      t4,t1,(t6) # lw t4, (t6), sw t1, (t6)
     mv      t3, t4
     nop
     nop
     lw      s1,(t2)
     sw t4, (s1)
     mv t1, s1
     mv      t3, t4
    addi a2, a2, 1
    bne  a2, a3, .L1


end:
    j exit


