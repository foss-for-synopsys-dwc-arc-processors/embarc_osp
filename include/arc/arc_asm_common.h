/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */

/**
 * \file
 * \ingroup ARC_HAL_MISC
 * \brief common macro definitions for assembly file
 */
/** @cond ARC_HAL_ASM_COMMON */

#ifndef _ARC_HAL_ASM_COMMON_H_
#define _ARC_HAL_ASM_COMMON_H_

#include "arc/arc.h"

#ifdef __ASSEMBLY__
/* the assembly macro definitions in ARC GNU and MWDT are
 * different, so need different processing
 */
#if defined(__GNU__)
#define MACRO_ARG(x) \x
#define ASM_MACRO1(name, arg1) name arg1
#define ASM_MACRO2(name, arg1, arg2) name arg1 arg2
#define ASM_MACRO3(name, arg1, arg2, arg3) name arg1 arg, agr3
#define ASM_MACRO4(name, arg1, arg2, arg3, arg4) name arg1 arg2 arg3 arg4
#else
#define MACRO_ARG(x) x
#define ASM_MACRO1(name, arg1) name, arg1
#define ASM_MACRO2(name, arg1, arg2) name, arg1, arg2
#define ASM_MACRO3(name, arg1, arg2, arg3) name, arg1, arg2, agr3
#define ASM_MACRO4(name, arg1, arg2, arg3, arg4) name, arg1, arg2, arg3, arg4
#endif

/* Note on the LD/ST addr modes with addr reg wback
 *
 * LD.a same as LD.aw
 *
 * LD.a    reg1, [reg2, x]  => Pre Incr
 *      Eff Addr for load = [reg2 + x]
 *
 * LD.ab   reg1, [reg2, x]  => Post Incr
 *      Eff Addr for load = [reg2]
 */
.macro ASM_MACRO1(PUSH, reg)
	st.a MACRO_ARG(reg), [sp, -4]
.endm

.macro ASM_MACRO1(PUSHAX, aux)
	lr r10, [MACRO_ARG(aux)]
	PUSH r10
.endm

.macro ASM_MACRO1(POP, reg)
	ld.ab MACRO_ARG(reg), [sp, 4]
.endm

.macro ASM_MACRO1(POPAX, aux)
	POP r10
	sr r10, [MACRO_ARG(aux)]
.endm

/* macro to get the cpu id */
.macro 	ASM_MACRO1(GET_CORE_ID, reg)
	lr MACRO_ARG(reg), [AUX_IDENTITY]
	xbfu MACRO_ARG(reg), MACRO_ARG(reg), 0xe8
.endm

/* macro to save accl regs */
.macro SAVE_R58_R59
#if ARC_FEATURE_FPU || ARC_FEATURE_DSP || ARC_FEATURE_MPU_OPTION_NUM > 6
	PUSH r58
	PUSH r59
#endif
.endm

/* macro to restore accl regs */
.macro RESTORE_R58_R59
#if ARC_FEATURE_FPU || ARC_FEATURE_DSP || ARC_FEATURE_MPU_OPTION_NUM > 6
	POP r59
	POP r58
#endif
.endm

/* macro to save fpu related regs */
.macro SAVE_FPU_REGS
#if ARC_FEATURE_FPU
	PUSHAX AUX_FPU_CTRL
	PUSHAX AUX_FPU_STATUS

#if ARC_FEATURE_FPU_DA
	PUSHAX AUX_FPU_DPFP1L
	PUSHAX AUX_FPU_DPFP1H
	PUSHAX AUX_FPU_DPFP2L
	PUSHAX AUX_FPU_DPFP2H
#endif

#endif
.endm

/* macro to restore fpu related regs */
.macro RESTORE_FPU_REGS
#if ARC_FEATURE_FPU

#if ARC_FEATURE_FPU_DA
	POPAX AUX_FPU_DPFP2H
	POPAX AUX_FPU_DPFP2L
	POPAX AUX_FPU_DPFP1H
	POPAX AUX_FPU_DPFP1L
#endif
	POPAX AUX_FPU_STATUS
	POPAX AUX_FPU_CTRL
#endif

.endm

