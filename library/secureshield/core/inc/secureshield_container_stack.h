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
#ifndef _SECURESHIELD_CONTAINER_STACK_H_
#define _SECURESHIELD_CONTAINER_STACK_H_

#include "secureshield.h"

/* maximum depth of nested context switches
 * this includes both interrupts and container call, as they use the same container stack
 * for their context switches */
#define SECURESHIELD_CONTAINER_STACK_MAX_DEPTH 	16

typedef struct {
	uint8_t rfu[3];  /* for 32 bit alignment, reserved for use */
	uint8_t src_id;
	uint32_t *src_sp;
} EMBARC_PACKED CONTAINER_STACK_ITEM;

typedef struct {
	uint32_t *cur_sp;
	uint32_t *normal_sp;
	uint32_t cpu_status;
	const CONTAINER_CONFIG *cfg;
} EMBARC_PACKED CONTAINER_CONTEXT;

/* state variables */
extern CONTAINER_STACK_ITEM g_container_stack[SECURESHIELD_CONTAINER_STACK_MAX_DEPTH];
extern uint32_t g_container_stack_ptr;

extern CONTAINER_CONTEXT g_container_context[SECURESHIELD_MAX_CONTAINERS];
extern uint32_t g_container_stack_curr_id;


/**
 * \brief push a container into the container context stack
 * \param[in] src_id container id
 * \param[in] src_sp container's current sp
 * \param[in] status cpu status
 * \param[in] dst_id destination container
 * \return 0 ok, -1 failed
 */
Inline int32_t container_stack_push(uint8_t src_id, uint32_t *src_sp,
			uint32_t *normal_sp, uint32_t status, uint8_t dst_id)
{
	/* check container stack overflow */
	if (g_container_stack_ptr == SECURESHIELD_CONTAINER_STACK_MAX_DEPTH) {
		SECURESHIELD_HALT("state stack overflow");
		return -1; /* fix coverity issue, never come here */
	}

	/* push state */
	g_container_stack[g_container_stack_ptr].src_id = src_id;
	g_container_stack[g_container_stack_ptr].src_sp = src_sp;
	++g_container_stack_ptr;

	/* save current stack pointer for the src container */
	g_container_context[src_id].cur_sp = src_sp;
	g_container_context[src_id].normal_sp = normal_sp;
	g_container_context[src_id].cpu_status = status;

	/* update current container id */
	g_container_stack_curr_id = dst_id;
	return 0;
}


/**
 * \brief pop a container from the container context stack
 * \param[in] dst_id container id
 * \param[in] dst_sp container's current sp
 * \param[in] status cpu status
 * \return  0 ok, -1 failed
 */
Inline int32_t container_stack_pop(uint8_t dst_id, uint32_t *dst_sp,
				uint32_t *normal_sp, uint32_t status)
{
	/* check container stack underflow */
	if (!g_container_stack_ptr) {
		SECURESHIELD_HALT("state stack underflow");
		return -1; /* fix coverity issue, never come here */
	}

	/* pop container */
	--g_container_stack_ptr;

	/* save current stack pointer */
	g_container_context[dst_id].cur_sp = dst_sp;
	g_container_context[dst_id].normal_sp = normal_sp;
	g_container_context[dst_id].cpu_status = status;

	/* update current container stack id */
	g_container_stack_curr_id = g_container_stack[g_container_stack_ptr].src_id;
	return 0;
}

#endif	/*_SECURESHIELD_CONTAINER_STACK_H_*/
