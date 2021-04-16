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
 * \ingroup ARC_HAL_ARC_CONNECT ARC_HAL
 * \brief header file of arc connect module
 */

#ifndef _ARC_CONNECT_H_
#define _ARC_CONNECT_H_

#include "embARC_toolchain.h"
#include "arc.h"
#include "arc_builtin.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	ARC_CONNECT_CMD_CHECK_CORE_ID			= 0x0,

	ARC_CONNECT_CMD_INTRPT_GENERATE_IRQ		= 0x1,
	ARC_CONNECT_CMD_INTRPT_GENERATE_ACK		= 0x2,
	ARC_CONNECT_CMD_INTRPT_READ_STATUS		= 0x3,
	ARC_CONNECT_CMD_INTRPT_CHECK_SOURCE		= 0x4,

	ARC_CONNECT_CMD_SEMA_CLAIM_AND_READ		= 0x11,
	ARC_CONNECT_CMD_SEMA_RELEASE			= 0x12,
	ARC_CONNECT_CMD_SEMA_FORCE_RELEASE		= 0x13,

	ARC_CONNECT_CMD_MSG_SRAM_SET_ADDR		= 0x21,
	ARC_CONNECT_CMD_MSG_SRAM_READ_ADDR		= 0x22,
	ARC_CONNECT_CMD_MSG_SRAM_SET_ADDR_OFFSET	= 0x23,
	ARC_CONNECT_CMD_MSG_SRAM_READ_ADDR_OFFSET	= 0x24,
	ARC_CONNECT_CMD_MSG_SRAM_WRITE			= 0x25,
	ARC_CONNECT_CMD_MSG_SRAM_WRITE_INC		= 0x26,
	ARC_CONNECT_CMD_MSG_SRAM_WRITE_IMM		= 0x27,
	ARC_CONNECT_CMD_MSG_SRAM_READ			= 0x28,
	ARC_CONNECT_CMD_MSG_SRAM_READ_INC		= 0x29,
	ARC_CONNECT_CMD_MSG_SRAM_READ_IMM		= 0x2a,

	ARC_CONNECT_CMD_DEBUG_RESET			= 0x31,
	ARC_CONNECT_CMD_DEBUG_HALT			= 0x32,
	ARC_CONNECT_CMD_DEBUG_RUN			= 0x33,
	ARC_CONNECT_CMD_DEBUG_SET_MASK			= 0x34,
	ARC_CONNECT_CMD_DEBUG_READ_MASK			= 0x35,
	ARC_CONNECT_CMD_DEBUG_SET_SELECT		= 0x36,
	ARC_CONNECT_CMD_DEBUG_READ_SELECT		= 0x37,
	ARC_CONNECT_CMD_DEBUG_READ_EN			= 0x38,
	ARC_CONNECT_CMD_DEBUG_READ_CMD			= 0x39,
	ARC_CONNECT_CMD_DEBUG_READ_CORE			= 0x3a,

	ARC_CONNECT_CMD_GFRC_CLEAR			= 0x41,
	ARC_CONNECT_CMD_GFRC_READ_LO			= 0x42,
	ARC_CONNECT_CMD_GFRC_READ_HI			= 0x43,
	ARC_CONNECT_CMD_GFRC_ENABLE			= 0x44,
	ARC_CONNECT_CMD_GFRC_DISABLE			= 0x45,
	ARC_CONNECT_CMD_GFRC_READ_DISABLE		= 0x46,
	ARC_CONNECT_CMD_GFRC_SET_CORE			= 0x47,
	ARC_CONNECT_CMD_GFRC_READ_CORE			= 0x48,
	ARC_CONNECT_CMD_GFRC_READ_HALT			= 0x49,

	ARC_CONNECT_CMD_PDM_SET_PM			= 0x81,
	ARC_CONNECT_CMD_PDM_READ_PSTATUS		= 0x82,

	ARC_CONNECT_CMD_PMU_SET_PUCNT			= 0x51,
	ARC_CONNECT_CMD_PMU_READ_PUCNT			= 0x52,
	ARC_CONNECT_CMD_PMU_SET_RSTCNT			= 0x53,
	ARC_CONNECT_CMD_PMU_READ_RSTCNT			= 0x54,
	ARC_CONNECT_CMD_PMU_SET_PDCNT			= 0x55,
	ARC_CONNECT_CMD_PMU_READ_PDCNT			= 0x56,

	ARC_CONNECT_CMD_IDU_ENABLE			= 0x71,
	ARC_CONNECT_CMD_IDU_DISABLE			= 0x72,
	ARC_CONNECT_CMD_IDU_READ_ENABLE			= 0x73,
	ARC_CONNECT_CMD_IDU_SET_MODE			= 0x74,
	ARC_CONNECT_CMD_IDU_READ_MODE			= 0x75,
	ARC_CONNECT_CMD_IDU_SET_DEST			= 0x76,
	ARC_CONNECT_CMD_IDU_READ_DEST			= 0x77,
	ARC_CONNECT_CMD_IDU_GEN_CIRQ			= 0x78,
	ARC_CONNECT_CMD_IDU_ACK_CIRQ			= 0x79,
	ARC_CONNECT_CMD_IDU_CHECK_STATUS		= 0x7a,
	ARC_CONNECT_CMD_IDU_CHECK_SOURCE		= 0x7b,
	ARC_CONNECT_CMD_IDU_SET_MASK			= 0x7c,
	ARC_CONNECT_CMD_IDU_READ_MASK			= 0x7d,
	ARC_CONNECT_CMD_IDU_CHECK_FIRST			= 0x7e
} ARC_CONNECT_CMD;

