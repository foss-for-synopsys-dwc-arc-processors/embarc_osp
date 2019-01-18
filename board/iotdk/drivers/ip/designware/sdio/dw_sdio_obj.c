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
#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "iotdk_hardware.h"
#include "ip/designware/sdio/dw_sdio.h"
#include "dw_sdio_obj.h"

#if (USE_DW_SDIO_0)

static DW_SDIO_CTRL dw_sdio_0_ctrl;
static DEV_SDIO dw_sdio_0;


static void dw_sdio_0_isr(void *ptr)
{
	dw_sdio_isr(&dw_sdio_0, ptr);
}

static int32_t dw_sdio_0_open(uint32_t card)
{

	return dw_sdio_open(&dw_sdio_0, card);
}

static int32_t dw_sdio_0_close(uint32_t card)
{
	return dw_sdio_close(&dw_sdio_0, card);
}

static int32_t dw_sdio_0_control(SDIO_CTRL_CMD_PTR ctrl_cmd, void *param)
{
	return dw_sdio_control(&dw_sdio_0, ctrl_cmd, param);
}

static int32_t dw_sdio_0_cmd_poll(SDIO_CMD_PTR cmd, SDIO_DATA_PTR data)
{
	return dw_sdio_cmd_poll(&dw_sdio_0, cmd, data);
}

static int32_t dw_sdio_0_cmd_int(SDIO_CMD_PTR cmd, SDIO_DATA_PTR data, DEV_CALLBACK cb)
{
	return -1;
}

static int32_t dw_sdio_0_cd(uint32_t card)
{
	return dw_sdio_cd(&dw_sdio_0, card);
}

static int32_t dw_sdio_0_wp(uint32_t card)
{
	return dw_sdio_wp(&dw_sdio_0, card);
}

static void dw_sdio_0_install(void)
{
	DEV_SDIO_PTR dw_sdio_ptr = &dw_sdio_0;
	DEV_SDIO_INFO_PTR dw_sdio_info_ptr = &(dw_sdio_0.sdio_info);
	DW_SDIO_CTRL_PTR dw_sdio_ctrl_ptr = &dw_sdio_0_ctrl;

	dw_sdio_info_ptr->opn_cnt = 0;
	dw_sdio_info_ptr->sdio_ctrl = (void *)dw_sdio_ctrl_ptr;
	dw_sdio_info_ptr->status = DEV_DISABLED;


	dw_sdio_ctrl_ptr->reg_base = (void *) BASE_ADDR_SDIO;
	dw_sdio_ctrl_ptr->intno = INTNO_SDIO;
	dw_sdio_ctrl_ptr->ref_clk = CLK_CPU;
	dw_sdio_ctrl_ptr->fifo_depth = 128;

	dw_sdio_ptr->open = dw_sdio_0_open;
	dw_sdio_ptr->close = dw_sdio_0_close;
	dw_sdio_ptr->control = dw_sdio_0_control;
	dw_sdio_ptr->cmd_poll = dw_sdio_0_cmd_poll;
	dw_sdio_ptr->cmd_int = dw_sdio_0_cmd_int;
	dw_sdio_ptr->cd = dw_sdio_0_cd;
	dw_sdio_ptr->wp = dw_sdio_0_wp;
}

#endif /* USE_DW_SDIO 0*/

DEV_SDIO_PTR sdio_get_dev(uint32_t sdio_id)
{
	static uint32_t install_flag = 0;

	if (install_flag == 0) {
		install_flag = 1;
		dw_sdio_all_install();
	}

	switch (sdio_id) {
#if (USE_DW_SDIO_0)

		case DW_SDIO_0_ID:
			return &dw_sdio_0;
			break;
#endif

		default:
			break;
	}

	return NULL;
}


void dw_sdio_all_install()
{
#if (USE_DW_SDIO_0)
	dw_sdio_0_install();
#endif
}