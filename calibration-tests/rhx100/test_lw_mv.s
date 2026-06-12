/*
;; riscv64-elf-gcc -march=rv32ic_zicsr_zifencei_zihintpause_a_zca_m_zcb_zcmp_zcmt_zba_zbb_zbc_zbs_zicond_zicbom_zicbop_zihintntl -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
*/

.include "utils.s"

.equ LOOP_CNT     ,32
.equ ICACHE_ADDR  ,0x001110

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
  mv  a2, zero
  csrr    a5,0x7c8     #_arcv_cache_enable
  lui     a4,0x11
  addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
  or      a5,a5,a4
  csrw    0x7c8,a5
    initregs
    li   a0, LOOP_CNT
    li   x17, 0x73
    li a2, LOOP_CNT
    li t2, 0x1000f008
    li a5, 0x10008a20
    sw t2, 20(t2)
.L1:
    nop
    nop
    nop
    nop
    c.mv    t0,t2
    lw      t2,20(t2)
    nop
    nop
    nop
    nop
    nop
    nop

    c.mv    a4, a1
    lw      t0,8(a5)

    addi a3, a3, 1
    bne  a2, a3, .L1
end:
    j exit
