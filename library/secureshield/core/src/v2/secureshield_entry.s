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
#define __ASSEMBLY__
#include "arc.h"
#include "arc_asm_common.h"
#include "secureshield_secure_call_exports.h"

#define MPU_DEFAULT_MODE	0x400181c0  // MPU enabled, SID=1, S mode, KR, KW, KE
#define MPU_REGION_SECURESHIELD_RUNTIME_RAM	1
#define SP_NOT_IN_RUMTIME		0
#define SP_IN_RUNTIME 			1

	.file "secureshield_entry.s"

.macro S_TO_N_CALL
#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
/* when gp is not changed during execution and sdata is enabled, the following is meaningful */
/* The newlib c of ARC GNU is compiled with sdata enabled */
	ld	gp, [normal_world_gp]	/* init small-data base register */
#endif
	sr	sp, [AUX_KERNEL_SP]	/*  AUX_KERNEL_SP is the target normal sp */
	lr	sp, [AUX_SEC_K_SP]	/*  load secure runtime sp */

	lr  	blink, [AUX_ERRET]
#if ARC_FEATURE_MPU_BUILD_S == 0 || SECURESHIELD_USE_MPU_SID == 0
	seti
#endif
	jl 	[blink]
	// mov 	blink, .L_ret_from_normal
	// push_s 	blink
	// lr 	blink, [AUX_SEC_STAT]
	// push_s 	blink
	// // from S to N,  SEC_STAT and blink will be pushed, so adjust runtime sp
	// //st 	sp, [secureshield_runtime_stack_ptr]
	// and 	blink, blink, 0x21 // keep NIC and SSC, ERM=0
	// sr 	blink, [AUX_ERSEC_STAT]
	// lr 	blink, [AUX_STATUS32]
	// bset 	blink, blink, 31  // enable interrupt
	// sr 	blink, [AUX_ERSTATUS]
	// mov 	blink, 1
	// kflag 	0x20
	// rtie
//.L_ret_from_normal:
#if ARC_FEATURE_MPU_BUILD_S == 0 || SECURESHIELD_USE_MPU_SID == 0
	clri
#endif
	sr 	sp, [AUX_SEC_K_SP]  // AUX_SEC_K_SP will be overwritten, recover it

#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
	mov 	gp, _f_sdata
#endif

.endm

.macro S_TO_S_CALL
	lr  	blink, [AUX_SEC_K_SP]
	push 	blink
	st 	blink, [secureshield_runtime_stack_ptr]

	lr	blink, [AUX_ERRET] /* get dst_fn from AUX_ERRET */
	/* \todo recover the right status32? */
	seti	// re-enable interrupt
	jl	[blink]	// jump to the target container in secure world
	clri	// disable interrupt


	pop 	blink
	st 	blink, [secureshield_runtime_stack_ptr] // recover original runtime_stack_ptr
	sr 	blink, [AUX_SEC_K_SP]
.endm


/*************************************************************************************/
	.global sjli_secureshield_sys_ops
	.align 4
sjli_secureshield_sys_ops:
	push_s 	blink
	push 	fp    /* fp may be used, saved here */
#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
	mov 	gp, _f_sdata
#endif
	jl 	secureshield_sys_ops
	CLEAR_SCRATCH_REGS
#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
/* when gp is not changed during execution and sdata is enabled, the following is meaningful */
/* The newlib c of ARC GNU is compiled with sdata enabled */
	ld	gp, [normal_world_gp]	/* init small-data base register */
#endif
	pop 	fp
	pop_s 	blink
	j 	[blink]

/*************************************************************************************/
	.global sjli_secureshield_int_ops
	.align 4
sjli_secureshield_int_ops:
	push_s 	blink
	push	fp    /* fp may be used, saved here */
#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
	mov 	gp, _f_sdata
#endif
	jl 	secureshield_int_ops
	CLEAR_SCRATCH_REGS
#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
/* when gp is not changed during execution and sdata is enabled, the following is meaningful */
/* The newlib c of ARC GNU is compiled with sdata enabled */
	ld	gp, [normal_world_gp]	/* init small-data base register */
#endif
	pop	fp
	pop_s 	blink
	j 	[blink]

/*************************************************************************************/
// the entry of container_call_in
// there is no  container_call_out
	.text
	.global sjli_container_call_in
sjli_container_call_in:
	sr  	blink, [AUX_ERRET]

	// find where sjli call comes from, normal world or secure world
	sr	blink, [AUX_MPU_PROBE]
	lr 	blink, [AUX_MPU_RPER]
	btst	blink, AUX_MPU_RPER_BIT_S
	bnz	_from_secure_world

	lr	sp, [AUX_KERNEL_SP]	// from normal world, load old sp from AUX_KERNEL_SP
