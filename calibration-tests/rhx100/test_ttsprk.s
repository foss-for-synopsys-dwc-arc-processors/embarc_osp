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

    li t1, 0x10020000
    li a3, 0
    li a1, 0x20
    li a5, 0x1004f7e0
    li t3, 0x20
    nop
    nop
    nop
    nop
    nop
    li      a4, 0x10028000
    li      t4, 0x10028000
    li      t2, 0x10028000
    li      a2, 0x10028000

.L3:
    li      a1, 0x1002a680
    li      t1, 0x1002a9a8
    li      t6, 0x1002acd0
    lw      a5,-516(a4)           /*         ld [10027dfc] */
    sh2add  a0,a5,a1
    sh2add  a7,a5,t1
    lw      a6,0(a0)               /*        ld [1002a680] */
    sw      a6,-532(a2)            /*        st 0 -> [10027dec]  */
    lui     a0,65576
    lw      a1,0(a7)               /*        ld [1002a9a8] */
    lui     t1,65576   
    sw      a1,-536(t2)              /*      st 0 -> [10027de8] */
    li      t5, 0x10028000
    lw      a6,0(t6)                 /*      ld [1002acd0] */
    lw      t2,-896(a0)              /*         ld [10027c80]  */
    sw      a6,-540(t4)            /*    st 0 -> [10027de4] */
    sh2add  a7,a5,t2  
    lw      t4,0(a7)                /*       ld [1002aff8] */
    lw      t6,-904(t5)              /*       ld [10027c78] */
    sw      t4,-544(t1)           /*     st 14 -> [10027de0] */

    addi      a4, a4,0x40
    mv        t4, a4
    mv        t2, a4
    mv        a2, a4
    addi a3, a3, 1
    bne  a3, t3, .L3

end:
    j exit