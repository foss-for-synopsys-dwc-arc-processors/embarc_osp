# See LICENSE for license details.

#include "encoding.h"

#if __riscv_xlen == 64
# define LREG ld
# define SREG sd
# define REGBYTES 8
#else
# define LREG lw
# define SREG sw
# define REGBYTES 4
#endif

  .section ".text.init"
  .globl   _vectors
  .type    _vectors, @function
_vectors:
Reset:
   jal    _start
   ebreak                                 /* initially this halts     */

  .8byte  MemError
  .8byte  InstError
.MC_vec:
  .8byte  EV_MCheck
  .8byte  EV_IMMUFault
  .8byte  EV_DMMUFault
  .8byte  EV_ProtV
  .8byte  EV_PrivilegeV
  .8byte  EV_SWI
  .8byte  trap_entry
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
EV_DMMUFault:
  ebreak
EV_ProtV:
  ebreak
EV_PrivilegeV:
  ebreak
EV_SWI:
  ebreak
# EV_Trap:
#   ebreak
EV_Extension:
  ebreak
EV_DivZero:
  ebreak
Unused:
  ebreak

  .align  2
EV_Misaligned:
  ebreak
  
  .globl _start
  .align 4
_start:
  li  x1, 0
  li  x2, 0
  li  x3, 0
  li  x4, 0
  li  x5, 0
  li  x6, 0
  li  x7, 0
  li  x8, 0
  li  x9, 0
  li  x10,0
  li  x11,0
  li  x12,0
  li  x13,0
  li  x14,0
  li  x15,0
  li  x16,0
  li  x17,0
  li  x18,0
  li  x19,0
  li  x20,0
  li  x21,0
  li  x22,0
  li  x23,0
  li  x24,0
  li  x25,0
  li  x26,0
  li  x27,0
  li  x28,0
  li  x29,0
  li  x30,0
  li  x31,0

  # enable FPU and accelerator if present
#  li t0, MSTATUS_FS | MSTATUS_XS     # @@@ commented out for RTL simulation
#  csrs mstatus, t0                   # @@@ commented out for RTL simulation

  # make sure XLEN agrees with compilation choice
  li t0, 1
  slli t0, t0, 31
#if __riscv_xlen == 64
  bgez t0, 1f
#else
  bltz t0, 1f
#endif
2:
  li a0, 1
  sw a0, tohost, t0
  j 2b
1:

#ifdef __riscv_flen
  # initialize FPU if we have one
#  la t0, 1f
#  csrw mtvec, t0

#  fssr    x0
#  fmv.s.x f0, x0
#  fmv.s.x f1, x0
#  fmv.s.x f2, x0
#  fmv.s.x f3, x0
#  fmv.s.x f4, x0
#  fmv.s.x f5, x0
#  fmv.s.x f6, x0
#  fmv.s.x f7, x0
#  fmv.s.x f8, x0
#  fmv.s.x f9, x0
#  fmv.s.x f10,x0
#  fmv.s.x f11,x0
#  fmv.s.x f12,x0
#  fmv.s.x f13,x0
#  fmv.s.x f14,x0
#  fmv.s.x f15,x0
#  fmv.s.x f16,x0
#  fmv.s.x f17,x0
#  fmv.s.x f18,x0
#  fmv.s.x f19,x0
#  fmv.s.x f20,x0
#  fmv.s.x f21,x0
#  fmv.s.x f22,x0
#  fmv.s.x f23,x0
#  fmv.s.x f24,x0
#  fmv.s.x f25,x0
#  fmv.s.x f26,x0
#  fmv.s.x f27,x0
#  fmv.s.x f28,x0
#  fmv.s.x f29,x0
#  fmv.s.x f30,x0
#  fmv.s.x f31,x0
1:
#endif

  # initialize trap vector
#  la t0, trap_entry            # @@@ commented out for RTL simulation
#  csrw mtvec, t0               # @@@ commented out for RTL simulation

  # initialize global pointer
.option push
.option norelax
  la gp, __global_pointer$
.option pop

  la  tp, _end + 63
  and tp, tp, -64

  # get core id
#  csrr a0, mhartid             # @@@ commented out for RTL simulation
  # for now, assume only 1 core
#  li a1, 1                     # @@@ commented out for RTL simulation
#1:bgeu a0, a1, 1b              # @@@ commented out for RTL simulation

  la sp, __C_STACK_TOP__
  jal main
  ebreak


