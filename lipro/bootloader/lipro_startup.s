#define __ASSEMBLY__
#include "arc.h"
        
.file "lipro_startup.s"

.weak	_f_sdata		/* start of small data, defined in link script */
.extern bl_main        
        
/* initial vector table */
	.section .init_vector, "a"
        .long _arc_reset
	.section .init_bootstrap, "ax"
	.global _arc_reset
	.global _start
	.align 4
_start:
_arc_reset:
_arc_reset_stage1:
	kflag	STATUS32_RESET_VALUE

/* STAGE 1 */

/* necessary hardware should be done first to speed up initialization
	1. system clk
	2. mem controller must be initialized before any access to external
	mem.
	3. others
*/
_arc_cache_init_start:
	lr	r0, [AUX_BCR_D_CACHE]
	cmp	r0, 2
	/* invalidate dcache */
	jle	_arc_icache_init
	mov	r0, 1
	sr	r0, [AUX_DC_IVDC]
	sr	r0, [AUX_DC_CTRL]
_arc_icache_init:
	lr	r0, [AUX_BCR_I_CACHE]
	cmp	r0, 2
	jle	_arc_cache_init_end
	/* invalidate icache */
	mov	r0, 1
	sr	r0, [AUX_IC_IVIC]
	nop_s
	nop_s
	nop_s
	sr	r0, [AUX_IC_CTRL]

_arc_cache_init_end:
/* STAGE 2: init necessary registers */

_arc_reset_stage2:
	mov	r0, 0

/* interrupt related init */
	sr	r0, [AUX_IRQ_ACT]
	sr	r0, [AUX_IRQ_CTRL]
	sr	r0, [AUX_IRQ_HINT]

/* sanity check - check code id */        
        lr      r0, [AUX_IDENTITY]       
        xbfu    r0, r0, 8, 8
        breq_s  r0, 0, _arc_reset_core0_only
        /* if not core 0, hal */
        flag    AUX_STATUS_MASK_HALT
_arc_reset_core0_only:  

/* init stack */
	mov	sp, _e_stack	/* init stack pointer */
	mov	gp, _f_sdata	/* init small-data base register */
	mov	fp, 0		/* init fp register */

        /* clear bss */
	mov	r0, _f_bss
	mov	r1, _e_bss
	cmp	r0, r1
	jge	_arc_reset_call_main
	mov	r2, 0
_s3_clear_bss_loop:
	st.ab	r2, [r0, 4]
	cmp	r0, r1
	jlt	_s3_clear_bss_loop

#if ARC_FEATURE_STACK_CHECK
	mov r0, _e_stack
	mov r1, _f_stack
#if ARC_FEATURE_SEC_PRESENT
	sr r1, [AUX_S_KSTACK_TOP]
	sr r0, [AUX_S_KSTACK_BASE]
	lr r0, [AUX_SEC_STAT]
	bset r0, r0, AUX_SEC_STAT_BIT_SSC
	sflag r0
#else
	sr r1, [AUX_KSTACK_TOP]
	sr r0, [AUX_KSTACK_BASE]
	lr r0, [AUX_STATUS32]
	bset r0, r0, AUX_STATUS_BIT_SC
	kflag r0
#endif
#endif
/* STAGE 3: go to main */

_arc_reset_call_main:

	jl	_init
        jl      bl_main
	jl	_fini
	.global _exit_loop
	.global _exit_halt
	.align 4
_exit_halt:
_exit_loop:
	flag	AUX_STATUS_MASK_HALT
	nop
	nop
	nop
	b	_exit_loop

	.global _init, _fini
	.section ".init",text
_init:
	.cfa_bf	_init
	push	%blink
	.cfa_push	{%blink}

	.section ".init$999999", text, 1, 2, check_text_align=0
	pop	%blink
	.cfa_pop	{%blink}
	j	[%blink]
	.cfa_ef

	.section ".fini", text
_fini:
	.cfa_bf	_fini
	push	%blink
	.cfa_push	{%blink}

	.section ".fini$999999", text, 1, 2, check_text_align=0
	pop	%blink
	.cfa_pop	{%blink}
	j	[%blink]
	.cfa_ef
/** @endcond */

/** }@*/
