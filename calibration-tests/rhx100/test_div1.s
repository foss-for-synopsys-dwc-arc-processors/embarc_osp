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
.org   ICACHE_ADDR
_start:
    initregs
  mv  a2, zero
  csrr    a5,0x7c8     #_arcv_cache_enable
  lui     a4,0x11
  addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
  or      a5,a5,a4
  csrw    0x7c8,a5
    li a1, 0x5
    li a5, 0
    div a2, a1, a5 /* 5/0 0*/
    mv a3, a2
    li a1, 1
    li a5, 0x5
    div a2, a1, a5 /* 1/5 0*/
    mv a3, a2
    li a1, 0x80000000
    li a5, 0xffffffff
    div a2, a1, a5  /* 0x80000000/0xffffffff 5 */
    mv a3, a2

    li a1, 0x80000000
    li a5, 1
    div a2, a1, a5  /* 0x80000000/1 22 */
    mv a3, a2

    li a1, 0x80000000
    li a5, 2
    div a2, a1, a5  /* 0x80000000/2 21*/
    mv a3, a2

    li a1, 0x80000000
    li a5, 0xfffffffe /* 0x80000000/0xfffffffe 22*/
    div a2, a1, a5
    mv a3, a2


    li a1, 0
    li a5, 0xffffffff
    div a2, a1, a5 /* 0/0xffffffff 5*/ 
    mv a3, a2

    li a1, 0xffffffff
    li a5, 0x8
    div a2, a1, a5 /* 0xffffffff/8 5*/ 
    mv a3, a2

    li a1, 0x8fffffff
    li a5, 0x8
    div a2, a1, a5 /* 0x8fffffff/8 5*/ 
    mv a3, a2

    li a1, 0xf
    li a5, 0x80000000
    div a2, a1, a5 /* 0xf/0x80000000 5*/ 
    mv a3, a2

    li a1, 0x80000000
    li a5, 0x1
    div a2, a1, a5 /* 80000000/1 22*/ 
    mv a3, a2

li a4, 0x1
li a1, 0xffff0100
li s1, 0xffffffff
li s2, 1

.L1:
    nop
    nop
    nop
    nop
    nop
    div a2, a4, a1  /* 1/0xffff0100 ~ 0xffffffff 5, 10, 11~13*/ 
    mv a3, a2
    add a1, a1, s2
    bne  s1, a1, .L1

li a4, 0x1
li a1, 0x0
li s1, 0x1000
li s2, 0x20

.L2:
    nop
    nop
    nop
    div a2, a4, a1  /* 1/ 0~0x1000 5, 10, 11~13*/ 
    mv a3, a2
    add a1, a1, s2
    bne  s1, a1, .L2

li a4, 0xffffffff
li a1, 0xffff0100
li s1, 0xffffffff
li s2, 0x1

.L3:
    nop
    div a2, a4, a1  /*  0xffffffff/ 0~0x1000  5*/ 
    mv a3, a2
    add a1, a1, s2
    bne  s1, a1, .L3

li a4, 0xffffffff
li a1, 0x0
li s1, 0x1000
li s2, 0x20

.L4:
    nop
    nop
    div a2, a1, a4  /* 0~0x1000 / 0xffffffff 5, 10, 11~13*/ 
    mv a3, a2
    add a1, a1, s2
    bne  s1, a1, .L4

li a4, 0xffffffff
li a1, 0xffff0100
li s1, 0xffffffff
li s2, 0x1

.L6:
    nop
    nop
    nop
    nop
    div a2, a4, a1  /* 0xffffffff / 0xffff0100 ~ 0xffffffff 5 */ 
    mv a3, a2
    add a1, a1, s2
    bne  s1, a1, .L6
end:
    j exit
