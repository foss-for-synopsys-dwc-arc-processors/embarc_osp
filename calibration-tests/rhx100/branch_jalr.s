

 /*
;; riscv64-elf-gcc -march=rv32i -mabi=ilp32 -static -nostdlib -nostartfiles -T link.ld enclose_branches_into_one_fetch_block.s -o enclose_branches_into_one_fetch_block.out
*/

.include "utils.s"

.equ LOOP_CNT     ,32
.equ ICACHE_ADDR  ,0x0040



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
  nop
  nop
  nop
  nop
  nop
  nop
  initregs

  li s0, LOOP_CNT

.L1:
    li a0, 0x00201ac4
    li a2, 0x00000040
    li t0, 0x00202000
    li gp, 0x200A54
    li sp, 0x27ff20
    li s5, 2

    nop
    # addi s1, s1, 1
    jal ra, _benchmark_body
    # bne s1, s0, .L1

end:
    j exit

    
.global _benchmark_body
.org 0x5dea
.section .icache, "ax"
.type _benchmark_body, @function
_benchmark_body:
_benchmark_body_5dfc:
    li a0, 0x00201ac4
    li a2, 0x00000040
    li t0, 0x00202000
    addi tp, tp, 2
    beq tp, a2, exit
    jal ra, _test_memset
    li a5, 0
    sw      zero,1988(gp)
    sw      zero,1992(gp) # 20121c <tm_entered_EINSCHALTSTROM_MESSEN_BLOCK_ERKENNUNG_CTRLch_BLOCK_ERKENNUNG_CTRL__N_copy>
    sb      zero,1851(gp) # 20118f <NICHT_INITIALISIERT_NICHT_INITIALISIERT_next_state>
    sw      zero,1984(gp) # 201214 <tm_entered_WIEDERHOLSPERRE_FH_TUERMODUL_CTRL>
    sb      zero,1849(gp) # 20118d <MEC_KINDERSICHERUNG_CTRL_next_state>
    sb      zero,1850(gp) # 20118e <ZENTRAL_KINDERSICHERUNG_CTRL_next_state>
    sb      zero,1843(gp) # 201187 <TIPP_SCHLIESSEN_FH_TUERMODUL_CTRL_next_state>
    sb      zero,1844(gp) # 201188 <INITIALISIERT_FH_TUERMODUL_CTRL_next_state>
    sb      zero,1841(gp) # 201185 <OEFFNEN_FH_TUERMODUL_CTRL_next_state>
    sb      zero,1842(gp) # 201186 <MANUELL_SCHLIESSEN_FH_TUERMODUL_CTRL_next_state>
    sb      zero,1840(gp) # 201184 <SCHLIESSEN_FH_TUERMODUL_CTRL_next_state>
    sb      zero,1837(gp) # 201181 <BEWEGUNG_BLOCK_ERKENNUNG_CTRL_next_state>
    bnez    a5,_benchmark_body_604c
    lbu     a2,1889(gp) # 2011b5 <FH_TUERMODUL__MFHA_copy>
_benchmark_body_5e48:
    lw      a3,40(sp)
    beqz    a3,_benchmark_body_5e5c
    lw      a7,1856(gp) # 201194 <time>
    beq     a7,a3,_benchmark_body_5e5e
    sb      zero,1892(gp) # 2011b8 <FH_TUERMODUL__MFHZ_copy>
    sw      zero,1976(gp) # 20120c <sc_FH_TUERMODUL_CTRL_2352_1>
_benchmark_body_5e5c:
    sw      zero,40(sp)
_benchmark_body_5e5e:
    lw      t5,80(sp)
    bnez    t5,_benchmark_body_6034
    lbu     a3,1892(gp) # 2011b8 <FH_TUERMODUL__MFHZ_copy>
_benchmark_body_5e68:
    lw      a0,36(sp)
    beqz    a0, _benchmark_body_5e78
    lw      a6,1856(gp) # 201194 <time>
    beq     a6,a0,_benchmark_body_5e7a
    sw      zero,1960(gp) # 2011fc <sc_FH_TUERMODUL_CTRL_1781_10>
_benchmark_body_5e78:
    sw      zero,36(sp)
_benchmark_body_5e7a:
    lw      t3,64(sp)
    beqz    t3,_benchmark_body_5e8c
    lw      t5,1856(gp) # 201194 <time>
    beq     t5,t3,_benchmark_body_5e8e
    sw      zero,1956(gp) # 2011f8 <sc_FH_TUERMODUL_CTRL_1739_10>
_benchmark_body_5e8c:
    sw      zero,64(sp)
_benchmark_body_5e8e:
    lw      a4,32(sp)
    lw      a5,76(sp)
     beq     a5,a4,_benchmark_body_5e9e
     lw      t0,1856(gp) # 201194 <time>
     sw      t0,1992(gp) # 20121c <tm_entered_EINSCHALTSTROM_MESSEN_BLOCK_ERKENNUNG_CTRLch_BLOCK_ERKENNUNG_CTRL__N_copy>
