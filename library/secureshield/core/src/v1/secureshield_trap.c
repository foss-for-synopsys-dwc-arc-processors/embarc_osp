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
//#define DEBUG
//#define DEBUG_HOSTLINK	/* use hostlink to output debug message */
#include "secureshield.h"
#include "secureshield_vmpu.h"
#include "secureshield_int.h"
#include "secureshield_sys.h"
#include "secureshield_secure_call_exports.h"
#include "secureshield_container_stack.h"
#include "secureshield_container_call.h"

extern void secureshield_secure_call_container_out(void);
extern void secureshield_exc_entry_cpu(void);

/**
 * \brief container call in handling
 * \param[in] src_frame exception handling
 * \return  target container sp
 */
static uint32_t trap_container_call_in(INT_EXC_FRAME *src_frame)
{
	uint32_t trap_pc;
	uint8_t src_id, dst_id;
	PROCESSOR_FRAME *dst_frame;
	uint32_t dst_fn;
	/* number of arguments to pass to the target function */
	uint8_t args;
	uint32_t *src, *dst;

	/* container_call is not allowed in interrupt */
	if (arc_int_active()) {
		return 0;
	}

	/* get it from stack or AUX register */
	trap_pc = src_frame->ret - 2; // the length of traps_s is 2 bytes

	if (container_call_check_magic((CONTAINER_CALL *)trap_pc) != 0) {
		return 0;
	}

	dst_fn = container_call_get_dst_fn((CONTAINER_CALL *)trap_pc);
	dst_id = container_call_get_dst_id((CONTAINER_CALL *)trap_pc);

	/* the target container of container call could not be 0 (background container) */
	if (dst_id == 0) {
		return 0;
	}

	src_id = g_container_stack_curr_id;
	/* check src and dst IDs */
	if (src_id == dst_id) {
		SECURESHIELD_HALT("container call is not allowed in the same container", src_id);
		return 0;
	}

	args = src_frame->r0; /* r0 is argument number */

	/* get the right interface from interface table */
	if (vmpu_find_interface(dst_id, (void *)dst_fn, args) == NULL) {
		SECURESHIELD_HALT("interface @%x not found in container %d", dst_fn, dst_id);
		return 0;
	}

	/* push the calling container and set the callee container */
	/* the left registers of src container will be saved later, reserve space here */
	if (container_stack_push(src_id, ((uint32_t *)src_frame) - ARC_CALLEE_FRAME_SIZE,
		(uint32_t *)_arc_aux_read(AUX_USER_SP), src_frame->status32, dst_id) != 0) {
		return 0;
	}

	/* create the cpu frame and exception frame for the destination container */
	dst_frame = (PROCESSOR_FRAME *)(g_container_context[dst_id].cur_sp);

	dst_frame->exc_frame.erbta = 0; /* erbta is 0, no branch */
	dst_frame->exc_frame.ret = dst_fn; /* eret */
	dst_frame->exc_frame.status32 = g_container_context[dst_id].cpu_status;

	/* set the dst_fn as the return address of destination container  */
	dst_frame->exc_frame.blink = (uint32_t)secureshield_secure_call_container_out;

#if !defined(__MW__) || !defined(_NO_SMALL_DATA_)
/* when gp is not changed during execution and sdata is enabled, the following is meaningful */
/* The newlib c of ARC GNU is compiled with sdata enabled */
	dst_frame->exc_frame.gp = src_frame->gp;
#endif

	/* copy parameters */
	src = (uint32_t *)&(src_frame->r1);
	dst = (uint32_t *)&(dst_frame->exc_frame.r0);
	/* r1->r0, r2->r1, ... r6->r5 */
	while(args--) {
		*dst = *src;
		dst++;
		src++;
	}

	/* switch access control tables */
	vmpu_switch(src_id, dst_id);

	/* need to check whether dst_sp is overflow ? */
	return (uint32_t)dst_frame;
}

/**
 * \brief container call out handling
 * \param[in] dst_frame exception frame
 * \return  target container sp
 */
static uint32_t trap_container_call_out(INT_EXC_FRAME *dst_frame)
{
	uint32_t src_id, dst_id;
	PROCESSOR_FRAME *src;

	/* discard the created cpu frame, recover the original sp of destination container */
	dst_id = g_container_stack_curr_id;

	if (container_stack_pop(dst_id, (uint32_t *)dst_frame - ARC_CALLEE_FRAME_SIZE,
		(uint32_t *)_arc_aux_read(AUX_USER_SP), dst_frame->status32) != 0) {
		return 0;
	}

	src_id = g_container_stack[g_container_stack_ptr].src_id;

	src = (PROCESSOR_FRAME *)g_container_stack[g_container_stack_ptr].src_sp;

	/* copy return value */
	src->exc_frame.r0 = dst_frame->r0;

	/* switch access control tables */
	vmpu_switch(dst_id, src_id);

	return (uint32_t)src;
}


/**
 * \brief trap exception handler
 * \param[in] exc_frame   exception frame
 * \return  target container sp
 */
uint32_t secureshield_trap_handler(void *exc_frame)
{
	INT_EXC_FRAME *trap_frame = (INT_EXC_FRAME *)exc_frame;
	uint8_t trap_id;

	SECURESHIELD_ASSERT(trap_frame != NULL);

	trap_id = _arc_aux_read(AUX_ECR) & SECURESHIELD_TRAP_ID_MASK;

	switch (trap_id) {
		case SECURESHIELD_SECURE_CALL_SYS:
			SECURESHIELD_DBG("system operations\r\n");
			secureshield_sys_ops(trap_frame);
			break;
		case SECURESHIELD_SECURE_CALL_CONTAINER_IN:
			SECURESHIELD_DBG("container call in\r\n");
			return trap_container_call_in(trap_frame);
			break;
		case SECURESHIELD_SECURE_CALL_CONTAINER_OUT:
			SECURESHIELD_DBG("container call out\r\n");
			return trap_container_call_out(trap_frame);
			break;
		case SECURESHIELD_SECURE_CALL_INT_OUT:
			SECURESHIELD_DBG("interrupt return\r\n");
			return secureshield_int_return(trap_frame);
			break;
		case SECURESHIELD_SECURE_CALL_INT_EXC:
			SECURESHIELD_DBG("interrupt operations\r\n");
			secureshield_int_ops(trap_frame);
			break;
		default:
			SECURESHIELD_DBG("unknown secure call id%d\r\n", trap_id);
			break;
	}
	return 0;
}