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

.global _test_memset
.org 0xcc
.type _test_memset, @function
_test_memset:
      li      t1,15
      mv      a4,a0
      bgeu    t1,a2,_test_memset_2a
      andi    a5,a4,15
      bnez    a5,_test_memset_8a
_test_memset_e:
      bnez    a1,_test_memset_78
_test_memset_10:
      andi    a3,a2,-16
      andi    a2,a2,15
      add     a3,a3,a4
_test_memset_18:
      sw      a1,0(a4)
      sw      a1,4(a4)
      sw      a1,8(a4)
      sw      a1,12(a4)
      addi    a4,a4,16
      bltu    a4,a3,_test_memset_18
      bnez    a2,_test_memset_2a
      ret
_test_memset_2a:
    sub     a3,t1,a2
     slli    a3,a3,0x2
     auipc   t0,0x0
     add     a3,a3,t0
     jr      10(a3)
     sb      a1,14(a4)
     sb      a1,13(a4)
     sb      a1,12(a4)
     sb      a1,11(a4)
     sb      a1,10(a4)
     sb      a1,9(a4)
     sb      a1,8(a4)
     sb      a1,7(a4)
     sb      a1,6(a4)
     sb      a1,5(a4)
     sb      a1,4(a4)
     sb      a1,3(a4)
     sb      a1,2(a4)
     sb      a1,1(a4)
     sb      a1,0(a4)
     nop
     nop
     nop
     nop
     ret
_test_memset_78:
     nop
     zext.b  a1,a1
     slli    a3,a1,0x8
     or      a1,a1,a3
     slli    a3,a1,0x10
     or      a1,a1,a3
     j       _test_memset_10
_test_memset_8a:
     slli    a3,a5,0x2
     auipc   t0,0x0
     add     a3,a3,t0
     mv      t0,ra
     jalr    -88(a3)
     mv      ra,t0
     addi    a5,a5,-16
     sub     a4,a4,a5
     add     a2,a2,a5
     bgeu    t1,a2,_test_memset_2a
     j       _test_memset_e

.macro setvectors
  .8byte  MemError
  .8byte  InstError
.MC_vec:
  .8byte  EV_MCheck
  .8byte  EV_IMMUFault
  .8byte  EV_DMMUFault
  .8byte  EV_ProtV
  .8byte  EV_PrivilegeV
  .8byte  EV_SWI
#  .8byte  trap_entry
  .8byte  EV_Extension
  .8byte  EV_DivZero
  .8byte  Unused
  .8byte  EV_Misaligned

  .align  2
EV_MCheck:
  ebreak
MemError:
  ebreak
InstError:
  ebreak
EV_MachineCheck:
  ebreak
EV_IMMUFault:
  ebreak
EV_ProtV:
  ebreak
EV_PrivilegeV:
  ebreak
EV_SWI:
  ebreak
EV_Trap:
  ebreak
EV_Extension:
  ebreak
EV_DivZero:
  ebreak
Unused:
  ebreak

  .align  2
EV_Misaligned:
  ebreak
.endm

.macro initregs
  mv      ra,zero
  mv      sp,zero
  mv      gp,zero
  mv      tp,zero
  mv      t0,zero
  mv      t1,zero
  mv      t2,zero
  mv      s0,zero
  mv      s1,zero
  mv      a0,zero
  mv      a1,zero
  mv      a2,zero
  mv      a3,zero
  mv      a4,zero
  mv      a5,zero
  mv      a6,zero
  mv      a7,zero
  mv      s2,zero
  mv      s3,zero
  mv      s4,zero
  mv      s5,zero
  mv      s6,zero
  mv      s7,zero
  mv      s8,zero
  mv      s9,zero
  mv      s10,zero
  mv      s11,zero
  mv      t3,zero
  mv      t4,zero
  mv      t5,zero
  mv      t6,zero
.endm

