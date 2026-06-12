.include "utils.s"

.equ LOOP_CNT     ,16
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
    lr	r5, [DC_CTRL]
    mov	r0, 0x1
	sr	r0, [DC_CTRL]

    mov r3, 0x00022e68
    mov r12, 0x23340
    mov  r0,0x000225f08
    nop
    nop
    nop
.L1:
    nop
    ld    r0,[r12]
    ld    r1,[r12, 0x40]
    ld    r2,[r12, 0x80]
    ld    r3,[r12, 0xc0]
    ld    r3,[r12, 0x100]
    mov    r4, r3

    add r12, r12, 0x140
    nop
    nop
    nop
    add r10, r10 ,1
    cmp r10, LOOP_CNT
    jne .L1

    mov r3, 0x00022e68
    mov  r0,0x000225f08
    nop
    nop
    nop
.L2:
    nop
    ldd    r0,[r12]
    mov    r4, r0
    ldd    r0,[r12, 0x40]
    mov    r4, r0
    add r12, r12, 0x200
    nop
    nop
    nop
    dbnz  r10, .L2
sr	r5, [DC_CTRL]
    mov r3, 0x00022e68
    mov r12, 0x23340
    mov  r0,0x000225f08
    nop
    nop
    nop
.L3:
    nop
    ld    r0,[r12]
    ld    r1,[r12, 0x40]
    ld    r2,[r12, 0x80]
    ld    r3,[r12, 0xc0]
    ld    r3,[r12, 0x100]
    mov    r4, r3

    add r12, r12, 0x140
    nop
    nop
    nop
    add r10, r10 ,1
    cmp r10, LOOP_CNT
    jne .L3

    mov r3, 0x00022e68
    mov  r0,0x000225f08
    nop
    nop
    nop
.L4:
    nop
    ldd    r0,[r12]
    mov    r4, r0
    nop
    nop
    ldd    r0,[r12, 0x40]
    mov    r4, r0
    add r12, r12, 0x200
    nop
    nop
    nop
    dbnz  r10, .L4
flag 1