_from_secure_world:
	sr 	MPU_DEFAULT_MODE, [AUX_MPU_EN]
	EXCEPTION_PROLOGUE		// save 1st part of container context into the container's stack
	mov	r0, sp

	bz 	_normal_use_aux_sec_k_sp
	// switch to runtime stack
	ld 	sp, [secureshield_runtime_stack_ptr]
	sr 	sp, [AUX_SEC_K_SP]
_normal_use_aux_sec_k_sp:
#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
	mov 	gp, _f_sdata
#endif
	lr 	sp, [AUX_SEC_K_SP]
	push_s	r0			// save old sp

	jl 	container_call_in	// call container_call_in in secureshield runtime' sp

	pop	sp			// recover old sp
	cmp 	r0, 0
	// r0 is the destination sp, !=0 a container switch required
	// bit0 = 1, go to secure world, bit0 = 0 go to normal world
	beq	_ret_directly		// if no container switch caused by errors in container_call_in, directly return
	btst	r0, 0
	bclr	r0, r0, 0	// clear bit 0

	// if container switch required
	SAVE_CALLEE_REGS	/* save callee save registers, the 2nd part of container context into the container's stack */
	CLEAR_CALLEE_REGS
	mov	sp, r0 		// r0 is the sp of destination container
	EXCEPTION_EPILOGUE	// pop the parameters

	sr 	0, [AUX_MPU_EN]		// leave secureshield runtime, disable default configuration

	bnz	_switch_to_secure_world //  z bit is the result of btst

_switch_to_normal_world:

	S_TO_N_CALL

	lr	r1, [AUX_KERNEL_SP]
	b 	_ret_container_call_out

_switch_to_secure_world:

	S_TO_S_CALL

	mov	r1, sp
	mov 	sp, blink
_ret_container_call_out:
	sr 	MPU_DEFAULT_MODE, [AUX_MPU_EN]
	lr	r2, [AUX_STATUS32]
	jl	container_call_out

	btst	r0, 0		// r0 is the sp of caller container, bit0 = 1 for secure container
				// bit0 = 0 for normal container
	bclr	r0, r0, 0	// clear bit 0,  z bit will not be updated

	mov	sp, r0
	RESTORE_CALLEE_REGS
	EXCEPTION_EPILOGUE

	sr 	0, [AUX_MPU_EN]		// leave secureshield runtime, disable default configuration

	bnz 	_ret_to_secure_world // z bit is the result of btst r0, 0
	sr	sp, [AUX_KERNEL_SP]
	lr	sp, [AUX_SEC_K_SP]
_ret_to_secure_world:
	lr	blink, [AUX_ERRET]
	j	[blink]

_ret_directly:
	EXCEPTION_PROLOGUE

	sr 	0, [AUX_MPU_EN]		// leave secureshield runtime, disable default configuration

	lr 	blink, [AUX_MPU_RPER]
	btst	blink, AUX_MPU_RPER_BIT_S
	bnz	_ret_directly_secure_world

	sr	sp, [AUX_KERNEL_SP]	/* sp is secure sp, AUX_KERNEL_SP is secure sp */
	lr	sp, [AUX_SEC_K_SP]

_ret_directly_secure_world:
	lr	blink, [AUX_ERRET]
	mov	r0, 0
	j	[blink]

/*************************************************************************************/
// the interrupt entry of secure interrupt
	.global secureshield_exc_entry_int
	.align 4
secureshield_exc_entry_int:
	clri	/* disable interrupt */

	sr 	MPU_DEFAULT_MODE, [AUX_MPU_EN]
	/* check whether secure interrupt happens in runtime (between seti and clri) */
	mov 	r1, SP_NOT_IN_RUMTIME
	sr 	sp, [AUX_MPU_PROBE]
	lr 	r0, [AUX_MPU_INDEX]
	cmp 	r0, MPU_REGION_SECURESHIELD_RUNTIME_RAM
	mov.z 	r1, SP_IN_RUNTIME

	lr 	r0, [AUX_SEC_STAT]
	btst 	r0, AUX_SEC_STAT_BIT_IRM
	bnz	_int_from_secure_world
	// secure interrupt comes from normal world
	// PUSH_MISC may push JLI, LDI, EI, LP_XXX into SEC_K_SP
	// we should remember it
	// AUX_KERNEL_SP has the sp of interrupted normal world
	// no consideration for interrupts happed in user mode
	mov 	r2, sp // sp could not be runtime stack, case: S->N_INT->S_INT

	lr	sp, [AUX_KERNEL_SP]
#ifdef ARC_FEATURE_CODE_DENSITY
	sub 	sp, sp, 24 	// keep spaces for ei, ldi, jli in normal world, not in stack
#else
	sub 	sp, sp, 12 	// keep spaces for lp_end, lp_start, lp_count in normal world, not in stack
#endif
	INTERRUPT_PROLOGUE
	mov 	r0, sp

	cmp 	r1, SP_IN_RUNTIME
	mov 	r1, 0  // r1 no uses
	mov.z 	sp, r2
	bz 	_int_from_normal_world
	ld 	sp, [secureshield_runtime_stack_ptr] // load the right runtime stack
