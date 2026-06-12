/*
;; riscv64-elf-gcc  -march=rv32ic_zicsr_zifencei_zihintpause_a_zca_m_zcb_zcmp_zcmt_zba_zbb_zbc_zbs_zicond_zicbom_zicbop_zihintntl -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
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

  mv  a2, zero
  csrr    a5,0x7c8     #_arcv_cache_enable
  lui     a4,0x11
  addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
  or      a5,a5,a4
  csrw    0x7c8,a5
  initregs
  li a5, 0x1002cc8c
  li   t2, LOOP_CNT

.L1:
    li a5, 0x1004f7b8
    c.sb    a4,0(a5)
    c.addi  a5,-1
    lbu     t1,-1(a5)
    sb      t1,0(a5)
    c.addi  a5,-1
    lbu     a2,-1(a5)
    c.sb    a2,0(a5)
    c.addi  a5,-1 
    lbu     t5,-1(a5)  
    sb      t5,0(a5)
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    lw            a2,0(a5)
    add           a1, a5, a2
    sb            a1,0(a5)
    lw            a2,0(a5)
    mv            x1,a2
    add           a1, a5, a5
    sw            a1,0(a5)
    lw            a2,0(a5)
    mv            x1,a2
    mul           a1, a5, a5
    sb            a1,0(a5)
    lw            a2,0(a5)
    mv            x1,a2
    mul           a1, a5, a5
    csrrs         a4,mcycle,zero
    sb            a1,0(a5)
    lw            a2,0(a5)
    mv            x1,a2
    mul           a1, a5, a5
    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    sb            a1,0(a5)
    lw            a2,0(a5)
    mv            x1,a2
    mul           a1, a5, a5
    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    csrrs         a4,mcycle,zero
    sb            a1,0(a5)
    lw            a2,0(a5)
    mv            x1,a2
    addi a3, a3, 1
    bne  a3, t2, .L1

end:
    j exit