#  .align 2
trap_entry:
ebreak
#  addi sp, sp, -272
#
#  SREG x1, 1*REGBYTES(sp)
#  SREG x2, 2*REGBYTES(sp)
#  SREG x3, 3*REGBYTES(sp)
#  SREG x4, 4*REGBYTES(sp)
#  SREG x5, 5*REGBYTES(sp)
#  SREG x6, 6*REGBYTES(sp)
#  SREG x7, 7*REGBYTES(sp)
#  SREG x8, 8*REGBYTES(sp)
#  SREG x9, 9*REGBYTES(sp)
#  SREG x10, 10*REGBYTES(sp)
#  SREG x11, 11*REGBYTES(sp)
#  SREG x12, 12*REGBYTES(sp)
#  SREG x13, 13*REGBYTES(sp)
#  SREG x14, 14*REGBYTES(sp)
#  SREG x15, 15*REGBYTES(sp)
#  SREG x16, 16*REGBYTES(sp)
#  SREG x17, 17*REGBYTES(sp)
#  SREG x18, 18*REGBYTES(sp)
#  SREG x19, 19*REGBYTES(sp)
#  SREG x20, 20*REGBYTES(sp)
#  SREG x21, 21*REGBYTES(sp)
#  SREG x22, 22*REGBYTES(sp)
#  SREG x23, 23*REGBYTES(sp)
#  SREG x24, 24*REGBYTES(sp)
#  SREG x25, 25*REGBYTES(sp)
#  SREG x26, 26*REGBYTES(sp)
#  SREG x27, 27*REGBYTES(sp)
#  SREG x28, 28*REGBYTES(sp)
#  SREG x29, 29*REGBYTES(sp)
#  SREG x30, 30*REGBYTES(sp)
#  SREG x31, 31*REGBYTES(sp)
#
#  #csrr a0, mcause
#  #csrr a1, mepc
#  #mv a2, sp
#  #jal handle_trap
#  #csrw mepc, a0
#
#  # Remain in M-mode after eret
#  li t0, MSTATUS_MPP
#  #csrs mstatus, t0
#
#  LREG x1, 1*REGBYTES(sp)
#  LREG x2, 2*REGBYTES(sp)
#  LREG x3, 3*REGBYTES(sp)
#  LREG x4, 4*REGBYTES(sp)
#  LREG x5, 5*REGBYTES(sp)
#  LREG x6, 6*REGBYTES(sp)
#  LREG x7, 7*REGBYTES(sp)
#  LREG x8, 8*REGBYTES(sp)
#  LREG x9, 9*REGBYTES(sp)
#  LREG x10, 10*REGBYTES(sp)
#  LREG x11, 11*REGBYTES(sp)
#  LREG x12, 12*REGBYTES(sp)
#  LREG x13, 13*REGBYTES(sp)
#  LREG x14, 14*REGBYTES(sp)
#  LREG x15, 15*REGBYTES(sp)
#  LREG x16, 16*REGBYTES(sp)
#  LREG x17, 17*REGBYTES(sp)
#  LREG x18, 18*REGBYTES(sp)
#  LREG x19, 19*REGBYTES(sp)
#  LREG x20, 20*REGBYTES(sp)
#  LREG x21, 21*REGBYTES(sp)
#  LREG x22, 22*REGBYTES(sp)
#  LREG x23, 23*REGBYTES(sp)
#  LREG x24, 24*REGBYTES(sp)
#  LREG x25, 25*REGBYTES(sp)
#  LREG x26, 26*REGBYTES(sp)
#  LREG x27, 27*REGBYTES(sp)
#  LREG x28, 28*REGBYTES(sp)
#  LREG x29, 29*REGBYTES(sp)
#  LREG x30, 30*REGBYTES(sp)
#  LREG x31, 31*REGBYTES(sp)
#
#  addi sp, sp, 272
#  mret
#
#   .align 3
#   .globl  strcmp
#    nop
##if __riscv_xlen == 64
#strcmp:
#    ld    a5,0(a0)
#    ld    a4,0(a1)
#    bne   a5,a4,strcmp_l1
#    auipc a2,0x1
#    ld    a2,-2008(a2) # 3428 <abort>
#    auipc a3,0x1
#    ld    a3,-2008(a3) # 3430 <abort+0x8>
#    j     strcmp_l2
#strcmp_l3:  
#    ld    a5,0(a0)
#    ld    a4,0(a1)
#    bne   a5,a4,strcmp_l1
#strcmp_l2: 
#    add   a4,a5,a2
#    not   a5,a5
#    and   a5,a4,a5
#    addi  a0,a0,8
#    and   a5,a5,a3
#    addi  a1,a1,8
#    beqz  a5,strcmp_l3
#    li    a0,0
#    ret
#strcmp_l5:
#    addi  a0,a0,1
#    addi  a1,a1,1
#    bne   a5,a4,strcmp_l4
#strcmp_l1:
#    lbu   a5,0(a0)
#    lbu   a4,0(a1)
#    bnez  a5,strcmp_l5
#    li    a0,0
#    subw  a0,a0,a4
#    ret
#strcmp_l4:
#    sext.w  a0,a5
#    subw    a0,a0,a4
#    ret
##else
#strcmp:
#    lw    a5,0(a0)
#    lw    a4,0(a1)
#    bne   a5,a4,strcmp_l1
#    auipc a2,0x1
#    lw    a2,-2008(a2) # 3428 <abort>
#    auipc a3,0x1
#    lw    a3,-2008(a3) # 3430 <abort+0x8>
#    j     strcmp_l2
#strcmp_l3:  
#    lw    a5,0(a0)
#    lw    a4,0(a1)
#    bne   a5,a4,strcmp_l1
#strcmp_l2: 
#    add   a4,a5,a2
#    not   a5,a5
#    and   a5,a4,a5
#    addi  a0,a0,4
#    and   a5,a5,a3
#    addi  a1,a1,4
#    beqz  a5,strcmp_l3
#    li    a0,0
#    ret
#strcmp_l5:
#    addi  a0,a0,1
#    addi  a1,a1,1
#    bne   a5,a4,strcmp_l4
#strcmp_l1:
#    lbu   a5,0(a0)
#    lbu   a4,0(a1)
#    bnez  a5,strcmp_l5
#    li    a0,0
#    sub   a0,a0,a4
#    ret
#strcmp_l4:
#    mv      a0,a5
#    sub     a0,a0,a4
#    ret
##endif

.section ".tohost","aw",@progbits
.align 6
.globl tohost
tohost: .dword 0
.align 6
.globl fromhost
fromhost: .dword 0