/* macro to save dsp related regs */
.macro SAVE_DSP_REGS
#if ARC_FEATURE_DSP
	PUSHAX AUX_DSP_CTRL
	PUSHAX AUX_ACC0_LO
	PUSHAX AUX_ACC0_GLO
	PUSHAX AUX_ACC0_HI
	PUSHAX AUX_ACC0_GHI

#if ARC_FEATURE_DSP_COMPLEX
	PUSHAX AUX_DSP_BFLY0
	PUSHAX AUX_DSP_FFT_CTRL
#endif

#endif
.endm

/* macro to restore dsp related regs */
.macro RESTORE_DSP_REGS
#if ARC_FEATURE_DSP

#if ARC_FEATURE_DSP_COMPLEX
	POPAX AUX_DSP_FFT_CTRL
	POPAX AUX_DSP_BFLY0
#endif
	POPAX AUX_ACC0_GHI
	POPAX AUX_ACC0_HI
	POPAX AUX_ACC0_GLO
	POPAX AUX_ACC0_LO
	POPAX AUX_DSP_CTRL
#endif
.endm

/* macro to save agu related regs */
.macro SAVE_AGU_REGS
#if ARC_FEATURE_AGU
// ARC_FEATURE_AGU_SMALL || ARC_FEATURE_AGU_MEDIUM || ARC_FEATURE_AGU_LARGE
	PUSHAX AUX_AGU_AP0
	PUSHAX AUX_AGU_AP1
	PUSHAX AUX_AGU_AP2
	PUSHAX AUX_AGU_AP3
	PUSHAX AUX_AGU_OS0
	PUSHAX AUX_AGU_OS1
	PUSHAX AUX_AGU_MOD0
	PUSHAX AUX_AGU_MOD1
	PUSHAX AUX_AGU_MOD2
	PUSHAX AUX_AGU_MOD3
#if ARC_FEATURE_AGU_MEDIUM || ARC_FEATURE_AGU_LARGE
	PUSHAX AUX_AGU_AP4
	PUSHAX AUX_AGU_AP5
	PUSHAX AUX_AGU_AP6
	PUSHAX AUX_AGU_AP7
	PUSHAX AUX_AGU_OS2
	PUSHAX AUX_AGU_OS3
	PUSHAX AUX_AGU_MOD4
	PUSHAX AUX_AGU_MOD5
	PUSHAX AUX_AGU_MOD6
	PUSHAX AUX_AGU_MOD7
	PUSHAX AUX_AGU_MOD8
	PUSHAX AUX_AGU_MOD9
	PUSHAX AUX_AGU_MOD10
	PUSHAX AUX_AGU_MOD11
#endif
#if ARC_FEATURE_AGU_LARGE
	PUSHAX AUX_AGU_AP8
	PUSHAX AUX_AGU_AP9
	PUSHAX AUX_AGU_AP10
	PUSHAX AUX_AGU_AP11
	PUSHAX AUX_AGU_OS4
	PUSHAX AUX_AGU_OS5
	PUSHAX AUX_AGU_OS6
	PUSHAX AUX_AGU_OS7
	PUSHAX AUX_AGU_MOD12
	PUSHAX AUX_AGU_MOD13
	PUSHAX AUX_AGU_MOD14
	PUSHAX AUX_AGU_MOD15
	PUSHAX AUX_AGU_MOD16
	PUSHAX AUX_AGU_MOD17
	PUSHAX AUX_AGU_MOD18
	PUSHAX AUX_AGU_MOD19
	PUSHAX AUX_AGU_MOD20
	PUSHAX AUX_AGU_MOD21
	PUSHAX AUX_AGU_MOD22
	PUSHAX AUX_AGU_MOD23
#endif

#endif
.endm

/* macro to restore dsp related regs */
.macro RESTORE_AGU_REGS
#if ARC_FEATURE_AGU
#if ARC_FEATURE_AGU_LARGE
	POPAX AGU_AUX_MOD23
	POPAX AGU_AUX_MOD22
	POPAX AGU_AUX_MOD21
	POPAX AGU_AUX_MOD20
	POPAX AGU_AUX_MOD19
	POPAX AGU_AUX_MOD18
	POPAX AGU_AUX_MOD17
	POPAX AGU_AUX_MOD16
	POPAX AGU_AUX_MOD15
	POPAX AGU_AUX_MOD14
	POPAX AGU_AUX_MOD13
	POPAX AGU_AUX_MOD12
	POPAX AUX_AGU_OS7
	POPAX AUX_AGU_OS6
	POPAX AUX_AGU_OS5
	POPAX AUX_AGU_OS4
	POPAX AUX_AGU_AP11
	POPAX AUX_AGU_AP10
	POPAX AUX_AGU_AP9
	POPAX AUX_AGU_AP8
