.include "utils.s"

.equ LOOP_CNT     ,32
.equ ICACHE_ADDR  ,0x36e0

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
    li   a0, LOOP_CNT
.L1:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    jal _test1   /*0x3762*/
    jal _test2   /*0x3764*/
    jal _test3   /*0x3766*/
    beq     a0,zero, _test3
    addi a1, a1, 1
    bne  a1, a0, .L1
end:
    c.j exit

.global _test1
.text
.org ICACHE_ADDR + 0x200
.type _test1, @function
_test1:
 c.nop
 nop
 addi a1, a1, 0
 ret

.global _test2
.text
.org ICACHE_ADDR + 0x250
.type _test1, @function
_test2:
 c.nop
 nop
 addi a2, a2, 0
 ret

.global _test3
.text
.org ICACHE_ADDR + 0x300
.type _test3, @function
_test3:
 c.nop
 nop
 addi t1, t1, 0
 ret