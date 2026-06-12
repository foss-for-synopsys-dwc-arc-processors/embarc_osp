/*
;; riscv64-elf-gcc -march=rv64i -mabi=ilp64-static -nostdlib -nostartfiles -T link.ld test.s -o test.ou
riscv64-elf-gcc -march=rv64i_zicsr_zifencei_zihintpause_a_zca_zcd_d_f_h_m_zcb_zba_zbb_zbc_zbs_zicond_zfa_zfh_zicbom_zicbop -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld test.s -o test.out
*/

.include "utils.s"

.equ LOOP_CNT     ,16
.equ ICACHE_ADDR  ,0x00000250

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
  csrr    a5,0x7c8     #_arcv_cache_enable
  lui     a4,0x11
  addi    a4,a4,257   # 11101 <STACK_SIZE+0x10d01>
  or      a5,a5,a4
  csrw    0x7c8,a5
    li sp,0
    li  a2, LOOP_CNT


.L1:
li a5, 0x12214
li a6, 0x4
lh      t5,10(a5)
addw    a7,t1,a7
mulw    t0,t6,t5
lh      t1,14(a5)
lh      t6,12(a5)
add     t5,a6,a5
mulw    t6,s0,t6
lh      t5,14(t5)
mv      a1, t5 
    addi  sp, sp, 1
    bne  sp,a2, .L1

end:
    j exit

