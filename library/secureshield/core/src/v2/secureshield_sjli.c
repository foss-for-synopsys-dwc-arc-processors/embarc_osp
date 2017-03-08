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
 * \version 2017.03
 * \date 2016-09-18
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */
//#define DEBUG
//#define DEBUG_HOSTLINK	/* use hostlink to output debug message */
#include "secureshield.h"
#include "secureshield_vmpu.h"
#include "secureshield_int.h"
#include "secureshield_sys.h"
#include "secureshield_sjli.h"
#include "secureshield_container_stack.h"
#include "secureshield_container_call.h"


extern void sjli_container_call_in(void);
extern void sjli_secureshield_sys_ops(void);
extern void sjli_secureshield_int_ops(void);
static void sjli_default_entry(void);


// a trick is used here, secureshield_sjli_entry_table must be in instructions space
// (e.g., if it in dccm, an exception will be raised)
//__attribute__ ((section(".sjli_table")))
const SJLI_ENTRY secureshield_sjli_entry_table[NUM_SJLI_ENTRY] = {
	[0] = sjli_secureshield_sys_ops,
	[1] = sjli_container_call_in,
	[2] = sjli_secureshield_int_ops,
	[3 ... NUM_SJLI_ENTRY-1] = sjli_default_entry /* secure application can add its own sjli service here */
 };


/**
 * \brief default sjli entry
 */
static void sjli_default_entry(void) 
{
	SECURESHIELD_HALT("default sjli entry");
}

/**
 * \brief the handler of container call, caller -> callee
 * \param[in]  src_frame container context frame
 * \return target container's stack pointer
 */
uint32_t container_call_in(INT_EXC_FRAME *src_frame)
{
	
	uint8_t src_id, dst_id;
	INT_EXC_FRAME *dst_frame;
	uint32_t dst_fn;
	/* number of arguments to pass to the target function */
	uint8_t args;
	uint32_t *src, *dst;
	uint8_t secure = 0;
#if ARC_FEATURE_MPU_BUILD_S == 1 && SECURESHIELD_USE_MPU_SID == 1
	dst_fn = src_frame->r2;
	dst_id = (uint32_t *)src_frame->r1 - __secureshield_config.cfgtbl_ptr_start;

	if (dst_id <= 0 || dst_id >= g_vmpu_container_count) {
		SECURESHIELD_HALT("container out of range (%d)", dst_id);
		return 0;
	}
#else
	uint32_t pc;
	/* get caller pc */
	pc = src_frame->ret - SJLI_INSTRUCTION_LENGTH;

	if (container_call_check_magic((CONTAINER_CALL *)pc) != 0) {
		return 0;
	}
	dst_fn = container_call_get_dst_fn((CONTAINER_CALL *)pc);
	dst_id = container_call_get_dst_id((CONTAINER_CALL *)pc);

	// the target container of container call could not be 0 (background container)
	if (dst_id == 0) {
		return 0;
	}
#endif
	src_id = g_container_stack_curr_id;
	/* check src and dst IDs */
	if (src_id == dst_id) {
		SECURESHIELD_HALT("container call is not allowed in the same container %d", src_id);
		return 0;
	}
	_arc_aux_write(AUX_MPU_EN, MPU_DEFAULT_MODE);
	args = src_frame->r0; /* r0 is argument number */

	/* get the right interface from interface table */
	if (vmpu_find_interface(dst_id, (void *)dst_fn, args) == NULL) {
		SECURESHIELD_HALT("interface @%x not found in container %d", dst_fn, dst_id);
		return 0;
	}

	
	/* push the calling container and set the callee container */
	/* the left registers of src container will be saved later, reserve space here */
	container_stack_push(src_id, ((uint32_t *)src_frame) - ARC_CALLEE_FRAME_SIZE,
		src_frame->status32, dst_id);

	/* create the cpu frame and exception frame for the destination container */
	dst_frame = (INT_EXC_FRAME *)(g_container_context[dst_id].cur_sp - ARC_EXC_FRAME_SIZE);

	dst_frame->erbta = 0; /* erbta, is 0 the correct value? */
	dst_frame->ret = dst_fn; /* eret */
	dst_frame->status32 = g_container_context[dst_id].cpu_status;

#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
/* when gp is not changed during execution and sdata is enabled, the following is meaningful */
/* The newlib c of ARC GNU is compiled with sdata enabled */
	dst_frame->gp = src_frame->gp;
// normal world's gp is different with secure world's gp
#endif

	/* copy parameters */
#if ARC_FEATURE_MPU_BUILD_S == 1 && SECURESHIELD_USE_MPU_SID == 1
	src = (uint32_t *)&(src_frame->r3);
#else
	src = (uint32_t *)&(src_frame->r1);
#endif
	dst = (uint32_t *)&(dst_frame->r0);
	/* r1->r0, r2->r1, ... r6->r5 */



	while(args--) {
		*dst = *src;
		dst++;
		src++;
	}

	/* switch access control tables */
	vmpu_switch(src_id, dst_id);


	_arc_aux_write(AUX_MPU_PROBE, dst_fn);

	if (_arc_aux_read(AUX_MPU_RPER) & (1 << AUX_MPU_RPER_BIT_S)) {
		secure = 1;
	}

	/* for all normal interrupts happed in secure world, it's handled in background container stack */
	if (secure == 1) {
		_arc_aux_write(AUX_KERNEL_SP, (uint32_t)g_container_context[0].cur_sp);
	}

	/* need to check whether dst_sp is overflow ? */
	return (((uint32_t)dst_frame) | secure);
}

/**
 * \brief the post handler of container call,  callee->caller
 * \param[in]  ret_value return value from callee container
 * \param[in]  sp        callee container's stack pointer
 * \param[in]  status32  callee container's status register
 * \return     caller container's stack pointer
 */
uint32_t container_call_out(uint32_t ret_value, uint32_t *sp, uint32_t status32)
{
	uint32_t src_id, dst_id;
	PROCESSOR_FRAME *src;
	uint8_t secure = 0;

	_arc_aux_write(AUX_MPU_EN, MPU_DEFAULT_MODE);
	/* discard the created cpu frame, recover the original sp of destination container */
	dst_id = g_container_stack_curr_id;


	container_stack_pop(dst_id, sp, status32);

	src_id = g_container_stack[g_container_stack_ptr].src_id;


	src = (PROCESSOR_FRAME *)g_container_stack[g_container_stack_ptr].src_sp;

	/* copy return value */
	src->exc_frame.r0 = ret_value;

	/* switch access control tables */
	vmpu_switch(dst_id, src_id);
	
	_arc_aux_write(AUX_MPU_PROBE, src->exc_frame.ret);
	if (_arc_aux_read(AUX_MPU_RPER) & (1 << AUX_MPU_RPER_BIT_S)) {
		secure = 1;
	}

	/* for all normal interrupts happened in secure world, it's handled in background container stack */
	if (secure == 1) {
		_arc_aux_write(AUX_KERNEL_SP, (uint32_t)g_container_context[0].cur_sp);
	}

	return  ((uint32_t) g_container_stack[g_container_stack_ptr].src_sp) | secure;
}

/**
 * \brief sjli init
 */
void sjli_init(void)
{
	_arc_aux_write(AUX_NSC_TABLE_BASE, (uint32_t)secureshield_sjli_entry_table);
	_arc_aux_write(AUX_NSC_TABLE_TOP, (uint32_t)(secureshield_sjli_entry_table + NUM_SJLI_ENTRY));
}