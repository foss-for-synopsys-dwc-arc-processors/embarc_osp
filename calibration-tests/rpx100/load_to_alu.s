/*
;; riscv64-elf-gcc -march=rv64 -mabi=lp64 -static -nostdlib -nostartfiles -T link.ld test.s -o test.out
*/

.include "utils.s"

.equ LOOP_CNT     ,16
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
    li t3, 0x2000003e
    li a3, 0
    li a0, 0x20
    nop
    nop
    nop
    nop
    nop
.L3:
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
    lw      s0,(t1)

    addi    t2, s0, 1
    csrr    a5,0x7c8
    csrr    a5,0x7c8
    csrr    a5,0x7c8
    csrr    a5,0x7c8
    csrr    a5,0x7c8
    csrr    a5,0x7c8
    csrr    a5,0x7c8
    csrr    a5,0x7c8
    lw      s0,(t3)
    lw      s1,128(t3)
    addi    t2, s1, 1
    nop
    nop
    lw      s0,64(t1)
    lw      s3,128(t1)
    nop
    nop
    nop
    addi    t2, s0, 1
    csrr    a5,0x7c8
    csrr    a5,0x7c8
    csrr    a5,0x7c8
    csrr    a5,0x7c8
    csrr    a5,0x7c8
    csrr    a5,0x7c8
    csrr    a5,0x7c8
    csrr    a5,0x7c8
    nop
    nop
    lw      s0,192(t1)
    lw      s3,256(t1)
    lw      s1,320(t1)
    addi    t2, s1, 1
    nop
    nop
    lw      s0,384(t1)
    lw      s3,448(t1)
    lw      s1,512(t1)
    lw      s4,576(t1)
    addi    t2, s4, 1
    addi t3, t3, 0x100
    addi a2, a2, 1
    addi t1, t1, 640
    bne  a2, a0, .L3

end:
    j exit


