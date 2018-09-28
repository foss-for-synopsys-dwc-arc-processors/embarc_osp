/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:

 * 1) Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 * with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */
#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "iotdk_hardware.h"

#include "dfss_spi_obj.h"
#include "ip/subsystem/spi/ss_spi_master.h"
#include "ip/subsystem/spi/ss_spi_slave.h"


#if (USE_DFSS_SPI_0)
static void spi_tx_cb0(void *param);
static void spi_rx_cb0(void *param);
static void spi_err_cb0(void *param);

static DEV_SPI dfss_spi_0;

static SS_SPI_MASTER_DEV_CONTEXT spi_master_context0 = {
	AR_IO_SPI_MST0_CTRLR0,
	DFSS_SPI_0_ID,
	IO_SPI_MST0_INT_RX_AVAIL,
	IO_SPI_MST0_INT_TX_REQ,
	IO_SPI_MST0_INT_IDLE,
	IO_SPI_MST0_INT_ERR,
	BOARD_SPI_CLK,
	0,
	spi_rx_cb0,
	spi_tx_cb0,
	spi_err_cb0,
};

static void spi_tx_cb0(void *param)
{
	ss_spi_master_tx_cb(&spi_master_context0, param);
}

static void spi_rx_cb0(void *param)
{
	ss_spi_master_rx_cb(&spi_master_context0, param);
}

static void spi_err_cb0(void *param)
{
	ss_spi_master_err_cb(&spi_master_context0, param);
}

static int32_t dfss_spi_0_open(uint32_t mode, uint32_t param)
{
	return ss_spi_master_open(&spi_master_context0, mode, param);
}

static int32_t dfss_spi_0_close (void)
{
	return ss_spi_master_close(&spi_master_context0);
}

static int32_t dfss_spi_0_control(uint32_t ctrl_cmd, void *param)
{
	return ss_spi_master_control(&spi_master_context0, ctrl_cmd, param);
}

static int32_t dfss_spi_0_write(const void *data, uint32_t len)
{
	return ss_spi_master_write(&spi_master_context0, data, len);
}

static int32_t dfss_spi_0_read(void *data, uint32_t len)
{
	return ss_spi_master_read(&spi_master_context0, data, len);
}

static void dfss_spi_0_install(void)
{
	DEV_SPI *dfss_spi_ptr = &dfss_spi_0;
	DEV_SPI_INFO *info = &(dfss_spi_0.spi_info);

	spi_master_context0.info = info;
	/** spi info init */
	info->status = DEV_DISABLED;
	info->freq = 1000000;
	info->opn_cnt = 0;
	info->mode = DEV_MASTER_MODE;
	info->clk_mode = SPI_CLK_MODE_DEFAULT;
	info->slave = SPI_SLAVE_NOT_SELECTED;
	info->dfs = SPI_DFS_DEFAULT;
	info->dummy = 0xff;
	info->spi_ctrl = (void *)&spi_master_context0;

	/** spi dev init */
	dfss_spi_ptr->spi_open = dfss_spi_0_open;
	dfss_spi_ptr->spi_close = dfss_spi_0_close;
	dfss_spi_ptr->spi_control = dfss_spi_0_control;
	dfss_spi_ptr->spi_write = dfss_spi_0_write;
	dfss_spi_ptr->spi_read = dfss_spi_0_read;
}
#endif /* USE_DFSS_SPI_0 */

#if (USE_DFSS_SPI_1)

static void spi_tx_cb1(void *param);
static void spi_rx_cb1(void *param);
static void spi_err_cb1(void *param);

static DEV_SPI dfss_spi_1;

static SS_SPI_MASTER_DEV_CONTEXT spi_master_context1 = {
	AR_IO_SPI_MST1_CTRLR0,
	DFSS_SPI_1_ID,
	IO_SPI_MST1_INT_RX_AVAIL,
	IO_SPI_MST1_INT_TX_REQ,
	IO_SPI_MST1_INT_IDLE,
	IO_SPI_MST1_INT_ERR,
	BOARD_SPI_CLK,
	0,
	spi_rx_cb1,
	spi_tx_cb1,
	spi_err_cb1,
};

static void spi_tx_cb1(void *param)
{
	ss_spi_master_tx_cb(&spi_master_context1, param);
}

static void spi_rx_cb1(void *param)
{
	ss_spi_master_rx_cb(&spi_master_context1, param);
}

static void spi_err_cb1(void *param)
{
	ss_spi_master_err_cb(&spi_master_context1, param);
}

static int32_t dfss_spi_1_open(uint32_t mode, uint32_t param)
{
	return ss_spi_master_open(&spi_master_context1, mode, param);
}

static int32_t dfss_spi_1_close(void)
{
	return ss_spi_master_close(&spi_master_context1);
}

static int32_t dfss_spi_1_control(uint32_t ctrl_cmd, void *param)
{
	return ss_spi_master_control(&spi_master_context1, ctrl_cmd, param);
}

static int32_t dfss_spi_1_write(const void *data, uint32_t len)
{
	return ss_spi_master_write(&spi_master_context1, data, len);
}

static int32_t dfss_spi_1_read(void *data, uint32_t len)
{
	return ss_spi_master_read(&spi_master_context1, data, len);
}

static void dfss_spi_1_install(void)
{
	DEV_SPI *dfss_spi_ptr = &dfss_spi_1;
	DEV_SPI_INFO *info = &(dfss_spi_1.spi_info);

	spi_master_context1.info = info;
	/** spi info init */
	info->status = DEV_DISABLED;
	info->freq = 1000000;
	info->opn_cnt = 0;
	info->mode = DEV_MASTER_MODE;
	info->clk_mode = SPI_CLK_MODE_DEFAULT;
	info->slave = SPI_SLAVE_NOT_SELECTED;
	info->dfs = SPI_DFS_DEFAULT;
	info->dummy = 0xff;
	info->spi_ctrl = (void *)&spi_master_context1;

	/** spi dev init */
	dfss_spi_ptr->spi_open = dfss_spi_1_open;
	dfss_spi_ptr->spi_close = dfss_spi_1_close;
	dfss_spi_ptr->spi_control = dfss_spi_1_control;
	dfss_spi_ptr->spi_write = dfss_spi_1_write;
	dfss_spi_ptr->spi_read = dfss_spi_1_read;
}

#endif /* USE_DFSS_SPI_1 */

DEV_SPI_PTR spi_get_dev(int32_t spi_id)
{
	static uint32_t install_flag = 0;

	/* install device objects */
	if (install_flag == 0) {
		install_flag = 1;
		dfss_spi_all_install();
	}

	switch (spi_id) {
#if (USE_DFSS_SPI_0)

		case DFSS_SPI_0_ID:
			return &dfss_spi_0;
			break;
#endif

#if (USE_DFSS_SPI_1)

		case DFSS_SPI_1_ID:
			return &dfss_spi_1;
			break;
#endif

		default:
			break;
	}

	return NULL;
}

/**
 * \brief	install all spi objects
 * \note	\b MUST be called during system init
 */
void dfss_spi_all_install(void)
{
#if (USE_DFSS_SPI_0)
	dfss_spi_0_install();
#endif

#if (USE_DFSS_SPI_1)
	dfss_spi_1_install();
#endif
}