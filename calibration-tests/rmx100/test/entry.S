.section ".text"
.globl _start
_start:
  # initialize global pointer
.option push
.option norelax
  la gp, __global_pointer$
.option pop

  la  tp, _end + 63
  and tp, tp, -64
  la sp, __C_STACK_TOP__
  jal main
  j _exit
