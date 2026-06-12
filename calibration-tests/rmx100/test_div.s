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
    li a1, 0x5
    li a5, 0
    div a2, a1, a5 /* 5/0 */
    mv a3, a2
    li a1, 1
    li a5, 0x5
    div a2, a1, a5 /* 1/5 */
    mv a3, a2
    li a1, 0x80000000
    li a5, 0xffffffff
    div a2, a1, a5  /* 0x80000000/0xffffffff */
    mv a3, a2

    li a1, 0x80000000
    li a5, 1
    div a2, a1, a5  /* 0x80000000/0xffffffff */
    mv a3, a2

    li a1, 0x80000000
    li a5, 2
    div a2, a1, a5  /* 0x80000000/2 */
    mv a3, a2

    li a1, 0x80000000
    li a5, 0xfffffffe /* 0x80000000/0xfffffffe */
    div a2, a1, a5
    mv a3, a2


    li a1, 0
    li a5, 0xffffffff
    div a2, a1, a5 /* 0/0xffffffff */ 
    mv a3, a2

    li a1, 0xffffffff
    li a5, 0x8
    div a2, a1, a5 /* 0xffffffff/8 */ 
    mv a3, a2

    li a1, 0xffffffff
    li a5, 0x8
    div a2, a1, a5 /* 0x8fffffff/8 */ 
    mv a3, a2

    li a1, 0xf
    li a5, 0x80000000
    div a2, a1, a5 /* 0xf/0x80000000 */ 
    mv a3, a2

    li a1, 0x80000000
    li a5, 0x1
    div a2, a1, a5 /* 0xf/0x80000000 */ 
    mv a3, a2

li a4, 0x8fffffff
li a1, 0x1
li s1, 0x80000000
li s2, 0x200
li t1, 2
.L1:
    nop
    div a2, a4, a1  /* 0x8fffffff/1~0x80000000 */ 
    mv a3, a2
    mul a1, a1, t1
    sub a4, a4, s2
    bne  s1, a1, .L1

li a4, 0x4fffffff
li a1, 0x1
li s1, 0x80000000
li s2, 0x200
li t1, 2
.L2:
    nop
    div a2, a4, a1  /* 0x4fffffff/1~0x80000000 */ 
    mv a3, a2
    mul a1, a1, t1
    sub a4, a4, s2
    bne  s1, a1, .L2

li a4, 0xffff0100
li a1, 0x0
li s1, 0x1000
li s2, 0x20

.L3:
    nop
    div a2, a4, a1  /* 0xffff0100/1~0x1000  ncam719 ~ 677 */ 
    mv a3, a2
    add a1, a1, s2
    bne  s1, a1, .L3

li a4, 0xffff0100
li a1, 0x0
li s1, 0x1000
li s2, 0x20

.L4:
    nop
    div a2, a1, a4  /* 1~0x1000 / 0xffff0100 */ 
    mv a3, a2
    add a1, a1, s2
    bne  s1, a1, .L4

li a4, 0x80000000
li a1, 0x0
li s1, 0x1000
li s2, 0x1

.L5:
    nop
    div a2, a4, a1  /* 0x80000000 / 1~0x1000*/ 
    mv a3, a2
    add a1, a1, s2
    bne  s1, a1, .L5

li a4, 0x80000000
li a1, 0xffff0100
li s1, 0xffffffff
li s2, 0x1

.L6:
    nop
    div a2, a4, a1  /* 0x80000000 / 0xffff0100 ~ 0xffffffff*/ 
    mv a3, a2
    add a1, a1, s2
    bne  s1, a1, .L6
end:
    j exit
