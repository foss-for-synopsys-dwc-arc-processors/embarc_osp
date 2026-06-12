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
    li   a0, LOOP_CNT
    li sp, 0x200f5c
    li gp, 0x10200af0
    li t3, 0x102012b0
    li a1, 16

  mv  a2, zero
  csrr    a5,0x7c8     #_arcv_cache_enable
  lui     a4,0x11
  addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
  or      a5,a5,a4
  csrw    0x7c8,a5

    li t1, 0x11000000
    li a3, 0
    li a0, 0x20
    nop
    nop
    nop
    nop
    nop

mv a2, zero
li t0, 0x200000
.L3:
  mul     s10,a6,a3
  add   s10,s10, a7 

  mul     t4,t5,a5        
  add     t2,s10,t4  
  add     t5,t2,t4            
  srai    t1,t5, 5
  div     t3, a2, a3
  sub     t2,t3,t4 
    addi a2, a2, 1

    bne  a2, a0, .L3

end:
    j exit


