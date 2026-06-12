/*
;; riscv64-elf-gcc -march=rv32i -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
*/

.include "utils.s"

.equ LOOP_CNT     ,16

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
    li s10, 0xcccccccd
    li a5, 0xccccd000
    li x2, LOOP_CNT
    li x14, 0x10015600
  mv  a2, zero
  csrr    a5,0x7c8     #_arcv_cache_enable
  lui     a4,0x11
  addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
  or      a5,a5,a4
  csrw    0x7c8,a5
.L1:
    fence.i
    li x3, 0
    sw           x11,(x14)
    sw           x11,4(x14)
    sw           x11,8(x14)
    sw           x11,12(x14)
    sw           x11,(x14)
    sw           x11,4(x14)
    sw           x11,8(x14)
    sw           x11,12(x14)
    addi         x14,x14,0x10
.L2:
    addi x3, x3, 1
    sw           x11,(x14)
    sw           x11,4(x14)
    sw           x11,8(x14)
    sw           x11,12(x14)
    bne  x3, x2, .L2

    addi x1, x1, 1
    bne  x1, x2, .L1
end:
    j exit