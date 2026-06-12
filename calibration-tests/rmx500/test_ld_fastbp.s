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

    li t1, 0x80000000
    li a3, 0
    li a0, 0x20
    nop
    nop
    nop
    nop
    nop


  mv  a2, zero
  csrr    a5,0x7c8     #_arcv_cache_enable
  lui     a4,0x11
  addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
  or      a5,a5,a4
  csrw    0x7c8,a5

    li t1, 0x8000f090
    li a3, 0
    li a0, 0x20
    li s0, 0x8000f090
    sw      t1,20(s0)
    sw      t1,8(s0)
    nop
    nop
    nop
    nop
    nop
.L3:
    lw t3, 8(s0)
    nop
    nop
    nop
    addi a2, a2, 1
    lw      s0,20(s0)
    bne  a2, a0, .L3

end:
    j exit


