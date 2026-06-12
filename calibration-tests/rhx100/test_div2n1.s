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


li s2, 2
li t1, 1
li t2, 1
li t3, 30
li a1, 2
li a2, 0
li a3, 0
li a4, 0x1

.L1:

    nop
    div a3, a5, a2  /* 2^n + 1 / 2^(n+1) + 1 n=0~4, latency = 1, n > 4, stall = 5  */
    mv t1, a3
    mul a1, a1, s2
    mul a4, a4, s2
    addi a5, a4, 1
    addi a2, a1, 1
    add t2, t2, 1
    bne  t2, t3, .L1

li s2, 2
li t1, 1
li t2, 1
li t3, 30
li a1, 2
li a2, 0
li a3, 0
li a4, 0x1
li a5, 0

.L4:

    nop
    rem a3, a5, a2  /* 2^n + 1 / 2^(n+1) + 1 n=0~4, latency = 1, n > 4, stall = 5  */
    mv t1, a3
    mul a1, a1, s2
    mul a4, a4, s2
    addi a5, a4, 1
    addi a2, a1, 1
    add t2, t2, 1
    bne  t2, t3, .L4

li a4, 0x1
li a1, 0x0
li s1, 0x1000
li s2, 0x20

li s2, 2
li t1, 1
li t2, 1
li t3, 30
li a1, 2
li a2, 0
li a3, 0
li a4, 0x1

.L2:

    nop
    nop
    div a3, a2, a5  /* 2^(n+1) + 1 / 2^n + 1 n=0~3, latency = 1, n > 3, stall = 6 */
    mv t1, a3
    mul a1, a1, s2
    mul a4, a4, s2
    addi a5, a4, 1
    addi a2, a1, 1
    add t2, t2, 1
    bne  t2, t3, .L2

# li s2, 1
# li a2, 2
# .L3:

#     nop
#     nop
#     div a3, a2, a1  /* 2^(n+1) + 1 / 2^n + 1 n=0~3, latency = 1, n > 3, stall = 6 */
#     mv t1, a3
#     addi a1, a1, 1
#     addi a2, a2, 1
#     add t2, t2, 1
#     bne  t2, t3, .L3
end:
    j exit