typedef enum {
	ARC_CONNECT_CMD_TYPE_CMD_ONLY			= 0x0,
	ARC_CONNECT_CMD_TYPE_CMD_RETURN			= 0x1,
	ARC_CONNECT_CMD_TYPE_CMD_WDATA			= 0x2,
	ARC_CONNECT_CMD_TYPE_CMD_WDATA_RETURN		= 0x3
} ARC_CONNECT_CMD_TYPE;

typedef struct arc_connect_op {
	ARC_CONNECT_CMD cmd;
	ARC_CONNECT_CMD_TYPE type;
	uint32_t param;
	uint32_t wdata;
} ARC_CONNECT_OP;

typedef enum {
	ARC_CONNECT_INTRPT_TRIGGER_LEVEL = 0,
	ARC_CONNECT_INTRPT_TRIGGER_EDGE = 1
} ARC_CONNECT_INTRPT_TRIGGER_MODE;

typedef enum {
	ARC_CONNECT_DISTRI_MODE_ROUND_ROBIN = 0,
	ARC_CONNECT_DISTRI_MODE_FIRST_ACK = 1,
	ARC_CONNECT_DISTRI_ALL_DEST = 2
} ARC_CONNECT_DISTRI_MODE;

typedef enum {
	ARC_CONNECT_CORE_0 = 1,
	ARC_CONNECT_CORE_1 = 2,
	ARC_CONNECT_CORE_2 = 4,
	ARC_CONNECT_CORE_3 = 8
} ARC_CONNECT_CORE_MASK;

#define ARC_CONNECT_OP_SET(op, cmd_v, type_v, param_v, wdata_v)			{ \
		(op)->cmd = (ARC_CONNECT_CMD)(cmd_v);				\
		(op)->type = (ARC_CONNECT_CMD_TYPE)(type_v);			\
		(op)->param = (uint32_t)(param_v);				\
		(op)->wdata = (uint32_t)(wdata_v); }

#define ARC_CONNECT_CMD_ONLY_OP_SET(op, cmd_v, param_v)				{ \
		(op)->cmd = (ARC_CONNECT_CMD)(cmd_v);				\
		(op)->param = (uint32_t)(param_v);				\
		(op)->type = ARC_CONNECT_CMD_TYPE_CMD_ONLY; }

#define ARC_CONNECT_CMD_RETURN_OP_SET(op, cmd_v, param_v)			{ \
		(op)->cmd = (ARC_CONNECT_CMD)(cmd_v);				\
		(op)->param = (uint32_t)(param_v);				\
		(op)->type = ARC_CONNECT_CMD_TYPE_CMD_RETURN; }

#define ARC_CONNECT_CMD_WDATA_OP_SET(op, cmd_v, param_v, wdata_v)		\
		ARC_CONNECT_OP_SET(op, cmd_v, ARC_CONNECT_CMD_TYPE_CMD_WDATA, param_v, wdata_v)

#define ARC_CONNECT_CMD_WDATA_RETURN_OP_SET(op, cmd_v, param_v, wdata_v)	\
		ARC_CONNECT_OP_SET(op, cmd_v, ARC_CONNECT_CMD_TYPE_CMD_WDATA_RETURN, param_v, wdata_v)


extern uint32_t arc_connect_check_core_id(void);

extern void arc_connect_idu_enable(void);

extern void arc_connect_idu_disable(void);

extern uint32_t arc_connect_idu_read_enable(void);

extern void arc_connect_idu_set_mode(uint32_t irq_num, uint16_t trigger_mode, uint16_t distri_mode);

extern uint32_t arc_connect_idu_read_mode(uint32_t irq_num);

extern void arc_connect_idu_set_dest(uint32_t irq_num, uint32_t target_core);

extern uint32_t arc_connect_idu_read_dest(uint32_t irq_num);

extern void arc_connect_idu_gen_cirq(uint32_t irq_num);

extern void arc_connect_idu_ack_cirq(uint32_t irq_num);

extern uint32_t arc_connect_idu_check_status(uint32_t irq_num);

extern uint32_t arc_connect_idu_check_source(uint32_t irq_num);

extern void arc_connect_idu_set_mask(uint32_t irq_num, uint32_t mask);

extern uint32_t arc_connect_idu_read_mask(uint32_t irq_num);

extern uint32_t arc_connect_idu_check_first(uint32_t irq_num);

extern void arc_connect_init_isr(uint32_t core, uint32_t irq_num, uint16_t trigger_mode, uint16_t distri_mode);

#ifdef __cplusplus
}
#endif

#endif /* _ARC_CONNECT_H_*/
