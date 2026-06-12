
/*
;; riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
*/

.include "utils.s"

.equ LOOP_CNT     ,5
.equ ICACHE_ADDR  ,0x0001000

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
    li gp, 0x200a54
    li a2, 16
    li sp, 0x10200a54
.L1:
     sw      zero,1988(gp)
    sw      zero,1992(gp)
    sb      zero,1851(gp)
    sw      zero,1984(gp)

    sb      zero,1849(gp)
    sb      zero,1850(gp)
    sb      zero,1843(gp)
    sb      zero,1844(gp)
    sb      zero,1841(gp)
    sb      zero,1842(gp)
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
     sw      zero,1988(sp)
    sw      zero,1992(sp)
    sb      zero,1851(sp)
    sw      zero,1984(sp)

    sb      zero,1849(sp)
    sb      zero,1850(sp)
    sb      zero,1843(sp)
    sb      zero,1844(sp)
    sb      zero,1841(sp)
    sb      zero,1842(sp)
    lw      a1, 1842(sp)
    add    a3, a1,a3
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    addi a3, a3, 1
    bne  a3, a2, .L1
end:
    j exit

