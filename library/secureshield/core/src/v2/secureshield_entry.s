/*------------------------------------------
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

#define MPU_DISABLE	0x400181c0  // MPU enabled, SID=1, S mode, KR, KW, KE
#define MPU_ENABLE	0

	.file "secureshield_entry.s"

/*************************************************************************************/
	.global sjli_secureshield_sys_ops
	.align 4
sjli_secureshield_sys_ops:
	push_s 	blink
	push 	fp    /* fp may be used, saved here */
#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
	push 	gp
	mov 	gp, _f_sdata
#endif
	jl 	secureshield_sys_ops
	CLEAR_SCRATCH_REGS
#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
/* when gp is not changed during execution and sdata is enabled, the following is meaningful */
/* The newlib c of ARC GNU is compiled with sdata enabled */
	pop 	gp
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
	push 	gp
	mov 	gp, _f_sdata
#endif
	jl 	secureshield_int_ops
	CLEAR_SCRATCH_REGS
#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
/* when gp is not changed during execution and sdata is enabled, the following is meaningful */
/* The newlib c of ARC GNU is compiled with sdata enabled */
	pop 	gp
#endif
	pop	fp
	pop_s 	blink
	j 	[blink]

/*************************************************************************************/
/* the entry of container_call_in */
	.text
	.global sjli_container_call_in
sjli_container_call_in:

	/* save 1st part of container context into the container's stack */
	EXCEPTION_PROLOGUE
	mov	r0, sp

#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
	mov 	gp, _f_sdata
#endif
	sr 	MPU_DISABLE, [AUX_MPU_EN]

	jl 	container_call_in

	cmp 	r0, 0
	/*
	 * r0 is the destination sp, !=0 a container switch required if no
	 * container switch caused by errors in container_call_in, directly
	 * return
	 */
	beq	_ret_directly
	/*
	 * save callee save registers, the 2nd part of container context into
	 * the container's stack
	 */
	SAVE_CALLEE_REGS

	/* load target container's context */
	mov	sp, r0
	RESTORE_CALLEE_REGS
	EXCEPTION_EPILOGUE

	sr 	MPU_ENABLE, [AUX_MPU_EN]

	/* return address it _ret_contianer_call_out */
	seti

	jl  [blink]

_ret_container_call_out:

	EXCEPTION_PROLOGUE
	SAVE_CALLEE_REGS

	clri

	sr 	MPU_DISABLE, [AUX_MPU_EN]

	mov	r0, sp

	jl	container_call_out

	mov	sp, r0
	RESTORE_CALLEE_REGS

_ret_directly:
	EXCEPTION_EPILOGUE

	sr 	MPU_ENABLE, [AUX_MPU_EN]

	/* leave secureshield runtime */
	j	[blink]

/*************************************************************************************/
/* the interrupt entry of secure interrupt */
	.global secureshield_exc_entry_int
	.align 4
secureshield_exc_entry_int:
	INTERRUPT_PROLOGUE

	mov 	r0, sp

	sr 	MPU_DISABLE, [AUX_MPU_EN]

	clri
	jl	secureshield_interrupt_handle
	cmp  	r0, 0
	/*
	 * r0 = 0, means:
	 * 1. error
	 * 2. interrupt belongs to the same container
 	 */
	bz 	_secure_int_return
	/*
	 *  the coming interrupt belongs to a different container.
	 *  it should be handled in the target container's context
	 */
_int_handle_ctx_switch:
	SAVE_CALLEE_REGS

	/* switch to target container's sp */
	mov 	sp, r0

	/* set correct stack pointers and status */
	/* clear regs to avoid information leakage */
	CLEAR_CALLEE_REGS
	CLEAR_SCRATCH_REGS

	POPAX	AUX_SEC_K_SP
	POPAX	AUX_KERNEL_SP
	POP 	blink
	POP 	r0

	sr 	MPU_ENABLE, [AUX_MPU_EN]

	seti
	jl 	[blink]
	clri

	sr 	MPU_DISABLE, [AUX_MPU_EN]
	mov 	r0, sp
	jl 	secureshield_interrupt_handle_return

	mov	sp, r0
	RESTORE_CALLEE_REGS

_secure_int_return:
	sr 	MPU_ENABLE, [AUX_MPU_EN]

	INTERRUPT_EPILOGUE

	rtie

/**********************************************************************/
	.text
	.global secureshield_exc_entry_cpu
	.align 4
secureshield_exc_entry_cpu:
	/*
	 * exception will be handled in secure container's stack and
	 * normal container's secure stack
	 */
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

	sr 	MPU_DISABLE, [AUX_MPU_EN]
	mov	r0, sp
	jl	[r2]		/* jump to exception handler where interrupts are not allowed! */
	sr 	MPU_ENABLE, [AUX_MPU_EN]

	EXCEPTION_EPILOGUE
	rtie