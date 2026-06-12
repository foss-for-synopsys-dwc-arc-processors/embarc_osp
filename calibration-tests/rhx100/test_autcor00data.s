/*
;; riscv64-elf-gcc -march=rv32i_zicsr -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld test.s -o test.out
*/

.include "utils.s"

.equ LOOP_CNT     ,16
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

  csrr    a5,0x7c8     #_arcv_cache_enable
  lui     a4,0x11
  addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
  or      a5,a5,a4
  csrw    0x7c8,a5
    mv  a2, zero
    li   a0, LOOP_CNT
.L1:
    li a5, 0x10014002
    li a6, 4
    mul     t1,t6,t0
    sra     t5,t1,a4
    add   a7,a7,t5   
    lh      t2,0(a5)
    add     s0,a5,a6
    lh      t0,0(s0)
    addi  a5,a5,2 
    addi a2, a2, 1    
    bne  a2, a0, .L1

    mv  a2, zero
    li   a0, LOOP_CNT
.L2:
    li a5, 0x214002
    li a6, 4
    mul     t1,t6,t0
    sra     t5,t1,a4
    add   a7,a7,t5   
    lh      t2,0(a5)
    add     s0,a5,a6
    lh      t0,0(s0)
    addi  a5,a5,2 
    addi a2, a2, 1    
    bne  a2, a0, .L2
end:
    j exit