#endif
#if ARC_FEATURE_AGU_MEDIUM || ARC_FEATURE_AGU_LARGE
	POPAX AGU_AUX_MOD11
	POPAX AGU_AUX_MOD10
	POPAX AGU_AUX_MOD9
	POPAX AGU_AUX_MOD8
	POPAX AGU_AUX_MOD7
	POPAX AGU_AUX_MOD6
	POPAX AGU_AUX_MOD5
	POPAX AGU_AUX_MOD4
	POPAX AUX_AGU_OS3
	POPAX AUX_AGU_OS2
	POPAX AUX_AGU_AP7
	POPAX AUX_AGU_AP6
	POPAX AUX_AGU_AP5
	POPAX AUX_AGU_AP4
#endif
// ARC_FEATURE_AGU_SMALL || ARC_FEATURE_AGU_MEDIUM || ARC_FEATURE_AGU_LARGE
	POPAX AUX_AGU_MOD3
	POPAX AUX_AGU_MOD2
	POPAX AUX_AGU_MOD1
	POPAX AUX_AGU_MOD0
	POPAX AUX_AGU_OS1
	POPAX AUX_AGU_OS0
	POPAX AUX_AGU_AP3
	POPAX AUX_AGU_AP2
	POPAX AUX_AGU_AP1
	POPAX AUX_AGU_AP0
#endif
.endm

/*--------------------------------------------------------------
 * Helpers to save/restore callee-saved regs:
 * used by several macros below
 *-------------------------------------------------------------*/
.macro SAVE_CALLEE_REGS
	PUSH	r13
	PUSH	r14
	PUSH	r15
#ifndef ARC_FEATURE_RF16
	PUSH	r16
	PUSH	r17
	PUSH	r18
	PUSH	r19
	PUSH	r20
	PUSH	r21
	PUSH	r22
	PUSH	r23
	PUSH	r24
	PUSH	r25
#endif

#if defined(ARC_ENABLE_EXTRA_CALLEE)
#if ARC_FEATURE_SEC_PRESENT
	PUSHAX AUX_KERNEL_SP
	PUSHAX AUX_SEC_K_SP
#else
	PUSHAX AUX_USER_SP
#endif
#endif

#if ARC_FEATURE_FPU_DSP_CONTEXT
	SAVE_FPU_REGS
	SAVE_DSP_REGS
	SAVE_AGU_REGS
#endif

.endm

.macro RESTORE_CALLEE_REGS

#if ARC_FEATURE_FPU_DSP_CONTEXT
	RESTORE_AGU_REGS
	RESTORE_DSP_REGS
	RESTORE_FPU_REGS
#endif

#if defined(ARC_ENABLE_EXTRA_CALLEE)
#if ARC_FEATURE_SEC_PRESENT
	POPAX AUX_SEC_K_SP
	POPAX AUX_KERNEL_SP
#else
	POPAX AUX_USER_SP
#endif
#endif

#ifndef ARC_FEATURE_RF16
	POP	r25
	POP	r24
	POP	r23
	POP	r22
	POP	r21
	POP	r20
	POP	r19
	POP	r18
	POP	r17
	POP	r16
#endif
	POP	r15
	POP	r14
	POP	r13
.endm

/* macro to clear callee regs */
.macro CLEAR_CALLEE_REGS
#ifndef ARC_FEATURE_RF16
	mov	r25, 0
	mov	r24, 0
	mov	r23, 0
	mov	r22, 0
	mov	r21, 0
	mov	r20, 0
	mov	r19, 0
	mov	r18, 0
	mov	r17, 0
	mov	r16, 0
#endif
	mov	r15, 0
	mov	r14, 0
	mov	r13, 0
#if ARC_FEATURE_FPU_DSP_CONTEXT

#if ARC_FEATURE_FPU

