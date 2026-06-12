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

  mv  a2, zero
  csrr    a5,0x7c8     #_arcv_cache_enable
  lui     a4,0x11
  addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
  or      a5,a5,a4
  csrw    0x7c8,a5

    li t1, 0x10020000
    li t2, 0x2012b0
    li t3, 0x2812b0
    li a3, 0
    li a0, 0x20
    sw t3,(t2)
    nop
    nop
    nop
    nop
    nop
.L3:
    lw      s0,(t1)
    sw      s0,(t2)
    nop
    nop
    lw      s0,(t3)
    sw      s0,32(t2)
    nop
    nop
    lw      s0,(t2)
    sw      s1,(s0)
    addi a2, a2, 1
    addi t1, t1, 128
    addi t3, t3, 128
    bne  a2, a0, .L3

end:
    j exit


