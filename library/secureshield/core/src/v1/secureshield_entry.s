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

#define MPU_DISABLE 	0x0
#define MPU_ENABLE	0x40000000

	.file "secureshield_entry.s"
/* entry for secureshield cpu exception handling */
	.text
	.global secureshield_exc_entry_cpu
	.align 4
secureshield_exc_entry_cpu:
	EXCEPTION_PROLOGUE

	/* disable MPU, runtime can do everything */
	sr 	MPU_DISABLE, [AUX_MPU_EN]

#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
	mov 	gp, _f_sdata
#endif

/* find the exception cause */
	lr	r0, [AUX_ECR]
	lsr	r0, r0, 16
	bmsk	r0, r0, 7
	mov	r1, secureshield_exc_handler_table
	ld.as	r2, [r1, r0]

/* jump to exception handler where interrupts are not allowed! */
	mov	r0, sp
	jl	[r2]

	cmp	r0, 0
	beq	_ret_exc

	/* there is a context switch request */
	SAVE_CALLEE_REGS
	mov	sp, r0
	RESTORE_CALLEE_REGS

_ret_exc:
	EXCEPTION_EPILOGUE

	/* leave runtime, re-enable the mpu */
	sr 	MPU_ENABLE, [AUX_MPU_EN]
	rtie

/*************************************************************************************/

	.global secureshield_exc_entry_int
	.align 4
secureshield_exc_entry_int:
#if ARC_FEATURE_FIRQ == 1
	/* the ret is saved or in ERET, so ilink can be used */
	lr	ilink, [AUX_IRQ_ACT]
	btst	ilink, 0
	bz	.L_normal_irq
	lr	ilink, [AUX_STATUS32_P0]
	sr	ilink, [AUX_ERSTATUS]
#if ARC_FEATURE_RGF_NUM_BANKS > 1
	btst 	ilink, AUX_STATUS_BIT_U
	bz 	_firq_from_secure
	sr 	sp, [AUX_USER_SP]
/* sp is also banked when there are more than 16 banked regs.*/
	kflag	0x1000	/* switch bank0, loop enable, get the original sp */
	aex 	sp, [AUX_USER_SP]
	b 	_firq_hanlde
_firq_from_secure:
	kflag 	0x1000
_firq_hanlde:
#endif
	lr 	ilink, [AUX_ERRET]
	EXCEPTION_PROLOGUE
	b .L_common_for_irq_and_firq
#endif
.L_normal_irq:
	INTERRUPT_PROLOGUE

.L_common_for_irq_and_firq:

	clri	/* disable interrupt */
	/* disable MPU, runtime can do everything */
	sr 	MPU_DISABLE, [AUX_MPU_EN]

	mov	r0, sp
	jl	secureshield_interrupt_handle

	/*
	 * if r0 is not 0, it means:
	 *  1: interrupt belongs to different secure container
	 *  2: interrupt belongs to normal container
	 */
	cmp	r0, 0
	beq	_ret_int_directly

	/* take interrupt as exception, return as an exception */
	kflag	AUX_STATUS_MASK_AE

	SAVE_CALLEE_REGS

	ld 	r1, [dst_container_user_sp]
	sr 	r1, [AUX_USER_SP]

	CLEAR_CALLEE_REGS
	CLEAR_SCRATCH_REGS

	mov	sp, r0

	/*
	 * AUX_ERET, AUX_ERSTATUS, AUX_ERBTA are set in
	 * secureshield_interrupt_handle
	 */
	/* set blink to interrupt return */
	mov	blink, secureshield_secure_call_int_out

	sr 	MPU_ENABLE, [AUX_MPU_EN]
	rtie	/* rtie will do an exception return */

_ret_int_directly:
#if ARC_FEATURE_FIRQ == 1
	lr	r0, [AUX_IRQ_ACT]
	btst	r0, 0
	bz 	_ret_int_rirq
	EXCEPTION_EPILOGUE
	lr ilink, [AUX_ERRET]

	sr 	MPU_ENABLE, [AUX_MPU_EN]
	rtie

_ret_int_rirq:
#endif
	INTERRUPT_EPILOGUE
	sr 	MPU_ENABLE, [AUX_MPU_EN]
	rtie



/*************************************************************************************/
/* the following are return entries of secure call,
 * because of this, user mode is given execute privilege of secureshield runtime,
 * but no read and write privilege
 */
	.text
	.align 4
	.global secureshield_secure_call_int_out
secureshield_secure_call_int_out:
	trap_s SECURESHIELD_SECURE_CALL_INT_OUT

	.align 4
	.global secureshield_secure_call_container_out
secureshield_secure_call_container_out:
	trap_s SECURESHIELD_SECURE_CALL_CONTAINER_OUT