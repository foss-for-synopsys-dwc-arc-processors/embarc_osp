/*
 * Copyright (c) 2021, Synopsys, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define __ASSEMBLY__
#include "arc.h"
#include "arc_asm_common.h"

.global OSTCBCur;
.global OSTCBHighRdy;
.global OSRunning;
.global OSPrioCur
.global OSPrioHighRdy

.global OSIntEnter
.global OSIntExit
.global OSTaskSwHook

.global g_exc_nest_count;
.global g_context_switch_reqflg;
.global set_hw_stack_check;

    .text
    .align 4
dispatcher:
    st sp, [r0]	//    ld 	r0, [OSTCBCur]
    ld sp, [r1]	//    ld 	r1, [OSTCBHighRdy]
# OSTCBCur = OSTCBHighRdy;
    st r1, [OSTCBCur]
# OSPrioCur = OSPrioHighRdy;
    ldb r0, [OSPrioHighRdy]
    stb r0, [OSPrioCur]

#if ARC_FEATURE_STACK_CHECK
#if ARC_FEATURE_SEC_PRESENT
    lr r0, [AUX_SEC_STAT]
    bclr r0, r0, AUX_SEC_STAT_BIT_SSC
    sflag r0
#else
    lr r0, [AUX_STATUS32]
    bclr r0, r0, AUX_STATUS_BIT_SC
    kflag r0
#endif
    jl	set_hw_stack_check
#if ARC_FEATURE_SEC_PRESENT
    lr r0, [AUX_SEC_STAT]
    bset r0, r0, AUX_SEC_STAT_BIT_SSC
    sflag r0
#else
    lr r0, [AUX_STATUS32]
    bset r0, r0, AUX_STATUS_BIT_SC
    kflag r0
#endif
#endif
    pop r0
    j [r0]

/* return routine when task dispatch happened in task context */
dispatch_r:
    RESTORE_NONSCRATCH_REGS
    j	[blink]

/*
 * OS_CPU_SR OS_CPU_SR_Save(void);
 */
    .global OS_CPU_SR_Save
    .align 4
OS_CPU_SR_Save:
    clri r0
    j [blink]

/*
 * void OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
 */
    .global OS_CPU_SR_Restore
    .align 4
OS_CPU_SR_Restore:
    seti r0
    j [blink]

/*
 * void OSStartHighRdy(void);
 */
    .global OSStartHighRdy
    .align 4
OSStartHighRdy:

    jl OSTaskSwHook

    mov r0, 1
    st r0, [OSRunning]

    ld r0, [OSTCBCur]
    ld sp, [r0]
#if ARC_FEATURE_STACK_CHECK
    mov r1, r0
#if ARC_FEATURE_SEC_PRESENT
    lr r0, [AUX_SEC_STAT]
    bclr r0, r0, AUX_SEC_STAT_BIT_SSC
    sflag r0
#else
    lr r0, [AUX_STATUS32]
    bclr r0, r0, AUX_STATUS_BIT_SC
    kflag r0
#endif
    jl	set_hw_stack_check
#if ARC_FEATURE_SEC_PRESENT
    lr r0, [AUX_SEC_STAT]
    bset r0, r0, AUX_SEC_STAT_BIT_SSC
    sflag r0
#else
    lr r0, [AUX_STATUS32]
    bset r0, r0, AUX_STATUS_BIT_SC
    kflag r0
#endif
#endif
    pop r0
    j [r0]

/*
 * void OSIntCtxSw(void);
 */
    .global OSIntCtxSw
    .align 4
OSIntCtxSw:
    mov r0, 1
    st r0, [g_context_switch_reqflg]
    j [blink]

/*
 * void OSCtxSw(void);
 */
    .global OSCtxSw
    .align 4
OSCtxSw:
    SAVE_NONSCRATCH_REGS
    mov r2, dispatch_r
    push r2
    ld 	r0, [OSTCBCur]
    ld 	r1, [OSTCBHighRdy]
    b dispatcher

/*
 * void start_r(void);
 */
    .global start_r
    .align 4
start_r:
    pop blink;
    pop r1
    pop r2
    pop r0

    j_s.d [r1]
    kflag r2

/****** exceptions and interrupts handing ******/
/****** entry for exception handling ******/
    .global exc_entry_cpu
    .align 4
exc_entry_cpu:

    EXCEPTION_PROLOGUE

    mov	blink,	sp
    mov	r3, sp		/* as exception handler's para(p_excinfo) */

    ld	r0, [g_exc_nest_count]
    add	r1, r0, 1
    st	r1, [g_exc_nest_count]
    brne	r0, 0, exc_handler_1
/* change to exception stack if interrupt happened in task context */
    mov	sp, _e_stack
exc_handler_1:
    PUSH	blink

    jl 	OSIntEnter

    lr	r0, [AUX_ECR]
    lsr	r0, r0, 16
    mov	r1, exc_int_handler_table
    ld.as	r2, [r1, r0]

    mov	r0, r3
    jl	[r2]

    jl 	OSIntExit

/* interrupts are not allowed */
ret_exc:
    POP	sp
    mov	r1, g_exc_nest_count
    ld	r0, [r1]
    sub	r0, r0, 1
    st	r0, [r1]
    brne	r0, 0, ret_exc_1 /* nest exception case */
    lr	r1, [AUX_IRQ_ACT] /* nest interrupt case */
    brne	r1, 0, ret_exc_1

    ld	r0, [g_context_switch_reqflg]
    brne	r0, 0, ret_exc_2
ret_exc_1:	/* return from non-task context, interrupts or exceptions are nested */
    EXCEPTION_EPILOGUE
    rtie