_int_from_normal_world:
	push 	r2 // need to remember
	b 	_secure_int_handler_1 // sp is secureshield runtime stack ptr

_int_from_secure_world:
	INTERRUPT_PROLOGUE
	// from secure container, AUX_KERNEL_SP is background container's sp
	// from  runtime, AUX_KERNEL_SP could be any normal container's sp
	// it may be overwritten to background container's sp for normal interrupt handling
	// so save it and restore when return
	PUSHAX 	AUX_KERNEL_SP
	mov	r0, sp

	cmp 	r1, SP_IN_RUNTIME
	bz 	_secure_int_handler_1 	// secure interrupts(N) happened in runtime, no need to adjust sp
	ld 	sp, [secureshield_runtime_stack_ptr] // secure interrupts(S)

_secure_int_handler_1:
	sr 	sp, [AUX_SEC_K_SP] 	// update AUX_SEC_K_SP with correct runtime sp

#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
	mov 	gp, _f_sdata
#endif
	push	r0

	jl	secureshield_interrupt_handle

	pop 	sp
	cmp  	r0, 0
	bz 	_secure_int_return //  r0 is 0 means  the secure interrupt belong to secure world or error
	btst	r0, 0		// bit 0 = 0 is no context switch, = 1 is context switch
	bclr	r0, r0, 0	// clear bit 0,  z bit will not be updated
	bz 	_int_handle_no_ctx_switch  // z is the result of btst
_int_handle_ctx_switch:
	SAVE_CALLEE_REGS
	mov 	sp, r0		// switch to target container's sp

	CLEAR_CALLEE_REGS	// case for container switch, clear callee regs
	CLEAR_SCRATCH_REGS	// clear scratch regs
_int_handle_no_ctx_switch:
	lr 	blink, [AUX_ERRET]
	sr	blink, [AUX_MPU_PROBE]
	lr 	blink, [AUX_MPU_RPER]
	btst	blink, AUX_MPU_RPER_BIT_S

	sr 	0, [AUX_MPU_EN]	// leave secureshield runtime, disable default configuration */

	bnz	_secure_int_handle

_normal_int_handle:

	S_TO_N_CALL

	b 	handle_int_return
_secure_int_handle:

	S_TO_S_CALL

	mov 	sp, blink

handle_int_return:
	lr 	r0, [AUX_STATUS32]
	jl 	secureshield_interrupt_handle_return

	btst	r0, 0
	// clear bit 0,  z bit will not be updated, bit0 = 0 is no context switch
	// bit = 1,  context switch
	bclr	r0, r0, 0
	mov	sp, r0
	bz 	_secure_int_return
	RESTORE_CALLEE_REGS

_secure_int_return:
	sr 	0,  [AUX_MPU_EN]

	lr 	r0, [AUX_SEC_STAT]
	btst 	r0, AUX_SEC_STAT_BIT_IRM
	bnz	_int_return_to_secure_world

_int_return_to_normal_world:
	INTERRUPT_EPILOGUE	// pop from interrupted normal container's stack
#ifdef ARC_FEATURE_CODE_DENSITY
	add 	sp, sp, 24 	// skip spaces for ei, ldi, jli, lp_end, lp_start, lp_count
#else
	add 	sp, sp, 12
#endif 
	sr 	sp, [AUX_KERNEL_SP]
	lr 	sp, [AUX_SEC_K_SP]
	pop 	sp
	rtie

_int_return_to_secure_world:
	POPAX	AUX_KERNEL_SP
	INTERRUPT_EPILOGUE
	rtie

/**********************************************************************/
	.text
	.global secureshield_exc_entry_cpu
	.align 4
secureshield_exc_entry_cpu:
	sr 	MPU_DEFAULT_MODE, [AUX_MPU_EN]
	// if exception happens in normal world, SEC_K_SP is used as the stack for
	// EXCEPTION_PROLOGUE and EXCEPTION_EPILOGUE.
	// if exception happens in secure world, in kernel mode, the original sp
	// is used as the stack for EXCEPTION_PROLOGUE and EXCEPTION_EPILOGUE,
	// in user mode, SEC_K_SP is used as the stack for EXCEPTION_PROLOGUE and EXCEPTION_EPILOGUE
	EXCEPTION_PROLOGUE

	/* find the exception cause */
	lr	r0, [AUX_ECR]
	lsr	r0, r0, 16
	bmsk	r0, r0, 7
	mov	r1, secureshield_exc_handler_table
	ld.as	r2, [r1, r0]

#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
	mov 	gp, _f_sdata
#endif

	mov	r0, blink
	jl	[r2]		/* jump to exception handler where interrupts are not allowed! */

	EXCEPTION_EPILOGUE
	sr 	0, [AUX_MPU_EN]
	rtie
/** @endcond */