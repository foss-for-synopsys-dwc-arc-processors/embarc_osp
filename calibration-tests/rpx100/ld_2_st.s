/*
;; riscv64-elf-gcc -march=rv64i -mabi=ilp64-static -nostdlib -nostartfiles -T link.ld test.s -o test.ou
riscv64-elf-gcc -march=rv64i_zicsr_zifencei_zihintpause_a_zca_zcd_d_f_h_m_zcb_zba_zbb_zbc_zbs_zicond_zfa_zfh_zicbom_zicbop -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld test.s -o test.out
*/

.include "utils.s"

.equ LOOP_CNT     ,32
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
    li a4, 0x8000de80
    li a1, 0x8000c4e0
    li t3, 0x8000c4e0
    li a7, 0x8000d580
    sd      t3,-80(a4)

.L1:
    sd      t1,-80(a4)
    sd      a3,-64(a4)
    sd      a3,-64(a4)
    sd      a0,-64(a4)
    ld      a5,-64(a1)
    sd      a1,-64(a4)
    ld      a5,-64(t3)
    sd      t3,-64(a4)
    ld      a5,-32(t6)
    ld      a5,-32(t5)
    sd      t6,-32(a4)
    sd      t5,-32(a4)
    ld      t0,112(t1)
    sd      t1,16(a7)
    ld      t0,(t0)
    mv  t1, a5
    mv     a5, t0
    li t6, 0x8000c4e0
    li t5, 0x8000c4e0
    li t1, 0x8000c4e0
 
    addi sp, sp, 1
    bne sp ,a2, .L1

end:
    j exit