/* there is a dispatch request */
ret_exc_2:
    /* clear dispatch request */
    mov	r0, 0
    st	r0, [g_context_switch_reqflg]

    SAVE_CALLEE_REGS	/* save callee save registers */

    /* clear exception bit to do exception exit by SW */
    lr	r0, [AUX_STATUS32]
    bclr	r0, r0, AUX_STATUS_BIT_AE
    kflag	r0

    mov	r1, ret_exc_r	/* save return address */
    PUSH	r1

    ld 	r0, [OSTCBCur]
    ld 	r1, [OSTCBHighRdy]
    b	dispatcher

ret_exc_r:
    /* recover exception status */
    lr	r0, [AUX_STATUS32]
    bset	r0, r0, AUX_STATUS_BIT_AE
    kflag	r0

    RESTORE_CALLEE_REGS
    EXCEPTION_EPILOGUE
    rtie

/****** entry for normal interrupt exception handling ******/
    .global exc_entry_int	/* entry for interrupt handling */
    .align 4
exc_entry_int:
#if ARC_FEATURE_FIRQ == 1
/*  check whether it is P0 interrupt */
#if ARC_FEATURE_RGF_NUM_BANKS > 1
    lr	r0, [AUX_IRQ_ACT]
    btst	r0, 0
    jnz	exc_entry_firq
#else
    PUSH	r10
    lr	r10, [AUX_IRQ_ACT]
    btst	r10, 0
    POP	r10
    jnz	exc_entry_firq
#endif
#endif
    INTERRUPT_PROLOGUE

    mov	blink, sp

    clri	/* disable interrupt */
    ld	r3, [g_exc_nest_count]
    add	r2, r3, 1
    st	r2, [g_exc_nest_count]
    seti	/* enable higher priority interrupt */

    brne	r3, 0, irq_handler_1
/* change to exception stack if interrupt happened in task context */
    mov	sp, _e_stack
#if ARC_FEATURE_STACK_CHECK
#if ARC_FEATURE_SEC_PRESENT
    lr r0, [AUX_SEC_STAT]
    bclr r0, r0, AUX_SEC_STAT_BIT_SSC
    sflag r0
#else
    lr r0, [AUX_STATUS32]
    bclr r0, r0, AUX_STATUS_BIT_SC
    kflag r0
#endif
#endif
irq_handler_1:
    PUSH	blink

    jl 	OSIntEnter

    lr	r0, [AUX_IRQ_CAUSE]
    sr	r0, [AUX_IRQ_SELECT]
    mov	r1, exc_int_handler_table
    ld.as	r2, [r1, r0]	/* r2 = exc_int_handler_table + irqno *4 */
/* handle software triggered interrupt */
    lr	r3, [AUX_IRQ_HINT]
    cmp	r3, r0
    bne.d irq_hint_handled
    xor	r3, r3, r3
    sr	r3, [AUX_IRQ_HINT]
irq_hint_handled:
    lr	r3, [AUX_IRQ_PRIORITY]
    PUSH	r3		/* save irq priority */

    jl	[r2]		/* jump to interrupt handler */

    jl	OSIntExit

ret_int:
    clri			/* disable interrupt */
    POP	r3		/* irq priority */
    POP	sp
    mov	r1, g_exc_nest_count
    ld	r0, [r1]
    sub	r0, r0, 1
    st	r0, [r1]
/* if there are multi-bits set in IRQ_ACT, it's still in nest interrupt */
    lr	r0, [AUX_IRQ_CAUSE]
    sr	r0, [AUX_IRQ_SELECT]
    lr 	r3, [AUX_IRQ_PRIORITY]
    lr	r1, [AUX_IRQ_ACT]
    bclr	r2, r1, r3
    brne	r2, 0, ret_int_1

    ld	r0, [g_context_switch_reqflg]
    brne	r0, 0, ret_int_2
ret_int_1:	/* return from non-task context */
    INTERRUPT_EPILOGUE
    rtie
/* there is a dispatch request */
ret_int_2:
    /* clear dispatch request */
    mov	r0, 0
    st	r0, [g_context_switch_reqflg]

    /* interrupt return by SW */
    lr	r10, [AUX_IRQ_ACT]
    PUSH	r10
    bclr	r10, r10, r3	/* clear related bits in IRQ_ACT */
    sr	r10, [AUX_IRQ_ACT]

    SAVE_CALLEE_REGS	/* save callee save registers */
    mov	r1, ret_int_r	/* save return address */
    PUSH	r1

    ld 	r0, [OSTCBCur]
    ld 	r1, [OSTCBHighRdy]
    b	dispatcher

ret_int_r:
    RESTORE_CALLEE_REGS
    /* recover AUX_IRQ_ACT to restore the interrup status */
    POPAX	AUX_IRQ_ACT
    INTERRUPT_EPILOGUE
    rtie

/****** entry for fast irq exception handling ******/
    .global exc_entry_firq
    .align 4
exc_entry_firq:
    SAVE_FIQ_EXC_REGS

    jl 	OSIntEnter

    lr	r0, [AUX_IRQ_CAUSE]
    mov	r1, exc_int_handler_table
/* r2 = _kernel_exc_tbl + irqno *4 */
    ld.as	r2, [r1, r0]

/* for the case of software triggered interrupt */
    lr	r3, [AUX_IRQ_HINT]
    cmp	r3, r0
    bne.d	firq_hint_handled
    xor	r3, r3, r3
    sr	r3, [AUX_IRQ_HINT]
firq_hint_handled:
/* jump to interrupt handler */
    mov	r0, sp
    jl	[r2]

    jl 	OSIntExit

firq_return:
    RESTORE_FIQ_EXC_REGS
    rtie
