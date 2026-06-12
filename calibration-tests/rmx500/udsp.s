// /remote/arc/mwdt_builds/latest_daily/Linux/ARC/MetaWare/arc/bin/ccac -Wo,-verify-changed -Wcg,-verify-machineinstrs -Hnocopyr -av5rmx -Zce -Zxsnpsudsp -O -c vector.S -o vector.elf

  .section ".text.init"
  .globl _start
_start:
    la x10, data
    li x1, 0x70017000
    li x2, 0x00040003
    li x3, -1
    snps.vscmul.vv  x3, x2, x1, e16, m1
    snps.vscjmul.vv  x3, x2, x1, e16, m1

    snps.vwcmul.vv  x4, x2, x1, e16, m1
    addi x6, x4, 1
    snps.vwcjmul.vv  x4, x2, x1, e16, m1
    addi x6, x4, 1

    snps.vwcmul.vv  x4, x2, x1, e16, m1
    addi x6, x5, 1
    snps.vwcjmul.vv  x4, x2, x1, e16, m1
    addi x6, x5, 1

    li x1, 0xff078001
    li x2, 0x00030002
    li x3, -1
    vadd.vv  x3, x2, x1, e16, m1
    vsub.vv  x3, x2, x1, e16, m1
    vssub.vv x3, x2, x1, e16, m1
    vsadd.vv x3, x2, x1, e16, m1

    vssub.vv x3, x2, x1, e32, m1
    vsadd.vv x3, x2, x1, e32, m1

    vsll.vx x3, x1, x2, e16, m1
    vsra.vx x3, x1, x2, e16, m1

    vwmul.vv  x3, x2, x1, e16, mf2
    mv x4, x4
    vnclip.wx x3, x2, x1, e16, mf2


    vwmul.vv  x4, x2, x1, e16, m1
    mv x5, x5
    vnclip.wx x3, x2, x1, e16, m1
    vadd.vv  x3, x2, x1, e16, m1

    .word 0x0020A25B
    addi x6, x4, 1


.global exit
exit:
.equ SYS_EXIT                     ,0x18
.equ ADP_Stopped_ApplicationExit  ,0x20026
.option push
.option norvc
  li   a0, SYS_EXIT
  lui  a1, ADP_Stopped_ApplicationExit >> 12
  add  a1, a1, ADP_Stopped_ApplicationExit & 0xfff
  slli x0, x0, 0x1f
  ebreak
  srai x0, x0, 7
.option pop
