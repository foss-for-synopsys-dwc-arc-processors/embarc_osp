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

    li t1, 0x80100024
    li t2, 0x8020006c
    li t3, 0x800800a8
    li t4, 0x800800e4
    li a3, 0
    li a0, 0x20
    nop
    nop
    nop
    nop
    nop
.L1:
    nop
    lw      a3,0(t1)
    sw      a3, 0(t2)
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    lw      a3,0(t1)
    sw      a3, 0(t2)
    mv      a4, a3

    lw      a3,0(t2)
    sw      a3, 0(t2)
    mv      a4, a3
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    lw      a3,0(t3)
    sw      a3, 0(t4)
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    lw      a3,0(t3)
    mv      a4, a3
    addi a2, a2, 1
    addi t1, t1, 0xe0
    bne  a2, a0, .L1

end:
    j exit


