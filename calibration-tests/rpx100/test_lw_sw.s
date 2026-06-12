/*
;; riscv64-elf-gcc -march=rv32i_zicsr -mabi=ilp32 -static -nostdlib -nostartfiles -T link_ccm.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
*/

.include "utils.s"

.equ LOOP_CNT     ,20
.equ ICACHE_ADDR  ,0x00f00

.section ".text.init"
.globl   _vectors
.type    _vectors, @function
_vectors:
Reset:
   jal    _start
   ebreak


.globl _start
.text
.org   ICACHE_ADDR
_start:
    initregs
  csrr    a5,0x7c8     #_arcv_cache_enable
  lui     a4,0x11
  addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
  or      a5,a5,a4
  csrw    0x7c8,a5

    li   t4, 1
    li sp, 0x0027ff34
    li gp, 0x200a54
    li a5, 0x204be0
    li a1, 0x204bd8
    li a0, 0x286870
    li a2, 0x286860
    sw a0,0(a1)
    sw a2,0(a0)
    li a0, 0x204bd8
    li a2, 0
.L1:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    lw      a0,0(a0)
    addi    t0,a4,-1
    sw      t0,8(a5)
    lw      t1,0(a0)
    sw      t1,0(a5)
    li a0, 0x204bd8     
    addi a2, a2, 1
    bne  a2, t4, .L1

    li   t4, LOOP_CNT
    li sp, 0x27ffd0
    li gp, 0x200a54
    li a5, 0x204be0
    li a1, 0x204bd8
    li a0, 0x286870
    li a2, 0x205d10
    sw a0,0(a1)
    sw a2,4(a0)
    li a0, 0x204bd8
    li a2, 0
    li s1, 0x205000
    li a3, 0x205d10
.L2:
    lw      a3,4(a0)
    addi    sp, sp, -16
    sw      s1,4(sp)
    sw      s0,8(sp)
    lui     s1,517
    sw      ra,12(sp)
    sw      a3,-1180(s1)
    lbu     a5,2(a3)
    lbu     t0,3(a3)
    
    addi a2, a2, 1
    bne  a2, t4, .L2
end:
    j exit