_benchmark_body_5e9e:
     lw      t2,108(sp)
     sw      s0,1856(gp) # 201194 <time>
     beqz    t2,_benchmark_body_6004
     beq     s2,s8,_benchmark_body_6074
     beq     s9,s7,_benchmark_body_6088
     beq     s3,s6,_benchmark_body_60b4
     lw      t6,88(sp)
     beq     t6,s10,_benchmark_body_5ebe
     mv      s3,s10
     mv      s2,s7
_benchmark_body_5ebe:
     li      s6,2
     mv      s8,s6
_benchmark_body_5ec2:
     beq     s6,s5,_benchmark_body_5ed4
     beq     s6,s4,_benchmark_body_5ed4
     lw      a4,12(sp)
     bnez    a4,_benchmark_body_602c
     li      s6,1
     nop
_benchmark_body_5ed4:
     lw      t0,20(sp)
     lw      s9,24(sp)
     lw      a0,96(sp)
     addi    t1,t0,-1
     lw      a7,24(sp)
     sw      t1,20(sp)
     lw      t4,32(sp)
     lw      a4,68(sp)
     sw      a0,1940(gp) # 2011e8 <FH_DU__MFH>
     lw      a0,52(sp)
     lw      t0,16(sp)
     sw      a7,1924(gp) # 2011d8 <FH_DU__I_EIN_old>
     lw      a7,56(sp)
     sb      s11,1911(gp) # 2011cb <FH_TUERMODUL__SFHZ_ZENTRAL_old>
     lw      t5,60(sp)
     sw      t4,1916(gp) # 2011d0 <BLOCK_ERKENNUNG_CTRL__N_old>
     sb      a4,1909(gp) # 2011c9 <FH_TUERMODUL__SFHZ_MEC_old>
     sb      a0,1901(gp) # 2011c1 <FH_TUERMODUL__BLOCK_old>
     sb      a7,1899(gp) # 2011bf <FH_TUERMODUL__SFHZ>
     sb      a7,1897(gp) # 2011bd <FH_TUERMODUL__SFHZ_old>
     sb      t5,1896(gp) # 2011bc <FH_TUERMODUL__SFHA>
     sb      t5,1894(gp) # 2011ba <FH_TUERMODUL__SFHA_old>
     sb      a3,1893(gp) # 2011b9 <FH_TUERMODUL__MFHZ>
     sw      zero,1964(gp) # 201200 <FH_TUERMODUL_CTRL__N_old>
     sw      s9,1944(gp) # 2011ec <FH_TUERMODUL__I_EIN_old>
     lw      t2,72(sp)
     sb      t0,1907(gp) # 2011c7 <FH_TUERMODUL__SFHA_ZENTRAL_old>
     sb      a0,1903(gp) # 2011c3 <FH_TUERMODUL__BLOCK>
     sb      t2,1905(gp) # 2011c5 <FH_TUERMODUL__SFHA_MEC_old>
     sb      a3,1891(gp) # 2011b7 <FH_TUERMODUL__MFHZ_old>
     sb      a2,1888(gp) # 2011b4 <FH_TUERMODUL__MFHA_old>
     sb      a2,1890(gp) # 2011b6 <FH_TUERMODUL__MFHA>
     lw      a2,28(sp)
     sb      a2,1886(gp) # 2011b2 <FH_TUERMODUL__EKS_LEISTE_AKTIV_old>
     lw      t3,48(sp)
     lw      t0,44(sp)
     lw      t6,12(sp)
     sb      s3,1878(gp) # 2011aa <FH_DU__S_FH_TMBFAUFCAN_old>
     sb      a2,1880(gp) # 2011ac <FH_DU__EKS_LEISTE_AKTIV_old>
     sb      s2,1876(gp) # 2011a8 <FH_DU__S_FH_TMBFZUCAN_old>
     lw      a2,92(sp)
     sb      s7,1874(gp) # 2011a6 <FH_DU__S_FH_TMBFZUDISC_old>
     lw      a1,104(sp)
     lw      s9,100(sp)
     sb      s10,1872(gp) # 2011a4 <FH_DU__S_FH_TMBFAUFDISC_old>
     sb      t3,1868(gp) # 2011a0 <FH_DU__BLOCK>
     sb      t3,1866(gp) # 20119e <FH_DU__BLOCK_old>
     lw      t3,28(sp)
     sb      t6,1865(gp) # 20119d <FH_DU__MFHZ>
     lw      t5,24(sp)
     sb      t0,1860(gp) # 201198 <FH_DU__MFHA_old>
     lw      a3,16(sp)
     sb      t6,1863(gp) # 20119b <FH_DU__MFHZ_old>
     sb      a2,1885(gp) # 2011b1 <FH_DU__KL_50>
     sw      a1,1932(gp) # 2011e0 <FH_DU__POSITION>
     sb      s0,1853(gp) # 201191 <stable>
     lw      a2,72(sp)
     sb      t0,1862(gp) # 20119a <FH_DU__MFHA>
     lw      a5,68(sp)
     sb      t3,1881(gp)
     sw      t5,1928(gp)
     sb      s9,1882(gp)
     sb      a5,1910(gp)
     sb      a3,1908(gp)
     sb      s11,1912(gp)
     sb      a2,1906(gp)
     lw      s9,16(sp)
     lui     t3,0x202
     addi    t4,t3,-1340
     sb      s9,1883(gp)
     sb      s6,1839(gp)
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
     sb      s4,1848(gp)
     sb      s0,1838(gp)
     beqz    s8,_benchmark_body_60c8
     lw      a2,32(sp)
     mv      s8,s2
     mv      s9,s7
     mv      s6,s3
     sw      a2,76(sp)
     sw      s10,88(sp)
     j       _benchmark_body_5dfc
     nop
