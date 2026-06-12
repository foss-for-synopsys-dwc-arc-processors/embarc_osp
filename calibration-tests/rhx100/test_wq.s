/*
;; riscv64-elf-gcc -march=rv32i -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
*/

.include "utils.s"

.equ LOOP_CNT     ,4
.equ ICACHE_ADDR  ,0x005f00

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

    li   a0, LOOP_CNT
    li sp, 0x0027ff34
    li gp, 0x200a54
    li t4, 0x201ac4
.L1:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    sb      s11,1884(gp)
    sb      s3,1879(gp)
    sb      s8,1852(gp)
    sb      s0,1845(gp)
    lw      s8,20(sp)
    sb      s0,5(t4)
    sb      s2,1877(gp)
    sw      zero,1968(gp)
    sb      s0,1846(gp)
    sb      s0,1836(gp)
    sb      s5,1847(gp)
    addi a2, a2, 1
    bne  a2, a0, .L1


    li gp, 0x200af0
    li a2, 0
.L2:
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

    sb      s6,1731(gp)
    sb      s3,1760(gp)
    sb      s8,1744(gp)
    sb      s11,1778(gp)
    sb      s2,1770(gp)
    sb      s0,1737(gp)
    sb      s0,5(t3)
    addi a2, a2, 1
    bne  a2, a0, .L2

    li gp, 0x200af0
    li a2, 0
    li s4, 0x286860
    li a0, 0x204bd8
    li a2, 0x286870
    sw a2, 4(a0)
    li a2, 0
    li a1, 0x286860
    li   s2, LOOP_CNT
.L3:
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

    sw      s0,4(s4)
    addi  s1, s1, 1
    nop
    lw    a5,8(a0)
    nop
    lw    a4,4(a0)
    addi    t0,a5,1
    c.sw    a1,0(a4)
    sw      zero,0(a1)
    c.sw    a1,4(a0)
    sw      t0,8(a0)

    addi a2, a2, 1
    bne  a2, s2, .L3
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
    lw a5, 0(a1)
    mv t0,a5

    li s0,  0x205000
    li a0,0x286860
    li a2,0x286850
    li a4, 0x174
    sw    a4,8(a0)
    sw    a2,0(a0)
    lw    a2,0(a0)
    mv    a0, a2
    li a0,0x204bd8
    li a2,0x286860 
    sw    a2,0(a0)
    lw    a2,0(a0)
    sw      a0,-1172(s0)
    
    li a3, 0
    li   s2, 2
.L4:
    nop
    nop
    nop
    nop
    li a0, 0x00204bcc        /* lw      a0,-1176(s1) */
    li    a1,0x00286870 
    nop
    li a5, 1                 /* c.lw    a5,8(a0)  */
    nop
    li a4, 0x286880          /* c.lw    a4,4(a0) */
    addi    t0,a5,1
    c.sw    a1,0(a4)
    sw      zero,0(a1)
    c.sw    a1,4(a0)
    sw      t0,8(a0) 
    nop 
    lw      a0,-1172(s0)
    nop                     /* li a0,0x204bd8 */
    c.lw    a4,8(a0)
    c.mv    a5,a0
    nop
    c.lw    a0,0(a0)
    addi    t0,a4,-1
    sw      t0,8(a5) 
    lw      t1,0(a0)
    sw      t1,0(a5)
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
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    addi a3, a3, 1
    bne  a3, s2, .L4

    li gp, 0x200a54
    li sp, 0x27ff20
    li t4, 0x201ac4
    li a3, 0
    li   a4, 2
.L5:
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
    sb      t6,1863(gp)
    sb      a2,1885(gp)
    sw      a1,1932(gp)
    sb      s0,1853(gp) 
    lw  a2,72(sp)
    sb      t0,1862(gp)
    lw  a5,68(sp)
    sb      t3,1881(gp)
    sw      t5,1928(gp)
    sb      s9,1882(gp)
    sb      a5,1910(gp)
    sb      a3,1908(gp)
    sb      s11,1912(gp)
    sb      a2,1906(gp)
    lw      s9,16(sp)
    lui     t3,514
    addi    t4,t3,-1340
    sb      s9,1883(gp)
    sb      s6,1839(gp)
    sb      s11,1884(gp)
    sb      s3,1879(gp)
    sb      s8,1852(gp)
    sb      s0,1845(gp)
    lw  s8,20(sp)
    sb      s0,5(t4)
    sb      s2,1877(gp)
    sw      zero,1968(gp)
    sb      s0,1846(gp)
    sb      s0,1836(gp)
    sb      s5,1847(gp)
    sb      s4,1848(gp)
    sb      s0,1838(gp)
    beq     s8,zero,216
    lw  a2,32(sp)
    c.mv    s8,s2
    c.mv    s9,s7  
    c.mv    s6,s3 
    sw  a2,76(sp)
    sw  s10,88(sp) 
    addi a3, a3, 1
    bne  a3, a4, .L5
end:
    j exit


