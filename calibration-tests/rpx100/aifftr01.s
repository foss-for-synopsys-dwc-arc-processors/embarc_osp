/*
;; riscv64-elf-gcc -march=rv64i -mabi=ilp64-static -nostdlib -nostartfiles -T link.ld test.s -o test.ou
riscv64-elf-gcc -march=rv32i_zicsr -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld test.s -o test.out
*/

.include "utils.s"

.equ LOOP_CNT     ,16
.equ ICACHE_ADDR  ,0x00010000

.section ".text.init"
.globl   _vectors
.type    _vectors, @function
_vectors:
Reset:
   jal    _start
   ebreak


.globl _start
.text
/*.org   ICACHE_ADDR*/
_start:
    li sp,0x10008000
    li  a1, LOOP_CNT
    li t1, 0
.L1:
    sd      a7,(sp)
    mul     a0,s1,a3
    mul     a4,s2,s2
    add     t2, a4, a4
    /*ld      a2,8(sp)
    mul     t2,s2,a3
    mul     a4,s2,s2*/
    addi t1,t1,1
    bne  t1,a1, .L1

end:
    j exit

