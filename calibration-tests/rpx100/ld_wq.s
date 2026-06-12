/*
riscv64-elf-gcc -march=rv64i_zicsr_zifencei_zihintpause_a_zca_zcd_d_f_h_m_zcb_zba_zbb_zbc_zbs_zicond_zfa_zfh_zicbom_zicbop -mabi=ilp64 -static -nostdlib -nostartfiles -T link.ld test.s -o test.out
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
  csrr    a5,0x7c8     #_arcv_cache_enable
  lui     a4,0x11
  addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
  or      a5,a5,a4
  csrw    0x7c8,a5
    li sp,0
    li  a2, LOOP_CNT
    li t1, 0
    li sp, 0
    li gp, 0xb920
    li a5, 0xcda6
.L1:
    sd      zero,-678(a5)
    sd      zero,1216(gp)
    sd      zero,1208(gp)
    sd      zero,1200(gp)
    sb      zero,1039(gp) 
    sb      zero,1038(gp)
    sb      zero,1037(gp)
    sb      zero,1032(gp)
    sb      zero,1031(gp)
    sb      zero,1030(gp)
    sb      zero,1029(gp)
    sb      zero,1028(gp) 
    lbu     t4,1085(gp)
    mv t5, t4
    addi  sp, sp, 1
    bne  sp,a2, .L1

end:
    j exit

