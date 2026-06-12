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
    li a7, 0x8000de80
    li a1, 0x1000c4e0
    li t3, 0x1000c4e0
    li gp, 0x1000b920


.L1:
    sw      a2,1136(gp)
    sw      a3,1120(gp)
    sw      a4,1112(gp)
    sb      a6,1107(gp)
    sb      a7,1105(gp)
    sb      t3,1103(gp)
    sb      t4,1101(gp)
    sb      t5,1099(gp)
    sb      t5,1097(gp)
    lbu     t4,1085(gp)
    mv   t3, t4
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

    addi sp, sp, 1
    bne sp ,a2, .L1

end:
    j exit

