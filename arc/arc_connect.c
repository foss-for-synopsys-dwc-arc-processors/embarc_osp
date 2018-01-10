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
#include "arc_connect.h"

uint32_t arc_connect_cmd_execute(ARC_CONNECT_OP *op)
{
	uint32_t regval = 0;

	if (!op) return 0;
	regval = op->cmd | ((op->param)<<8);
	switch (op->type) {
		case ARC_CONNECT_CMD_TYPE_CMD_ONLY:
			_arc_sr_reg(AUX_CONNECT_CMD, regval);
			break;
		case ARC_CONNECT_CMD_TYPE_CMD_RETURN:
			_arc_sr_reg(AUX_CONNECT_CMD, regval);
			return _arc_lr_reg(AUX_CONNECT_READBACK);
			break;
		case ARC_CONNECT_CMD_TYPE_CMD_WDATA:
			_arc_sr_reg(AUX_CONNECT_WDATA, op->wdata);
			_arc_sr_reg(AUX_CONNECT_CMD, regval);
			break;
		case ARC_CONNECT_CMD_TYPE_CMD_WDATA_RETURN:
			_arc_sr_reg(AUX_CONNECT_WDATA, op->wdata);
			_arc_sr_reg(AUX_CONNECT_CMD, regval);
			return _arc_lr_reg(AUX_CONNECT_READBACK);
			break;
		default:
			break;
	}
	return 0;
}

uint32_t arc_connect_check_core_id(void)
{
	ARC_CONNECT_OP op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_CHECK_CORE_ID, 0);

	return arc_connect_cmd_execute(&op);
}

void arc_connect_idu_enable(void)
{
	ARC_CONNECT_OP op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_IDU_ENABLE, 0);

	arc_connect_cmd_execute(&op);
}

void arc_connect_idu_disable(void)
{
	ARC_CONNECT_OP op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_IDU_DISABLE, 0);

	arc_connect_cmd_execute(&op);
}

uint32_t arc_connect_idu_read_enable(void)
{
	ARC_CONNECT_OP op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_IDU_READ_ENABLE, 0);

	return arc_connect_cmd_execute(&op);
}

void arc_connect_idu_set_mode(uint32_t irq_num, uint16_t trigger_mode, uint16_t distri_mode)
{
	ARC_CONNECT_OP op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_IDU_SET_MODE, \
		irq_num, (distri_mode|(trigger_mode<<4)));

	arc_connect_cmd_execute(&op);
}

uint32_t arc_connect_idu_read_mode(uint32_t irq_num)
{
	ARC_CONNECT_OP op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_IDU_READ_ENABLE, irq_num);

	return arc_connect_cmd_execute(&op);
}

void arc_connect_idu_set_dest(uint32_t irq_num, uint32_t target_core)
{
	ARC_CONNECT_OP op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_IDU_SET_DEST, irq_num, target_core);

	arc_connect_cmd_execute(&op);
}

uint32_t arc_connect_idu_read_dest(uint32_t irq_num)
{
	ARC_CONNECT_OP op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_IDU_READ_DEST, irq_num);

	return arc_connect_cmd_execute(&op);
}

void arc_connect_idu_gen_cirq(uint32_t irq_num)
{
	ARC_CONNECT_OP op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_IDU_GEN_CIRQ, irq_num);

	arc_connect_cmd_execute(&op);
}

void arc_connect_idu_ack_cirq(uint32_t irq_num)
{
	ARC_CONNECT_OP op;

	ARC_CONNECT_CMD_ONLY_OP_SET(&op, ARC_CONNECT_CMD_IDU_ACK_CIRQ, irq_num);

	arc_connect_cmd_execute(&op);
}

uint32_t arc_connect_idu_check_status(uint32_t irq_num)
{
	ARC_CONNECT_OP op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_IDU_CHECK_STATUS, irq_num);

	return arc_connect_cmd_execute(&op);
}

uint32_t arc_connect_idu_check_source(uint32_t irq_num)
{
	ARC_CONNECT_OP op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_IDU_CHECK_SOURCE, irq_num);

	return arc_connect_cmd_execute(&op);
}

void arc_connect_idu_set_mask(uint32_t irq_num, uint32_t mask)
{
	ARC_CONNECT_OP op;

	ARC_CONNECT_CMD_WDATA_OP_SET(&op, ARC_CONNECT_CMD_IDU_SET_MASK, irq_num, mask);

	arc_connect_cmd_execute(&op);
}

uint32_t arc_connect_idu_read_mask(uint32_t irq_num)
{
	ARC_CONNECT_OP op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_IDU_READ_MASK, irq_num);

	return arc_connect_cmd_execute(&op);
}

uint32_t arc_connect_idu_check_first(uint32_t irq_num)
{
	ARC_CONNECT_OP op;

	ARC_CONNECT_CMD_RETURN_OP_SET(&op, ARC_CONNECT_CMD_IDU_CHECK_FIRST, irq_num);

	return arc_connect_cmd_execute(&op);
}

void arc_connect_init_isr(uint32_t core, uint32_t irq_num, uint16_t trigger_mode, uint16_t distri_mode)
{
	arc_connect_idu_disable();
	arc_connect_idu_set_mode(irq_num, trigger_mode, distri_mode);
	arc_connect_idu_set_dest(irq_num, core);
	arc_connect_idu_set_mask(irq_num, 0x0);
	arc_connect_idu_enable();
}