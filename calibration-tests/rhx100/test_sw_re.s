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
    sw      a2,(t1)
    lw      a2, (t1)
    li a3, 0
    li a0, 0x120
    nop
    nop
    nop
    nop
    nop

mv a2, zero
.L3:
    li a3, 0
     lw      a5, 8(t1)

     sw      a2,(t1) 
     addi    t2, t6, 1
.L1:
     mul     s1,a5,a5
     add     s2,s1,a2
    addi a3, a3, 1
    bne  a3, a0, .L1

    nop
    nop
    nop
    addi a2, a2, 1
    bne  a2, a0, .L3

end:
    j exit


