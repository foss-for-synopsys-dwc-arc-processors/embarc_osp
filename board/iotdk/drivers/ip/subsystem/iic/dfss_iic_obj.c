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

#include "iotdk_hardware.h"

#include "dfss_iic_obj.h"
#include "ip/subsystem/iic/ss_i2c_master.h"

#if (USE_DFSS_IIC_0)

static void iic_tx_cb0(void *param);
static void iic_rx_cb0(void *param);
static void iic_err_cb0(void *param);

#define DFSS_IIC_0_TARADDR	(0)

static DEV_IIC			dfss_iic_0;

static SS_IIC_MASTER_DEV_CONTEXT iic_master_context0 = {
	.reg_base = AR_IO_I2C_MST0_CON,
	.dev_id = 0,
	.int_err = IO_I2C_MST0_INT_ERR,
	.int_rx_avail = IO_I2C_MST0_INT_RX_AVAIL,
	.int_tx_req = IO_I2C_MST0_INT_TX_REQ,
	.int_stop_det = IO_I2C_MST0_INT_STOP_DET,
	.int_rx_cb = iic_rx_cb0,
	.int_tx_cb = iic_tx_cb0,
	.int_err_cb = iic_err_cb0,
};


static void iic_tx_cb0(void *param)
{
	ss_iic_master_tx_cb(&iic_master_context0, param);
}

static void iic_rx_cb0(void *param)
{
	ss_iic_master_rx_cb(&iic_master_context0, param);
}

static void iic_err_cb0(void *param)
{
	ss_iic_master_err_cb(&iic_master_context0, param);
}

static int32_t dfss_iic_0_open(uint32_t mode, uint32_t param)
{
	(void)mode;
	return ss_iic_master_open(&iic_master_context0, param);
}

static int32_t dfss_iic_0_close(void)
{
	return ss_iic_master_close(&iic_master_context0);
}

static int32_t dfss_iic_0_control(uint32_t ctrl_cmd, void *param)
{
	return ss_iic_master_control(&iic_master_context0, ctrl_cmd, param);
}

static int32_t dfss_iic_0_write(const void *data, uint32_t len)
{
	return ss_iic_master_write(&iic_master_context0, data, len);
}

static int32_t dfss_iic_0_read(void *data, uint32_t len)
{
	return ss_iic_master_read(&iic_master_context0, data, len);
}

static void dfss_iic_0_install(void)
{
	DEV_IIC *dfss_iic_ptr = &dfss_iic_0;
	DEV_IIC_INFO *dfss_iic_info_ptr = &(dfss_iic_0.iic_info);


	iic_master_context0.info = &(dfss_iic_0.iic_info);
	/* Info init */
	dfss_iic_info_ptr->iic_ctrl = (void *)&iic_master_context0;
	dfss_iic_info_ptr->opn_cnt = 0;
	dfss_iic_info_ptr->addr_mode = IIC_7BIT_ADDRESS;
	dfss_iic_info_ptr->tar_addr = DFSS_IIC_0_TARADDR;

	/** iic dev init */
	dfss_iic_ptr->iic_open = dfss_iic_0_open;
	dfss_iic_ptr->iic_close = dfss_iic_0_close;
	dfss_iic_ptr->iic_control = dfss_iic_0_control;
	dfss_iic_ptr->iic_write = dfss_iic_0_write;
	dfss_iic_ptr->iic_read = dfss_iic_0_read;

}
#endif /* USE_DFSS_IIC_0 */

#if (USE_DFSS_IIC_1)
static void iic_tx_cb1(void *param);
static void iic_rx_cb1(void *param);
static void iic_err_cb1(void *param);

#define DFSS_IIC_1_TARADDR	(0)

static DEV_IIC			dfss_iic_1;

static SS_IIC_MASTER_DEV_CONTEXT iic_master_context1 = {
	.reg_base = AR_IO_I2C_MST1_CON,
	.dev_id = 0,
	.int_err = IO_I2C_MST1_INT_ERR,
	.int_rx_avail = IO_I2C_MST1_INT_RX_AVAIL,
	.int_tx_req = IO_I2C_MST1_INT_TX_REQ,
	.int_stop_det = IO_I2C_MST1_INT_STOP_DET,
	.int_rx_cb = iic_rx_cb1,
	.int_tx_cb = iic_tx_cb1,
	.int_err_cb = iic_err_cb1,
};


static void iic_tx_cb1(void *param)
{
	ss_iic_master_tx_cb(&iic_master_context1, param);
}

static void iic_rx_cb1(void *param)
{
	ss_iic_master_rx_cb(&iic_master_context1, param);
}

static void iic_err_cb1(void *param)
{
	ss_iic_master_err_cb(&iic_master_context1, param);
}

static int32_t dfss_iic_1_open(uint32_t mode, uint32_t param)
{
	(void)mode;
	return ss_iic_master_open(&iic_master_context1, param);
}

static int32_t dfss_iic_1_close(void)
{
	return ss_iic_master_close(&iic_master_context1);
}

static int32_t dfss_iic_1_control(uint32_t ctrl_cmd, void *param)
{
	return ss_iic_master_control(&iic_master_context1, ctrl_cmd, param);
}

static int32_t dfss_iic_1_write(const void *data, uint32_t len)
{
	return ss_iic_master_write(&iic_master_context1, data, len);
}

static int32_t dfss_iic_1_read(void *data, uint32_t len)
{
	return ss_iic_master_read(&iic_master_context1, data, len);
}

static void dfss_iic_1_install(void)
{
	DEV_IIC *dfss_iic_ptr = &dfss_iic_1;
	DEV_IIC_INFO *dfss_iic_info_ptr = &(dfss_iic_1.iic_info);


	iic_master_context1.info = &(dfss_iic_1.iic_info);
	/* Info init */
	dfss_iic_info_ptr->iic_ctrl = (void *)&iic_master_context1;
	dfss_iic_info_ptr->opn_cnt = 0;
	dfss_iic_info_ptr->addr_mode = IIC_7BIT_ADDRESS;
	dfss_iic_info_ptr->tar_addr = DFSS_IIC_1_TARADDR;

	/** iic dev init */
	dfss_iic_ptr->iic_open = dfss_iic_1_open;
	dfss_iic_ptr->iic_close = dfss_iic_1_close;
	dfss_iic_ptr->iic_control = dfss_iic_1_control;
	dfss_iic_ptr->iic_write = dfss_iic_1_write;
	dfss_iic_ptr->iic_read = dfss_iic_1_read;

}
#endif /* USE_DFSS_IIC_1 */

DEV_IIC_PTR iic_get_dev(int32_t iic_id)
{
	static uint32_t install_flag = 0;

	/* intall device objects */
	if (install_flag == 0) {
		install_flag = 1;
		dfss_iic_all_install();
	}

	switch (iic_id) {
#if USE_DFSS_IIC_0

		case DFSS_IIC_0_ID:
			return &dfss_iic_0;
			break;
#endif
#if USE_DFSS_IIC_1

		case DFSS_IIC_1_ID:
			return &dfss_iic_1;
			break;
#endif

		default:
			break;
	}

	return NULL;
}

/**
 * \brief	install all iic objects
 * \note	\b MUST be called during system init
 */
void dfss_iic_all_install(void)
{
#if USE_DFSS_IIC_0
	dfss_iic_0_install();
#endif
#if USE_DFSS_IIC_1
	dfss_iic_1_install();
#endif
}
