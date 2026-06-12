

 /*
;; riscv64-elf-gcc -march=rv32i -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
*/

.include "utils.s"

.equ LOOP_CNT     ,32
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
    li   a0, LOOP_CNT
    li   x17, 0x73
    li a2, LOOP_CNT
.L1:
    nop
    li a4, 0x70000000
    li a0, 0x9000
    nop
    nop
    nop
    div    a7,a4,a0
    rem    a4,a4,a0 
    mul     a5,a7,a1 
    mv  a6,a5

    nop
    nop
    nop
    nop
    nop
    nop
    divu    a7,a4,a0
    remu    a4,a4,a0 
    mul     a5,a7,a1   
    mv  a6,a5 
    nop
    nop
    mul     a5,a7,a1
    remu    a7,a4,a0
    mv  a6,a7

    remu    a7,a4,a0
    mv     a7, a3
    mul     a5,a7,a1
    mv     a5, a3
    mul     a5,a7,a1
    add     a5, a5, a2
    addi a3, a3, 1
    bne  a2, a3, .L1
end:
    j exit