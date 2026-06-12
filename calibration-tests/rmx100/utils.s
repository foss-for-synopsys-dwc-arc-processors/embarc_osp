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
