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
 * \version 2017.03
 * \date 2016-09-18
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */

#define __ASSEMBLY__
#include "arc.h"
#include "arc_asm_common.h"
#include "secureshield_secure_call_exports.h"


#define MPU_DEFAULT_MODE 	0x40000000

	.file "secureshield_entry.s"
/* entry for secureshield cpu exception handling */
	.text
	.global secureshield_exc_entry_cpu
	.align 4
secureshield_exc_entry_cpu:
	/* disable MPU, runtime can do everything */
	sr 	0x0, [AUX_MPU_EN]
 /*  as the sp may be not correct, pop and push is not allowed
 use ilink as temp register, ilink is not used in secureshield */
	lr	ilink, [AUX_ERSTATUS]
	btst	ilink, AUX_STATUS_BIT_U
	bz	_exc_called_from_priv
	/* recover user mode sp, secureshield sp is in user_sp */
	lr	sp, [AUX_USER_SP]
_exc_called_from_priv:

	EXCEPTION_PROLOGUE

	mov	blink, sp

/* switch to secureshield stack */
	mov	sp, __secureshield_stack;
	PUSH	blink

#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
	mov 	gp, _f_sdata
#endif
/* find the exception cause */
	lr	r0, [AUX_ECR]
	lsr	r0, r0, 16
	bmsk	r0, r0, 7
	mov	r1, secureshield_exc_handler_table
	ld.as	r2, [r1, r0]

	mov	r0, blink
	mov	r1, sp
	jl	[r2]		/* jump to exception handler where interrupts are not allowed! */

/*
 * come back to last sp, secureshield stack will not be used
 * 1. to save more callee registers, do container switch
 * 2. no container switch, just return from exception
 */
	POP	sp
	cmp	r0, 0
	beq	_ret_exc

/* there is a context switch request */
	SAVE_CALLEE_REGS	/* save callee save registers */
	mov	sp, r0
	RESTORE_CALLEE_REGS

_ret_exc:	/* return from exception to unprivileged mode*/
	EXCEPTION_EPILOGUE

	/* leave runtime, re-enable the mpu */
	sr 	MPU_DEFAULT_MODE, [AUX_MPU_EN]

/*
 * switch to the target container through exception return.
 * AUX_USER_SP always has the runtime stack pointer
 */
	lr 	ilink, [AUX_ERSTATUS]
	btst 	ilink, AUX_STATUS_BIT_U
	bz	_exc_ret_to_secure
	sr 	sp, [AUX_USER_SP]
	mov 	sp, __secureshield_stack
	rtie
_exc_ret_to_secure:
	mov 	ilink, __secureshield_stack
	sr 	ilink, [AUX_USER_SP]
	rtie
	
/*************************************************************************************/

	.global secureshield_exc_entry_int
	.align 4
secureshield_exc_entry_int:
	clri	/* disable interrupt */
	
	/* disable MPU, runtime can do everything */
	sr 	0x0, [AUX_MPU_EN]

/* check where interrupt comes from: unprivileged or privileged */
/*  utilize the fact that Z bit is set if interrupt taken in U mode*/
	bnz	_int_from_priv
/* recover user sp to save more registers */
	lr	sp, [AUX_USER_SP]
/* interrupt from privileged mode */
	b 	_int_not_from_runtime
_int_from_priv:
	// special handling for the case when nest interrupt happens before clri
	cmp 	ilink, secureshield_exc_entry_int
	bnz	_int_not_from_runtime
	lr 	ilink, [AUX_USER_SP]
	cmp 	ilink, __secureshield_stack
	bz 	_int_not_from_runtime
	lr 	sp, [AUX_USER_SP]
_int_not_from_runtime:
#if ARC_FEATURE_FIRQ == 1
	lr	ilink, [AUX_IRQ_ACT] // ilink can be used
	btst	ilink, 0
	bz	.L_normal_irq
	lr	ilink, [AUX_STATUS32_P0]	/* status32 p0 */
	sr	ilink, [AUX_ERSTATUS] /* return pc is already in eret */
#if ARC_FEATURE_RGF_NUM_BANKS > 1
/* sp is also banked when there are more than 16 banked regs.*/
	kflag	0x1000	/* switch bank0, loop enable, get the original sp */

#endif
	lr 	ilink, [AUX_ERRET]
	EXCEPTION_PROLOGUE
	b .L_common_for_irq_and_firq
#endif
.L_normal_irq:
	INTERRUPT_PROLOGUE

.L_common_for_irq_and_firq:
	mov	blink, sp


/* switch to secureshield stack when user mode switches to kernel mode */
	mov 	sp, __secureshield_stack
_int_handler_1:
	PUSH	blink

#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
	mov 	gp, _f_sdata
#endif

	mov	r0, blink
	mov	r1, sp
	bl	secureshield_interrupt_handle

/*
 * come back to container sp, secureshield stack will not be used
 */
	POP	sp
	kflag	AUX_STATUS_MASK_AE	/* take interrupt as exception, return as an exception */

	cmp	r0, 0
	beq	_ret_int_unprivileged_no_cx

/* there is a context switch request */
	SAVE_CALLEE_REGS	/* save callee save registers */
	mov	sp, r0
	RESTORE_CALLEE_REGS

	EXCEPTION_EPILOGUE

/* interrupt  belongs to the same container, no container switch */
_ret_int_unprivileged_no_cx:
	
	/* leave runtime, re-enable the mpu */
	sr 	MPU_DEFAULT_MODE, [AUX_MPU_EN]

/* AUX_ERET, AUX_ERSTATUS, AUX_ERBTA are set in secureshield_interrupt_handle */
/* set blink to interrupt return */
	mov	blink, secureshield_secure_call_int_out

	lr 	r10, [AUX_ERSTATUS]
	btst 	r10, AUX_STATUS_BIT_U
	bz	_int_exc_ret_to_secure
	sr 	sp, [AUX_USER_SP]
	mov 	sp, __secureshield_stack
	rtie
_int_exc_ret_to_secure:
	mov 	r10, __secureshield_stack
	sr 	r10, [AUX_USER_SP]
	rtie	/* rtie will do an exception return */


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