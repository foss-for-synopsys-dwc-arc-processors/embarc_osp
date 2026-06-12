/*
;; riscv64-elf-gcc -march=rv64i -mabi=ilp64-static -nostdlib -nostartfiles -T link.ld test.s -o test.ou
riscv64-elf-gcc -march=rv32i -mabi=ilp32 -static -nostdlib -nostartfiles -T link_ccm.ld bne.s -o bne.out
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
    initregs
    li  a2, LOOP_CNT

.L1:

    addi  s1, s1, 1
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    bne  s1,a2, .L1

end:
    j exit