#if ARC_FEATURE_FPU_DA
	sr 	0, [AUX_FPU_DPFP2H]
	sr 	0, [AUX_FPU_DPFP2L]
	sr 	0, [AUX_FPU_DPFP1H]
	sr 	0, [AUX_FPU_DPFP1L]
#endif
	sr 	0, [AUX_FPU_STATUS]
	sr 	0, [AUX_FPU_CTRL]
#endif /* ARC_FEATURE_FPU */

#if ARC_FEATURE_DSP

#if ARC_FEATURE_DSP_COMPLEX
	sr 	0, [AUX_DSP_FFT_CTRL]
	sr 	0, [AUX_DSP_BFLY0]
#endif
	sr 	0, [AUX_ACC0_GHI]
	sr 	0, [AUX_ACC0_HI]
	sr 	0, [AUX_ACC0_GLO]
	sr 	0, [AUX_ACC0_LO]
	sr 	0, [AUX_DSP_CTRL]
#endif /* ARC_FEATURE_DSP */

#endif /* ARC_FEATURE_FPU_DSP_CONTEXT */
.endm

/* macro to clear scratch regs */
.macro CLEAR_SCRATCH_REGS
	mov	r1, 0
	mov 	r2, 0
	mov	r3, 0
	mov	r4, 0
	mov	r5, 0
	mov	r6, 0
	mov	r7, 0
	mov	r8, 0
	mov 	r9, 0
	mov	r10, 0
	mov	r11, 0
	mov	r12, 0

#if ARC_FEATURE_FPU || ARC_FEATURE_DSP || ARC_FEATURE_MPU_OPTION_NUM > 6
	mov 	r59, 0
	mov	r58, 0
#endif
	mov 	fp, 0
	mov 	r29, 0
	mov 	r30, 0
.endm

.macro SAVE_LP_REGS
	PUSH	r60
	PUSHAX	AUX_LP_START
	PUSHAX	AUX_LP_END
.endm

.macro RESTORE_LP_REGS
	POPAX	AUX_LP_END
	POPAX	AUX_LP_START
	POP	r10
/* must not use the LP_COUNT register(r60) as the destination of multi-cycle instruction */
	mov	r60, r10

.endm

/* macro to save r0 to r12 */
.macro SAVE_R0_TO_R12
	PUSH    r0
	PUSH    r1
	PUSH    r2
	PUSH    r3
#ifndef ARC_FEATURE_RF16
 	PUSH    r4
 	PUSH    r5
 	PUSH    r6
 	PUSH    r7
 	PUSH    r8
 	PUSH    r9
#endif
	PUSH    r10
	PUSH    r11
	PUSH    r12
.endm

/* macro to restore r0 to r12 */
.macro RESTORE_R0_TO_R12
	POP	r12
	POP	r11
	POP	r10
#ifndef ARC_FEATURE_RF16
	POP	r9
	POP	r8
	POP	r7
	POP	r6
	POP	r5
	POP	r4
#endif
	POP	r3
	POP	r2
	POP	r1
	POP	r0
.endm

/* macro to save code density regs */
.macro SAVE_CODE_DENSITY
	PUSHAX	AUX_JLI_BASE
	PUSHAX	AUX_LDI_BASE
	PUSHAX	AUX_EI_BASE
.endm

/* macro to restore code density regs */
.macro RESTORE_CODE_DENSITY
	POPAX	AUX_EI_BASE
	POPAX	AUX_LDI_BASE
	POPAX	AUX_JLI_BASE
.endm

/* macro to save all non-caller saved regs */
.macro SAVE_NONSCRATCH_REGS
/* caller saved regs are saved by caller function */
	PUSH	gp
	PUSH	fp
	PUSH	blink
	SAVE_CALLEE_REGS
.endm

/* macro to restore all non-caller saved regs */
.macro RESTORE_NONSCRATCH_REGS
	RESTORE_CALLEE_REGS
	POP	blink
	POP	fp
	POP	gp
.endm

/* macro to save regs  in firq */
.macro SAVE_FIQ_EXC_REGS
#ifndef ARC_FEATURE_RGF_BANKED_REGS
	SAVE_R0_TO_R12

	SAVE_R58_R59

	PUSH	gp
	PUSH	fp
	PUSH	r30		/* general purpose */
	PUSH	blink