_benchmark_body_6004:
     beq     s2,s8,_benchmark_body_6064
     beq     s9,s7,_benchmark_body_609c
     beq     s3,s6,_benchmark_body_60c0
     mv      s11,s2
_benchmark_body_6012:
     li      s6,2
     sw      s3,16(sp)
     mv      s8,s6
_benchmark_body_6018:
     beq     s6,s5,_benchmark_body_5ed4
     beq     s6,s4,_benchmark_body_5ed4
     lw      s9,12(sp)
     bnez    s9,_benchmark_body_6030
     li      s6,1
     j       _benchmark_body_5ed4
     nop
_benchmark_body_602c:
     li      s6,1
     j       _benchmark_body_5ed4
_benchmark_body_6030:
     li      s6,1
     j       _benchmark_body_5ed4
_benchmark_body_6034:
     lw      a5,1856(gp)
     beq     a5,t5,_benchmark_body_60a4
     sb      zero,1892(gp)
     sw      zero,1972(gp)
     sw      zero,80(sp)
     li      a3,0
     j       _benchmark_body_5e68
     nop
_benchmark_body_604c:
     lw      t1,1856(gp)
     beq     t1,a5,_benchmark_body_60ac
     sb      zero,1889(gp)
     sw      zero,1980(gp)
     sw      zero,84(sp)
     li      a2,0
     j       _benchmark_body_5e48
     nop
_benchmark_body_6064:
     beq     s9,s7,_benchmark_body_60fc
     bne     s3,s6,_benchmark_body_6012
     li      s6,2
     li      s8,2
     j       _benchmark_body_6018
     nop
_benchmark_body_6074:
     beq     s9,s7,_benchmark_body_60e8
     beq     s3,s6,_bmark_end
     lw      a7,88(sp)
     beq     a7,s10,_bmark_end
     mv      s3,s10
     mv      s2,s7
     j       _benchmark_body_5ebe
_benchmark_body_6088:
     beq     s3,s6,_bmark_end
     lw      t4,88(sp)
     beq     t4,s10,_benchmark_body_5ebe
     li      s6,2
     mv      s3,s10
     mv      s8,s6
     j       _benchmark_body_5ec2
     nop
_benchmark_body_609c:
    beq     s3,s6,_bmark_end
    mv      s11,s2
    j       _benchmark_body_6012
_benchmark_body_60a4:
    lbu     a3,1892(gp)
    j       _benchmark_body_5e68
    nop
_benchmark_body_60ac:
    lbu     a2,1889(gp)
    j       _benchmark_body_5e48
    nop
_benchmark_body_60b4:
    lw      t5,88(sp)
    beq     t5,s10,_bmark_end
    mv      s3,s10
    mv      s2,s7
    j       _benchmark_body_5ebe
_benchmark_body_60c0:
    mv      s11,s2
    li      s6,2
    li      s8,2
    j       _benchmark_body_6018
_benchmark_body_60c8:
    lw      s11,124(sp)
    lw      ra,172(sp)
    lw      s9,132(sp)
    lw      s7,140(sp)
    lw      s5,148(sp)
    lw      s3,156(sp)
    lw      s1,164(sp)
    lw      s10,128(sp)
    lw      s8,136(sp)
    lw      s6,144(sp)
    lw      s4,152(sp)
    lw      s2,160(sp)
    lw      s0,168(sp)
    addi    sp,sp,176
    ret
    nop
_benchmark_body_60e8:
    beq     s3,s6,_bmark_end
    lw      a1,88(sp)
    beq     a1,s10,_benchmark_body_5ebe
    li      s6,2
    mv      s3,s10
    mv      s8,s6
    j       _bmark_end
    nop
_benchmark_body_60fc:
    bne     s3,s6,_benchmark_body_6012
    li      s6,2
    li      s8,2
    j       _benchmark_body_6018

_bmark_end:
    j exit
