.include "utils.s"

.equ LOOP_CNT     ,64
.equ ICACHE_ADDR  ,0x00007ea0

;------------------------------------------------------------------------------
; Start of test code
;-------------------------------------------------------------------------------
.section    vectors,text
.global     _start
.text
.org     ICACHE_ADDR
_vectors:
    setvectors

;-------------------------------------------------------------------------------
; testcase starts
;-------------------------------------------------------------------------------
.section test, text
_start:
    initregs

    mov r3, 0x00022e68
    mov r12, 0x23300
    movl   r0,0x000225f08
    nop
    nop
    nop
.L1:
    nop
    ldb    r0,[r12,2]
    lsl      r1,r0,8
    ldb    r0,[r12,3]
    nop
    ldb    r0,[r12,0x40]
    lsl      r1,r0,8
    ldb    r0,[r12,0x80]

    add r12, r12, 0x100
    nop
    nop
    nop
    add r10, r10 ,1
    cmp r10, LOOP_CNT
    jne .L1

flag 1
