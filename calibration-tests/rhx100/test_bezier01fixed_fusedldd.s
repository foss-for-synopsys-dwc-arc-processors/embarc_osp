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
_start:
    initregs
    li   a0, LOOP_CNT
    li sp, 0x200f5c
    li gp, 0x10200af0
    li t3, 0x102012b0
    li a3, 32

    mv  a2, zero
    csrr    a5,0x7c8     #_arcv_cache_enable
    lui     a4,0x11
    addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
    or      a5,a5,a4
    csrw    0x7c8,a5

    li s0, 0x10012164
    li sp, 0x100ffed0
    nop
    nop
    nop
    nop
    nop
.L3:
    lw      s11,8(s0)        
    lw      s9,16(s0)
    c.lw    a1,24(s0)       
    lw      t2,28(s0)        
           
    lw      s10,12(s0)      
    lw      s8,20(s0)        
    sw  t1,0(sp)            

    sw  s11,12(sp)            
    sw  a0,4(sp)             
    sw  a1,8 (sp)

    nop
    nop
    nop
    nop
    nop
    addi s0, s0, 0x40
    addi a2, a2, 1
    bne  a2, a3, .L3
end:
    j exit


