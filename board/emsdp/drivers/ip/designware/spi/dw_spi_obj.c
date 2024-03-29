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
#include "dw_spi_obj.h"
#include "emsdp/emsdp.h"

#if (USE_DW_SPI_0)
static void dw_spi_0_isr(void *ptr);
#define DW_SPI_0_BASE           (EMSDP_SPI_BASE)                /*!< designware spi 0 relative baseaddr */
#define DW_SPI_0_INTNO          (EMSDP_REDPINE_SPI_INTR)        /*!< designware spi 0 interrupt number  */
#define DW_SPI_0_RX_SAMPLEDLY   0                               /*!< designware spi 0 RXD Sample Delay  */

#define DW_SPI_0_TX_FIFO_LEN    (32)
#define DW_SPI_0_RX_FIFO_LEN    (32)

#define DW_SPI_0_SUPPORTED_MODE DW_SPI_MASTER_SUPPORTED

static DEV_SPI dw_spi_0;                                        /*!< designware spi object */
static DW_SPI_CTRL dw_spi_0_ctrl;                               /*!< designware spi 0 ctrl */
static uint32_t dw_spi_0_cs_status;

static int32_t dw_spi_0_open(uint32_t mode, uint32_t param)
{
	return dw_spi_open(&dw_spi_0, mode, param);
}

static int32_t dw_spi_0_close(void)
{
	return dw_spi_close(&dw_spi_0);
}

static int32_t dw_spi_0_control(uint32_t ctrl_cmd, void *param)
{
	int32_t ercd;

	ercd = dw_spi_control(&dw_spi_0, ctrl_cmd, param);

	if (ctrl_cmd == SPI_CMD_MST_SEL_DEV) {
		dw_spi_0_cs_status = cpu_lock_save();
	} else if (ctrl_cmd == SPI_CMD_MST_DSEL_DEV) {
		cpu_unlock_restore(dw_spi_0_cs_status);
	}

	return ercd;
}

static int32_t dw_spi_0_write(const void *data, uint32_t len)
{
	return dw_spi_write(&dw_spi_0, data, len);
}

static int32_t dw_spi_0_read(void *data, uint32_t len)
{
	return dw_spi_read(&dw_spi_0, data, len);
}

static void dw_spi_0_isr(void *ptr)
{
	dw_spi_isr(&dw_spi_0, ptr);
}

static void dw_spi_0_install(void)
{
	uint32_t spi_abs_base = 0;
	DEV_SPI *dw_spi_ptr = &dw_spi_0;
	DEV_SPI_INFO *dw_spi_info_ptr = &(dw_spi_0.spi_info);
	DW_SPI_CTRL *dw_spi_ctrl_ptr = &dw_spi_0_ctrl;
	DW_SPI_REG *dw_spi_reg_ptr;

	/**
	 * get absolute designware base address
	 */
	spi_abs_base = (uint32_t)DW_SPI_0_BASE;

	dw_spi_reg_ptr = (DW_SPI_REG *)spi_abs_base;

	/** spi info init */
	dw_spi_info_ptr->spi_ctrl = (void *)dw_spi_ctrl_ptr;
	dw_spi_info_ptr->status = DEV_DISABLED;
	dw_spi_info_ptr->freq = 1000;
	dw_spi_info_ptr->opn_cnt = 0;
	dw_spi_info_ptr->mode = DEV_MASTER_MODE;
	dw_spi_info_ptr->clk_mode = SPI_CLK_MODE_DEFAULT;
	dw_spi_info_ptr->slave = SPI_SLAVE_NOT_SELECTED;
	dw_spi_info_ptr->dfs = SPI_DFS_DEFAULT;
	dw_spi_info_ptr->dummy = 0xff;

	/* spi ctrl init */
	/* Variables which should be set during object implementation */
	dw_spi_ctrl_ptr->dw_spi_regs = dw_spi_reg_ptr;
	dw_spi_ctrl_ptr->intno = DW_SPI_0_INTNO;
	dw_spi_ctrl_ptr->support_modes = DW_SPI_0_SUPPORTED_MODE;
	dw_spi_ctrl_ptr->tx_fifo_len = DW_SPI_0_TX_FIFO_LEN;
	dw_spi_ctrl_ptr->rx_fifo_len = DW_SPI_0_TX_FIFO_LEN;
	dw_spi_ctrl_ptr->dw_apb_bus_freq = CLK_BUS_APB;
	dw_spi_ctrl_ptr->rx_sampledly = DW_SPI_0_RX_SAMPLEDLY;
	dw_spi_ctrl_ptr->dw_spi_int_handler = dw_spi_0_isr;
	/* Variables which always change during spi operation */
	dw_spi_ctrl_ptr->int_status = 0;

	/** spi dev init */
	dw_spi_ptr->spi_open = dw_spi_0_open;
	dw_spi_ptr->spi_close = dw_spi_0_close;
	dw_spi_ptr->spi_control = dw_spi_0_control;
	dw_spi_ptr->spi_write = dw_spi_0_write;
	dw_spi_ptr->spi_read = dw_spi_0_read;
}
#endif /* USE_DW_SPI_0 */

#if (USE_DW_SPI_1)
static void dw_spi_1_isr(void *ptr);
#define DW_SPI_1_BASE           (EMSDP_FLASH_BASE)      /*!< designware spi 1 relative baseaddr */
#define DW_SPI_1_INTNO          (EMSDP_SPI1_INTR)       /*!< designware spi 1 interrupt number  */
#define DW_SPI_1_RX_SAMPLEDLY   0                       /*!< designware spi 1 RXD Sample Delay  */

