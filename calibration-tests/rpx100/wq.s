/*
;; riscv64-elf-gcc -march=rv64i -mabi=ilp64-static -nostdlib -nostartfiles -T link.ld test.s -o test.ou
riscv64-elf-gcc -march=rv64i_zicsr_zifencei_zihintpause_a_zca_zcd_d_f_h_m_zcb_zba_zbb_zbc_zbs_zicond_zfa_zfh_zicbom_zicbop -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld test.s -o test.out
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
    li a5, 0x000d290
    li a4, 0x000e2a0
.L1:
    c.ld    a3,0(a5)         #
    c.mv    t0,a5            # w1: a3 <- 00000000 w2: t0 <- 0000c610 ld [c610]
    c.sd    a3,0(a4)         #
    addi    a7,a4,0         #
    addi    a5,a5,0        # w0: a5 <- 0000c690 w2: a7 <- 0000d680 st 0 -> [d620] st 0 -> [d624]
    ld      a0,-120(a5)       #
    addi    a4,a4,0       # w0: a4 <- 0000d6a0 w1: a0 <- 00000000 ld [c618]

    # sd      a0,-120(a4)       #                     st 0 -> [d628] st d6a0 -> [d62c]
    # ld      a1,-112(a5)       # w1: a1 <- 00000000 ld [c620]

    # sd      a1,-112(a4)       #                     st 0 -> [d630] st 0 -> [d634]
    # ld      t3,-104(a5)       # w1: t3 <- 00000000 ld [c628]

    sd      t3,-104(a4)       #                     st 0 -> [d638] st 0 -> [d63c]
    ld      t6,-96(a5)        #
    ld      t5,-88(a5)        # w1: t6 <- 00000000 w2: t5 <- 00000000 ld [c630]
    sd      t6,-96(a4)        #
    sd      t5,-88(a4)        #                     st 0 -> [d640]
    ld      t1,-80(a5)        #                     ld [c640] # dcsm dcm dcstrf dcrf cpcinv
    sd      t1,-80(a4)        #                     st 0 -> [d650] st 0 -> [d654] # bissue bmemrw+7 bdmp+7 bdstrhaz+7 bdclbuf+4 bdcstall+7
    ld      t2,-72(a5)        # w1: t2 <- 00000000 ld [c648]
    sd      t2,-72(a4)
    ld      a3,-64(a5)
    ld      a0,-56(a5)
    sd      a3,-64(a4)
    sd      a0,-56(a4)
    ld      a1,-48(a5)
    sd      a1,-48(a4)
    ld      t3,-40(a5)
    sd      t3,-40(a4)
    ld      t6,-32(a5)
    ld      t5,-24(a5)
    sd      t6,-32(a4)
    sd      t5,-24(a4)
    ld      t1,112(t0)
    sd      t1,16(a7)
    ld      t0,120(t0)
    sd      t0,24(a7)
    addi  sp, sp, 1
    bne  sp,a2, .L1

end:
    j exit