#else
#if ARC_FEATURE_RGF_BANKED_REGS != 4 && ARC_FEATURE_RGF_BANKED_REGS != 8 && \
	ARC_FEATURE_RGF_BANKED_REGS != 16  && ARC_FEATURE_RGF_BANKED_REGS != 32
#error	"unsupported ARC_FEATURE_RGF_BANKED_REGS"
#endif

#if ARC_FEATURE_RGF_BANKED_REGS == 4 || ARC_FEATURE_RGF_BANKED_REGS == 8 || \
	ARC_FEATURE_RGF_BANKED_REGS == 16
#ifndef ARC_FEATURE_RF16
	PUSH	r4
	PUSH	r5
	PUSH	r6
	PUSH	r7
	PUSH	r8
	PUSH	r9
#endif
#endif

#if ARC_FEATURE_RGF_BANKED_REGS == 4 || ARC_FEATURE_RGF_BANKED_REGS == 8
	PUSH	r10
	PUSH	r11
#endif

#if ARC_FEATURE_RGF_BANKED_REGS == 4
	PUSH	r12
#endif

	SAVE_R58_R59

#if ARC_FEATURE_RGF_BANKED_REGS == 4 || ARC_FEATURE_RGF_BANKED_REGS == 8
	PUSH	gp
	PUSH	fp
	PUSH	r30		/* general purpose */
	PUSH	blink
#endif

#endif /* #ifndef ARC_FEATURE_RGF_BANKED_REGS */

#if ARC_FEATURE_CODE_DENSITY
	SAVE_CODE_DENSITY
#endif
	SAVE_LP_REGS
.endm

/* macro  to restore reg in firq */
.macro RESTORE_FIQ_EXC_REGS
	RESTORE_LP_REGS
#if ARC_FEATURE_CODE_DENSITY
	RESTORE_CODE_DENSITY
#endif

#ifndef ARC_FEATURE_RGF_BANKED_REGS
	POP	blink
	POP	r30
	POP	fp
	POP	gp

	RESTORE_R58_R59

	RESTORE_R0_TO_R12
#else

#if ARC_FEATURE_RGF_BANKED_REGS != 4 && ARC_FEATURE_RGF_BANKED_REGS != 8 && \
	ARC_FEATURE_RGF_BANKED_REGS != 16 && ARC_FEATURE_RGF_BANKED_REGS != 32
#error	"unsupported ARC_FEATURE_RGF_BANKED_REGS"
#endif

#if ARC_FEATURE_RGF_BANKED_REGS == 4 || ARC_FEATURE_RGF_BANKED_REGS == 8
	POP	blink
	POP	r30
	POP	fp
	POP	gp
#endif

	RESTORE_R58_R59

#if ARC_FEATURE_RGF_BANKED_REGS == 4
	POP	r12
#endif

#if ARC_FEATURE_RGF_BANKED_REGS == 4 || ARC_FEATURE_RGF_BANKED_REGS == 8
	POP		r11
	POP		r10
#endif

#if ARC_FEATURE_RGF_BANKED_REGS == 4 || ARC_FEATURE_RGF_BANKED_REGS == 8 || \
	ARC_FEATURE_RGF_BANKED_REGS == 16
#ifndef ARC_FEATURE_RF16
	POP		r9
	POP		r8
	POP		r7
	POP		r6
	POP		r5
	POP		r4
#endif
#endif

#endif  /* #ifndef ARC_FEATURE_RGF_BANKED_REGS */
.endm

/* normal interrupt prologue, pc, status and r0-r11 are saved by hardware */
.macro INTERRUPT_PROLOGUE
	PUSH	r12
	SAVE_R58_R59
	PUSH	gp
	PUSH	fp
	PUSH	ilink
	PUSH	r30

	sub	sp, sp, 4 /* skip bta */
.endm


/* normal interrupt epilogue, pc, status and r0-r11 are restored by hardware */
.macro INTERRUPT_EPILOGUE
	add	sp, sp, 4 /* skip bta */

	POP	r30
	POP	ilink
	POP	fp
	POP	gp
	RESTORE_R58_R59
	POP	r12
.endm

