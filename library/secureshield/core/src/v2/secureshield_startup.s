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
 * \date 2016-10-21
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */
#define __ASSEMBLY__
#include "arc.h"
#include "arc_asm_common.h"

.file "secureshield_startup.s"

.weak	_f_sdata		/* start of small data, defined in link script */

	.section .secureshield_init, "ax"
	.global _secureshield_start
	.align 4
_secureshield_start:
_init_phase1:
_copy_text:
	mov	r0, _f_text
	mov	r1, _load_addr_text
	cmp	r0, r1

/* if load addr == run addr, no need to copy */
	jeq	_copy_rodata
	mov	r3, _e_text
_copy_text_loop:
	ld.ab	r2, [r1, 4]
	st.ab	r2, [r0, 4]
	cmp	r0, r3
	jlt	_copy_text_loop
_copy_rodata:
	mov	r0, _f_rodata
	mov	r1, _load_addr_rodata
	cmp	r0, r1

/* if load addr == run addr, no need to copy */
	jeq	_copy_data
	mov	r3, _e_rodata
_copy_rodata_loop:
	ld.ab	r2, [r1, 4]
	st.ab	r2, [r0, 4]
	cmp	r0, r3
	jlt	_copy_rodata_loop
_copy_data:
	mov	r0, _f_data
	mov	r1, _load_addr_data
	cmp	r0, r1
	jeq	_clear_bss

/* if load addr == run addr, no need to copy */
	mov	r3, _e_data
_copy_data_loop:
	ld.ab	r2, [r1, 4]
	st.ab	r2, [r0, 4]
	cmp	r0, r3
	jlt	_copy_data_loop
_clear_bss:
	mov	r0, _f_bss
	mov	r1, _e_bss
	cmp	r0, r1
	jge	_init_phase2
	mov	r2, 0
_clear_bss_loop:
	st.ab	r2, [r0, 4]
	cmp	r0, r1
	jlt	_clear_bss_loop
	
	/* switch to secureshield runtime stack, save original sp into AUX_USER_SP */
	mov 	r0, __secureshield_stack
	sr 	r0, [AUX_SEC_K_SP]
	sr 	r0, [AUX_KERNEL_SP]
	/* sp is the secure sp, aux_kernel_sp is normal (background container) */
	aex 	sp, [AUX_KERNEL_SP]
	push 	blink
	/* switch to secure small data */
	push 	gp
	mov 	gp, _f_sdata

_init_phase2:

#if defined(__MW__)
	jl	_init
#elif defined(__GNU__)
	jl	__do_global_ctors_aux
	jl	__do_init_array_aux
#endif
	jl 	secure_console_init
	jl 	vmpu_init_pre
	cmp	r0, 0
	jz	_next_stage
#if defined(__MW__)
	jl	_fini
#elif defined(__GNU__)
	jl	__do_global_dtors_aux
#endif
	pop 	gp
	pop 	blink
	lr 	sp, [AUX_KERNEL_SP]
	j 	[blink]
_next_stage:
	jl 	secureshield_init
	// sync-up AUX_SEC_STAT and AUX_ERSEC_STAT
	lr 	r0, [AUX_SEC_STAT]
	sr 	r0, [AUX_ERSEC_STAT]
	// disable default mpu region
	sr 	0,  [AUX_MPU_EN]
	pop 	gp
	pop 	blink
#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
	st 	gp, [normal_world_gp]
#endif
	CLEAR_SCRATCH_REGS
	j 	[blink]

/*************************************************************************************/
	.global _exit_loop
	.global _exit_halt
	.align 4
_exit_halt:
_exit_loop:
	flag	0x1
	nop
	nop
	nop
	b	_exit_loop

#if defined(__MW__)
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
#endif