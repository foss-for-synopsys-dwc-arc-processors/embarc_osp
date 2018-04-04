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
#ifndef _SECURESHIELD_CONTAINER_CALL_H_
#define _SECURESHIELD_CONTAINER_CALL_H_

/* container call related definitions */

#include "secureshield.h"
#include "secureshield_secure_call_exports.h"


#if SECURESHIELD_VERSION == 1
/* TRAP_S ID */
#define SECURESHIELD_CONTAINER_CALL_OPCODE \
 	(uint16_t) (0x781E + (SECURESHIELD_SECURE_CALL_CONTAINER_IN << 5))
#elif SECURESHIELD_VERSION == 2

/* SJLI  U12 */
#define SECURESHIELD_CONTAINER_CALL_OPCODE \
	(uint32_t) (SJLI_OPCODE(SECURESHIELD_SECURE_CALL_CONTAINER_IN))
#endif

typedef struct {
#if SECURESHIELD_VERSION == 1
	uint16_t opcode;	/* opcode of trap_s */
#elif SECURESHIELD_VERSION == 2
	uint32_t opcode;	/* opcode of sjli */
#endif
	uint32_t branch;	/* jump instruction */
	uint32_t magic;		/* magic code */
	uint32_t dst_fn;	/* destination function */
	uint32_t *cfg_ptr;	/* pointer to container configuration */
} EMBARC_PACKED CONTAINER_CALL;


/**
 * \brief check the container call structure's magic code
 * \param[in] pc the place where container call is made
 * \return  0 ok, -1 failed
 */
Inline int32_t container_call_check_magic(CONTAINER_CALL *pc)
{
	/* need to do more checks ? */

	if (pc->opcode != SECURESHIELD_CONTAINER_CALL_OPCODE) {
		SECURESHIELD_HALT("container call opcode invalid (0x%x)", pc->opcode);
		return -1;
	}

	if (pc->magic != SECURESHIELD_CONTAINER_CALL_MAGIC) {
		SECURESHIELD_HALT("container call magic invalid (0x%x)", pc->magic);
		return -1;
	}

	return 0;
}

/**
 * \brief get the destination interface function of container call
 * \param[in] pc pc the place where container call is made
 * \return destination interface function
 */
Inline uint32_t container_call_get_dst_fn(CONTAINER_CALL *pc)
{
	return pc->dst_fn;
}

/**
 * \brief get the destination container id
 * \param[in] pc the place where container call is made
 * \return container id, 0 failed
 */
Inline uint32_t container_call_get_dst_id(CONTAINER_CALL *pc)
{
	uint32_t container = pc->cfg_ptr - __secureshield_config.cfgtbl_ptr_start;

	if (container <= 0 || container >= g_vmpu_container_count) {
		SECURESHIELD_HALT("container out of range (%d)", container);
		return 0;
	}

	return container;
}

/**
 * \brief whether the specified container is secure or not
 * \param[in] id container id
 * \return container type
 */
Inline uint32_t container_is_secure(uint8_t id)
{
	return g_container_context[id].cfg->type;
}

#endif/* _SECURESHIELD_CONTAINER_CALL_H_ */
