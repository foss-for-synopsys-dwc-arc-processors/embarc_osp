.ifndef UTILS_S
.equ    UTILS_S, 0xABADCAFE

;-------------------------------------------------------------------------------
; Constant definitions
;-------------------------------------------------------------------------------
.equ TEST_PASS    ,0x6666_6666_6666_6666
.equ TEST_FAIL    ,0x9999_9999_9999_9999
.equ OOPS         ,0xfefe_fefe_fefe_fefe
.equ TIMER0_CTRL  ,0x22
.equ TIMER0_LIMIT ,0x23
.equ TIMER0_COUNT ,0x21
.equ CTRL_VALUE   ,0x3
.equ LIMIT_VALUE  ,0xffff_ffff
.equ COUNT_VALUE  ,0x0

.macro initregs
    movl        r0,0
    movl_s      r1,r0
    movl_s      r2,r0
    movl_s      r3,r0
    movl_s      r4,r0
    movl_s      r5,r0
    movl_s      r6,r0
    movl_s      r7,r0
    movl_s      r8,r0
    movl_s      r9,r0
    movl_s      r10,r0
    movl_s      r11,r0
    movl_s      r12,r0
    movl_s      r13,r0
    movl_s      r14,r0
    movl_s      r15,r0
    movl_s      r16,r0
    movl_s      r17,r0
    movl_s      r18,r0
    movl_s      r19,r0
    movl_s      r20,r0
    movl_s      r21,r0
    movl_s      r22,r0
    movl_s      r23,r0
    movl_s      r24,r0
    movl_s      r25,r0
    movl_s      r26,r0
.endm


.macro setvectors
    .quad       _start                      ; addr 0x000, vec 0x00
    .quad       Memory_Error            ; addr 0x008, vec 0x01
    .quad       Instruction_Error       ; addr 0x010, vec 0x02
    .quad       EV_MachineCheck         ; addr 0x018, vec 0x03
    .quad       EV_TLBmissI                 ; addr 0x020, vec 0x04
    .quad       EV_TLBmissD                 ; addr 0x020, vec 0x05
    .quad       EV_ProtV                    ; addr 0x030, vec 0x06
    .quad       EV_PrivilegeV           ; addr 0x038, vec 0x07
    .quad       EV_SWI                      ; addr 0x040, vec 0x08
    .quad       EV_Trap                     ; addr 0x048, vec 0x09
    .quad       EV_Extension            ; addr 0x050, vec 0x0A
    .quad       EV_DivZero                  ; addr 0x058, vec 0x0B
    .quad       EV_DCError                  ; addr 0x060, vec 0x0C
    .quad       EV_Maligned                 ; addr 0x068, vec 0x0D
    .quad       EV_Unused1                  ; addr 0x070, vec 0x0E
    .quad       EV_Unused2                  ; addr 0x078, vec 0x0F
.endm

;============================================================================= ;
;========================= INTERRUPT AND ERROR HANDLERS ====================== ;
;============================================================================= ;
    .global Memory_Error
    .global Instruction_Error
    .global EV_MachineCheck
    .global EV_TLBmissI
    .global EV_TLBmissD
    .global EV_ProtV
    .global EV_Maligned
    .global EV_PrivilegeV
    .global EV_SWI
    .global EV_Trap
    .global EV_Extension
    .global EV_DivZero
    .global EV_DCError
    .global EV_Unused1
    .global EV_Unused2
Memory_Error:
Instruction_Error:
EV_MachineCheck:
EV_TLBmissI:
EV_TLBmissD:
EV_ProtV:
EV_Maligned:
EV_PrivilegeV:
EV_SWI:
EV_Trap:
EV_Extension:
EV_DivZero:
EV_DCError:
EV_Unused1:
EV_Unused2:
    b_s Oops

;------------------------------------------------------------------------------
; Exception
;------------------------------------------------------------------------------
Oops:
    movl     r30, OOPS
        flag 1

;------------------------------------------------------------------------------
; Test passed
;------------------------------------------------------------------------------
    .global passed
passed:
        movl     r30, TEST_PASS
        flag 1

;------------------------------------------------------------------------------
; Test failed
;------------------------------------------------------------------------------
    .global failed
failed:
        movl     r30, TEST_FAIL
        flag 1

.endif        ;UTILS_S