#if ARC_FEATURE_SEC_PRESENT
/* exception prologue, create the same frame of interrupt manually */
.macro EXCEPTION_PROLOGUE
	st.as	r10, [sp, -6]	/* save r10 first, free up a register*/

	PUSHAX	AUX_ERSTATUS
	sub 	sp, sp, 4 /* slot for SEC_STAT */
	PUSHAX	AUX_ERRET

	PUSH	blink

	PUSH	r11
	sub	sp, sp, 4 /* r10 is  pushed before */
#ifndef ARC_FEATURE_RF16
	PUSH	r9
	PUSH	r8
	PUSH	r7
	PUSH	r6
	PUSH	r5
	PUSH	r4
#endif
	PUSH	r3
	PUSH	r2
	PUSH	r1
	PUSH	r0

#if ARC_FEATURE_CODE_DENSITY
	SAVE_CODE_DENSITY
#endif
	SAVE_LP_REGS

	PUSH	r12
	SAVE_R58_R59
	PUSH	gp
	PUSH	fp
	PUSH	ilink
	PUSH	r30

	PUSHAX	AUX_ERBTA
.endm

/* exception epilogue, restore the same frame of interrupt manually */
.macro EXCEPTION_EPILOGUE
	POPAX	AUX_ERBTA

	POP	r30
	POP	ilink
	POP	fp
	POP	gp
	RESTORE_R58_R59
	POP	r12

	RESTORE_LP_REGS

#if ARC_FEATURE_CODE_DENSITY
	RESTORE_CODE_DENSITY
#endif
	POP	r0
	POP	r1
	POP	r2
	POP	r3
#ifndef ARC_FEATURE_RF16
	POP	r4
	POP	r5
	POP	r6
	POP	r7
	POP	r8
	POP	r9
#endif
	add	sp, sp, 4 /* r10 will be popped finally */
	POP	r11

	POP	blink

	POPAX	AUX_ERRET
	add 	sp, sp, 4  /* slot for SEC_STAT */
	POPAX	AUX_ERSTATUS

	ld.as	r10, [sp, -6]	/* restore r10 */
.endm
#else /* normal version */
/* exception prologue, create the same frame of interrupt manually */
.macro EXCEPTION_PROLOGUE
#if ARC_FEATURE_CODE_DENSITY
	st.as	r10, [sp, -11]	/* save r10 first, free up a register*/
#else
	st.as	r10, [sp, -8]
#endif
	PUSHAX	AUX_ERSTATUS
	PUSHAX	AUX_ERRET

#if ARC_FEATURE_CODE_DENSITY
	SAVE_CODE_DENSITY
#endif
	SAVE_LP_REGS

	PUSH	blink

	PUSH	r11
	sub	sp, sp, 4 /* r10 is  pushed before */
#ifndef ARC_FEATURE_RF16
	PUSH	r9
	PUSH	r8
	PUSH	r7
	PUSH	r6
	PUSH	r5
	PUSH	r4
#endif
	PUSH	r3
	PUSH	r2
	PUSH	r1
	PUSH	r0

	PUSH	r12
	SAVE_R58_R59
	PUSH	gp
	PUSH	fp
	PUSH	ilink
	PUSH	r30

	PUSHAX	AUX_ERBTA
.endm

/* exception epilogue, restore the same frame of interrupt manually */
.macro EXCEPTION_EPILOGUE
	POPAX	AUX_ERBTA

	POP	r30
	POP	ilink
	POP	fp
	POP	gp
	RESTORE_R58_R59
	POP	r12

	POP	r0
	POP	r1
	POP	r2
	POP	r3
#ifndef ARC_FEATURE_RF16
	POP	r4
	POP	r5
	POP	r6
	POP	r7
	POP	r8
	POP	r9
#endif
	add	sp, sp, 4 /* r10 will be popped finally */
	POP	r11

	POP	blink

	RESTORE_LP_REGS

#if ARC_FEATURE_CODE_DENSITY
	RESTORE_CODE_DENSITY
#endif

	POPAX	AUX_ERRET
	POPAX	AUX_ERSTATUS

#if ARC_FEATURE_CODE_DENSITY
	ld.as	r10, [sp, -11]	/* restore r10 */
#else
	ld.as	r10, [sp, -8]
#endif
.endm

#endif /* ARC_FEATURE_SEC_PRESENT */

#endif  /* __ASSEMBLY__ */
#endif	/* _ARC_HAL_ASM_COMMON_H */
/** @endcond */
