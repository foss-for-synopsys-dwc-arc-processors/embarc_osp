/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

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

#include "board.h"

#include "dfss_i2s_obj.h"
#include "ip/subsystem/i2s/ss_i2s_master.h"


void dfss_i2s_all_install(void);


#if (USE_DFSS_I2S_0)	//TX

static void dfss_i2s_0_tx_cb0(void *param);
static void dfss_i2s_0_rx_cb0(void *param);
static void dfss_i2s_0_err_cb0(void *param);

static DEV_I2S	dfss_i2s_0;						/*!< DFSS I2S object */
SS_I2S_MST_DEV_CONTEXT i2s_master_context0 = {
	.reg_base = AR_IO_I2S_TX_MST0_IER,

	.dev_id = 0,
	.io_mode = SS_I2S_TX,

	.int_tx_req = IO_I2S_TX_MST0_INT_TX_REQ,
	.int_tx_err = IO_I2S_TX_MST0_INT_ERR,
	.int_rx_avil = 0,
	.int_rx_err = 0,

	.int_tx_cb = dfss_i2s_0_tx_cb0,
	.int_rx_cb = dfss_i2s_0_rx_cb0,
	.int_err_cb = dfss_i2s_0_err_cb0,
};

static void dfss_i2s_0_isr(void *ptr);

void _regWrite (uint32_t addr, uint32_t val)
{
	volatile uint32_t *reg;

	reg = (volatile uint32_t *) addr;

	*reg = val;
}
/** DesignWare I2S 0 open */
static int32_t dfss_i2s_0_open (uint32_t mode, uint32_t param)
{
	_regWrite(EMSDP_CRU_BASE + 0x1A8, EMSDP_AUDIO_REF_CLOCK/(32*param));
	return ss_i2s_mst_open(&i2s_master_context0, mode, param);
}
/** DesignWare I2S 0 close */
static int32_t dfss_i2s_0_close (void)
{
	return ss_i2s_mst_close(&i2s_master_context0);
}
/** DesignWare I2S 0 control */
static int32_t dfss_i2s_0_control (uint32_t ctrl_cmd, void *param)
{
	return ss_i2s_mst_control(&i2s_master_context0, ctrl_cmd, param);
}
/** DesignWare I2S 0 write */
static int32_t dfss_i2s_0_write (const void *data, uint32_t len)
{
	return ss_i2s_mst_write(&i2s_master_context0, (void *)data, len);
}
/** DesignWare I2S 0 interrupt routine */
static void dfss_i2s_0_tx_cb0(void *param)
{
	ss_i2s_mst_int_tx_cb(&i2s_master_context0, param);
}

static void dfss_i2s_0_rx_cb0(void *param)
{
	return;
}

static void dfss_i2s_0_err_cb0(void *param)
{
	ss_i2s_mst_int_err_cb(&i2s_master_context0, param);
}
/** Install DesignWare I2S 0 to system */
static void dfss_i2s_0_install(void)
{
	DEV_I2S_PTR dfss_i2s_ptr = &dfss_i2s_0;
	DEV_I2S_INFO_PTR dfss_i2s_info_ptr = &(dfss_i2s_0.i2s_info);

	i2s_master_context0.info = dfss_i2s_info_ptr;

	/* Info init */
	dfss_i2s_info_ptr->opn_cnt = 0;
	dfss_i2s_info_ptr->status = DEV_DISABLED;
	dfss_i2s_info_ptr->device = I2S_DEVICE_TRANSMITTER;
	dfss_i2s_info_ptr->mode = DEV_MASTER_MODE;
	dfss_i2s_info_ptr->cur_state = I2S_FREE;
	dfss_i2s_info_ptr->err_state = I2S_ERR_NONE;

	/** I2S dev init */
	dfss_i2s_ptr->i2s_open = dfss_i2s_0_open;
	dfss_i2s_ptr->i2s_close = dfss_i2s_0_close;
	dfss_i2s_ptr->i2s_control = dfss_i2s_0_control;
	dfss_i2s_ptr->i2s_write = dfss_i2s_0_write;
	dfss_i2s_ptr->i2s_read = NULL;
}
#endif /* USE_DFSS_I2S_0 */

#if (USE_DFSS_I2S_1)	//RX

static void dfss_i2s_1_tx_cb1(void *param);
static void dfss_i2s_1_rx_cb1(void *param);
static void dfss_i2s_1_err_cb1(void *param);

