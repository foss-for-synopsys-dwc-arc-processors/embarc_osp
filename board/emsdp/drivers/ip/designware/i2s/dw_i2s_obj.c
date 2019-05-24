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

#include "emsdp_hardware.h"

#include "ip/designware/i2s/dw_i2s.h"
#include "ip/designware/i2s/dw_i2s_hal.h"
#include "dw_i2s_obj.h"

void dw_i2s_all_install(void);


#if (USE_DW_I2S_RX)
static void dw_i2s_rx_isr(void *ptr);
#define DW_I2S_RX_REFCLK		(24.576)			/*!< DW I2S audio reference clock (MHz) */
#define DW_I2S_RX_RELBASE	(EMSDP_I2S_BASE)		/*!< DW I2S 0 relative baseaddr */

#define DW_I2S_RX_MODE_EN	(DW_I2S_SLAVE_SUPPORTED)	/*!< DW I2S master/slave enabled */
#define DW_I2S_RX_RX_FIFO_LEN	(8)				         /*!< DW I2S FIFO Length */
// TODO #define DW_I2S_RX_CHANNELS	(DW_I2S_CHANNEL0_SUPPORTED | DW_I2S_CHANNEL1_SUPPORTED | DW_I2S_CHANNEL2_SUPPORTED | DW_I2S_CHANNEL3_SUPPORTED)
#define DW_I2S_RX_CHANNELS	(DW_I2S_CHANNEL3_SUPPORTED)

#define DW_I2S_RX_WS_LEN		(DW_I2S_WSS_32_CLK)		/*!< DW I2S word select length*/
//#define DW_I2S_RX_SCLKG		(DW_I2S_SCLKG_24_CLK)	/*!< DW I2S SCLK gate */
#define DW_I2S_RX_SCLKG		(0)				/*!< DW I2S SCLK gate */

static DEV_I2S	dw_i2s_rx;						/*!< DW I2S object */
static DW_I2S_RX_CTRL dw_i2s_rx_ctrl;			/*!< DW I2S 0 ctrl */
static DW_I2S_CONFIG dw_i2s_rx_config;			/*!< DW I2S 0 config */

/** DesignWare I2S 0 open */
static int32_t dw_i2s_rx_open (uint32_t mode, uint32_t param)
{
	return dw_i2s_open(&dw_i2s_rx, mode, param);
}
/** DesignWare I2S 0 close */
static int32_t dw_i2s_rx_close (void)
{
	return dw_i2s_close(&dw_i2s_rx);
}
/** DesignWare I2S 0 control */
static int32_t dw_i2s_rx_control (uint32_t ctrl_cmd, void *param)
{
	return dw_i2s_control(&dw_i2s_rx, ctrl_cmd, param);
}

/** DesignWare I2S 0 read */
static int32_t dw_i2s_rx_read (void *data, uint32_t len)
{
	return dw_i2s_read(&dw_i2s_rx, data, len, DW_I2S_CHANNEL0);
}

/** DesignWare I2S 0 interrupt routine */
static void dw_i2s_rx_isr(void *ptr)
{
	dw_i2s_isr_rx(&dw_i2s_rx, ptr);
}

/** Install DesignWare I2S 0 to system */
static void dw_i2s_rx_install(void)
{
	uint32_t i2s_abs_base = 0;
	DEV_I2S *dw_i2s_ptr = &dw_i2s_rx;
	DEV_I2S_INFO *dw_i2s_info_ptr = &(dw_i2s_rx.i2s_info);
	DW_I2S_RX_CTRL *dw_i2s_ctrl_ptr = &dw_i2s_rx_ctrl;
	DW_I2S_CONFIG *dw_i2s_config_ptr = &dw_i2s_rx_config;
	DW_I2S_RX_REG *dw_i2s_reg_ptr;

	/* Info init */
	dw_i2s_info_ptr->i2s_ctrl = (void *)dw_i2s_ctrl_ptr;
	dw_i2s_info_ptr->i2s_config = (void *)dw_i2s_config_ptr;
	dw_i2s_info_ptr->opn_cnt = 0;
	dw_i2s_info_ptr->status = DEV_DISABLED;
	dw_i2s_info_ptr->device = I2S_DEVICE_RECEIVER;
	dw_i2s_info_ptr->mode = DEV_SLAVE_MODE;
	dw_i2s_info_ptr->cur_state = I2S_FREE;
	dw_i2s_info_ptr->err_state = I2S_ERR_NONE;

	/*
	 * get absolute designware base address
	 */
	i2s_abs_base = (uint32_t)EMSDP_I2S_BASE; // Memory address
	dw_i2s_reg_ptr = (DW_I2S_RX_REG *)i2s_abs_base;

	/* I2S ctrl init */
	dw_i2s_ctrl_ptr->dw_i2s_regs = dw_i2s_reg_ptr;
	/* Variables which should be set during object implementation */
	dw_i2s_config_ptr->support_modes = DW_I2S_RX_MODE_EN;
	dw_i2s_config_ptr->fifo_len = DW_I2S_RX_RX_FIFO_LEN;
	dw_i2s_config_ptr->channels = DW_I2S_RX_CHANNELS;
	dw_i2s_config_ptr->ws_length = DW_I2S_RX_WS_LEN;
	dw_i2s_config_ptr->sclk_gate = DW_I2S_RX_SCLKG;
	dw_i2s_config_ptr->data_res[0] = I2S_AUD_DATA_16B;
	// sample rate 16KHz as default, the clock divider should be 1538/(32*2) in I2S_WS_LENGTH = 32
	dw_i2s_config_ptr->sample_rate[0] = I2S_AUD_SR_16K;
	dw_i2s_config_ptr->intno[0] = (EMSDP_I2S_RX_OR0_INTR<<16) | EMSDP_I2S_RX_DA0_INTR;
	dw_i2s_config_ptr->intno[1] = (EMSDP_I2S_RX_OR1_INTR<<16) | EMSDP_I2S_RX_DA1_INTR;
	dw_i2s_config_ptr->intno[2] = (EMSDP_I2S_RX_OR2_INTR<<16) | EMSDP_I2S_RX_DA2_INTR;
	dw_i2s_config_ptr->intno[3] = (EMSDP_I2S_RX_OR3_INTR<<16) | EMSDP_I2S_RX_DA3_INTR;
	dw_i2s_config_ptr->dw_i2s_int_handler = dw_i2s_rx_isr;

	/* Variables which always change during I2S operation */
	dw_i2s_ctrl_ptr->int_status[0] = 0;

	/** I2S dev init */
	dw_i2s_ptr->i2s_open = dw_i2s_rx_open;
	dw_i2s_ptr->i2s_close = dw_i2s_rx_close;
	dw_i2s_ptr->i2s_control = dw_i2s_rx_control;
	dw_i2s_ptr->i2s_write = NULL;
	dw_i2s_ptr->i2s_read = dw_i2s_rx_read;
}
#endif /* USE_DW_I2S_RX */

/** get one designware device structure */
DEV_I2S_PTR dw_i2s_get_dev(int32_t i2s_id)
{
	static uint32_t install_flag = 0;

	/* intall device objects */
	if (install_flag == 0) {
		install_flag = 1;
		dw_i2s_all_install();
	}

	switch (i2s_id) {
#if (USE_DW_I2S_RX)
		case DW_I2S_RX_ID:
			return &dw_i2s_rx;
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
void dw_i2s_all_install(void)
{
#if (USE_DW_I2S_RX)
	dw_i2s_rx_install();
#endif
}
