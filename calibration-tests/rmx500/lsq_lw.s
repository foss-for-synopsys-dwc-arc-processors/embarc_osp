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
.L1:
     lw      t6,(t1)
     addi    t2, t6, 1
     nop
     nop
     nop
     nop
     nop
     lw      s1,32(t1) 
     lw      t6,64(t1)
     addi    t2, t6, 1
    nop
    nop
    nop
    nop
    nop
     lw      s4,96(t1)
     lw      s5,130(t1)
     lw      t6,180(t1)
     addi    t2, t6, 1
    addi a2, a2, 1
    addi t1, t1, 0xe0
    bne  a2, a0, .L1
mv a2, zero
.L2:
     lw      s1,(t1) 
     lw      s2,32(t1)
     lw      s3,64(t1)
     lw      s4,96(t1)
     lw      t6,130(t1)
    addi    t2, t6, 1
    nop
    nop
    nop
    nop
    nop
    addi a2, a2, 1
    addi t1, t1, 0xe0
    bne  a2, a0, .L2
mv a2, zero
.L3:
     lw      s1,(t1) 
     lw      s2,32(t1)
     lw      s3,64(t1)
     lw      s4,96(t1)
     lw      s5,130(t1)
     lw      t6,180(t1)
    addi    t2, t6, 1
    nop
    nop
    nop
    nop
    nop
    addi a2, a2, 1
    addi t1, t1, 0xe0
    bne  a2, a0, .L3

end:
    j exit