static DEV_I2S	dfss_i2s_1;						/*!< DFSS I2S object */
SS_I2S_MST_DEV_CONTEXT i2s_master_context1 = {
	.reg_base = AR_IO_I2S_RX_MST0_IER,

	.dev_id = 0,
	.io_mode = SS_I2S_RX,

	.int_tx_req = 0,
	.int_tx_err = 0,
	.int_rx_avil = IO_I2S_RX_MST0_INT_RX_AVAIL,
	.int_rx_err = IO_I2S_RX_MST0_INT_ERR,

	.int_tx_cb = dfss_i2s_1_tx_cb1,
	.int_rx_cb = dfss_i2s_1_rx_cb1,
	.int_err_cb = dfss_i2s_1_err_cb1,
};

static void dfss_i2s_1_isr(void *ptr);

/** DesignWare I2S 1 open */
static int32_t dfss_i2s_1_open (uint32_t mode, uint32_t param)
{
	_regWrite(EMSDP_CRU_BASE + 0x1A8, EMSDP_AUDIO_REF_CLOCK/(32*param));
	return ss_i2s_mst_open(&i2s_master_context1, mode, param);
}
/** DesignWare I2S 1 close */
static int32_t dfss_i2s_1_close (void)
{
	return ss_i2s_mst_close(&i2s_master_context1);
}
/** DesignWare I2S 1 control */
static int32_t dfss_i2s_1_control (uint32_t ctrl_cmd, void *param)
{
	return ss_i2s_mst_control(&i2s_master_context1, ctrl_cmd, param);
}
/** DesignWare I2S 1 read */
static int32_t dfss_i2s_1_read (void *data, uint32_t len)
{
	return ss_i2s_mst_read(&i2s_master_context1, data, len);
}
/** DesignWare I2S 1 interrupt routine */
static void dfss_i2s_1_tx_cb1(void *param)
{
	return;
}

static void dfss_i2s_1_rx_cb1(void *param)
{
	ss_i2s_mst_int_rx_cb(&i2s_master_context1, param);
}

static void dfss_i2s_1_err_cb1(void *param)
{
	ss_i2s_mst_int_err_cb(&i2s_master_context1, param);
}
/** Install DesignWare I2S 1 to system */
static void dfss_i2s_1_install(void)
{
	DEV_I2S *dfss_i2s_ptr = &dfss_i2s_1;
	DEV_I2S_INFO *dfss_i2s_info_ptr = &(dfss_i2s_1.i2s_info);
	i2s_master_context1.info = dfss_i2s_info_ptr;

	/* Info init */
	dfss_i2s_info_ptr->opn_cnt = 0;
	dfss_i2s_info_ptr->status = DEV_DISABLED;
	dfss_i2s_info_ptr->device = I2S_DEVICE_RECEIVER;
	dfss_i2s_info_ptr->mode = DEV_MASTER_MODE;
	dfss_i2s_info_ptr->cur_state = I2S_FREE;
	dfss_i2s_info_ptr->err_state = I2S_ERR_NONE;

	/** I2S dev init */
	dfss_i2s_ptr->i2s_open = dfss_i2s_1_open;
	dfss_i2s_ptr->i2s_close = dfss_i2s_1_close;
	dfss_i2s_ptr->i2s_control = dfss_i2s_1_control;
	dfss_i2s_ptr->i2s_write = NULL;
	dfss_i2s_ptr->i2s_read = dfss_i2s_1_read;
}
#endif /* USE_DFSS_I2S_1 */

/** get one designware device structure */
DEV_I2S_PTR dfss_i2s_get_dev(int32_t i2s_id)
{
	static uint32_t install_flag = 0;

	/* intall device objects */
	if (install_flag == 0) {
		install_flag = 1;
		dfss_i2s_all_install();
	}

	switch (i2s_id) {
#if (USE_DFSS_I2S_0)
		case DFSS_I2S_0_ID:
			return &dfss_i2s_0;
			break;
#endif
#if (USE_DFSS_I2S_1)
		case DFSS_I2S_1_ID:
			return &dfss_i2s_1;
			break;
#endif
		default:
			break;
	}
	return NULL;
}

/**
 * \brief	install all i2s objects
 * \note	\b MUST be called during system init
 */
void dfss_i2s_all_install(void)
{
#if (USE_DFSS_I2S_0)
	dfss_i2s_0_install();
#endif
#if (USE_DFSS_I2S_1)
	dfss_i2s_1_install();
#endif
}