#define DW_SPI_1_TX_FIFO_LEN    (32)
#define DW_SPI_1_RX_FIFO_LEN    (32)

#define DW_SPI_1_SUPPORTED_MODE DW_SPI_MASTER_SUPPORTED

static DEV_SPI dw_spi_1;                                        /*!< designware spi 1 object */
static DW_SPI_CTRL dw_spi_1_ctrl;                               /*!< designware spi 1 ctrl */
static uint32_t dw_spi_1_cs_status;

static int32_t dw_spi_1_open(uint32_t mode, uint32_t param)
{
	return dw_spi_open(&dw_spi_1, mode, param);
}

static int32_t dw_spi_1_close(void)
{
	return dw_spi_close(&dw_spi_1);
}

static int32_t dw_spi_1_control(uint32_t ctrl_cmd, void *param)
{
	int32_t ercd;

	ercd = dw_spi_control(&dw_spi_1, ctrl_cmd, param);

	if (ctrl_cmd == SPI_CMD_MST_SEL_DEV) {
		dw_spi_1_cs_status = cpu_lock_save();
	} else if (ctrl_cmd == SPI_CMD_MST_DSEL_DEV) {
		cpu_unlock_restore(dw_spi_1_cs_status);
	}

	return ercd;
}

static int32_t dw_spi_1_write(const void *data, uint32_t len)
{
	return dw_spi_write(&dw_spi_1, data, len);
}

static int32_t dw_spi_1_read(void *data, uint32_t len)
{
	return dw_spi_read(&dw_spi_1, data, len);
}

static void dw_spi_1_isr(void *ptr)
{
	dw_spi_isr(&dw_spi_1, ptr);
}

static void dw_spi_1_install(void)
{
	uint32_t spi_abs_base = 0;
	DEV_SPI *dw_spi_ptr = &dw_spi_1;
	DEV_SPI_INFO *dw_spi_info_ptr = &(dw_spi_1.spi_info);
	DW_SPI_CTRL *dw_spi_ctrl_ptr = &dw_spi_1_ctrl;
	DW_SPI_REG *dw_spi_reg_ptr;

	/**
	 * get absolute designware base address
	 */
	spi_abs_base = (uint32_t)DW_SPI_1_BASE;
	dw_spi_reg_ptr = (DW_SPI_REG *)spi_abs_base;

	/** spi info init */
	dw_spi_info_ptr->spi_ctrl = (void *)dw_spi_ctrl_ptr;
	dw_spi_info_ptr->status = DEV_DISABLED;
	dw_spi_info_ptr->freq = 1000;
	dw_spi_info_ptr->opn_cnt = 0;
	dw_spi_info_ptr->mode = DEV_MASTER_MODE;
	dw_spi_info_ptr->clk_mode = SPI_CLK_MODE_DEFAULT;
	dw_spi_info_ptr->slave = SPI_SLAVE_NOT_SELECTED;
	dw_spi_info_ptr->dfs = SPI_DFS_DEFAULT;
	dw_spi_info_ptr->dummy = 0xff;

	/* spi ctrl init */
	/* Variables which should be set during object implementation */
	dw_spi_ctrl_ptr->dw_spi_regs = dw_spi_reg_ptr;
	dw_spi_ctrl_ptr->intno = DW_SPI_1_INTNO;
	dw_spi_ctrl_ptr->support_modes = DW_SPI_1_SUPPORTED_MODE;
	dw_spi_ctrl_ptr->tx_fifo_len = DW_SPI_1_TX_FIFO_LEN;
	dw_spi_ctrl_ptr->rx_fifo_len = DW_SPI_1_TX_FIFO_LEN;
	dw_spi_ctrl_ptr->rx_sampledly = DW_SPI_1_RX_SAMPLEDLY;
	dw_spi_ctrl_ptr->dw_apb_bus_freq = CLK_BUS_APB;
	dw_spi_ctrl_ptr->dw_spi_int_handler = dw_spi_1_isr;
	/* Variables which always change during spi operation */
	dw_spi_ctrl_ptr->int_status = 0;

	/** spi dev init */
	dw_spi_ptr->spi_open = dw_spi_1_open;
	dw_spi_ptr->spi_close = dw_spi_1_close;
	dw_spi_ptr->spi_control = dw_spi_1_control;
	dw_spi_ptr->spi_write = dw_spi_1_write;
	dw_spi_ptr->spi_read = dw_spi_1_read;
}
#endif /* USE_DW_SPI_1 */

/** get one designware device structure */
DEV_SPI_PTR dw_spi_get_dev(int32_t spi_id)
{
	static uint32_t install_flag = 0;

	/* install device objects */
	if (install_flag == 0) {
		install_flag = 1;
		dw_spi_all_install();
	}

	switch (spi_id) {
#if (USE_DW_SPI_0)
	case DW_SPI_0_ID:
		return &dw_spi_0;
		break;
#endif
#if (USE_DW_SPI_1)
	case DW_SPI_1_ID:
		return &dw_spi_1;
		break;
#endif
	default:
		break;
	}
	return NULL;
}

/**
 * @brief	install all spi objects
 * @note	@b MUST be called during system init
 */
void dw_spi_all_install(void)
{
#if (USE_DW_SPI_0)
	dw_spi_0_install();
#endif
#if (USE_DW_SPI_1)
	dw_spi_1_install();
#endif
}
