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
#include "arc_exception.h"

#include "dw_spi.h"
#include "dw_spi_obj.h"

#include "hsdk/hsdk.h"

#define DW_SPI_DEFAULT_FREQ		1000000
/**
 * \name	HSDK DesignWare SPI 0 Object Instantiation
 * @{
 */
#if (USE_DW_SPI_0)
static void dw_spi_0_isr(void *ptr);
#define DW_SPI_0_REGBASE	(HSDC_SPI0_REGBASE)		/*!< designware spi 0 relative baseaddr */
#define DW_SPI_0_INTNO		(HSDC_SPI0_ISR)			/*!< designware spi 0 interrupt number  */
#define DW_SPI_0_CLK		(HSDC_CLK_SYS_PLL_SPIREF)	/*!< designware spi 0 clock  */
#define DW_SPI_0_RX_SAMPLEDLY	0				/*!< designware spi 0 RXD Sample Delay  */

#define DW_SPI_0_TX_FIFO_LEN	(32)
#define DW_SPI_0_RX_FIFO_LEN	(32)

#define DW_SPI_0_SUPPORTED_MODE	DW_SPI_MASTER_SUPPORTED

DEV_SPI			dw_spi_0;			/*!< designware spi object */
DW_SPI_CTRL		dw_spi_0_ctrl;			/*!< designware spi 0 ctrl */

/** designware spi 0 open */
static int32_t dw_spi_0_open (uint32_t mode, uint32_t param)
{
	return dw_spi_open(&dw_spi_0, mode, param);
}
/** designware spi 0 close */
static int32_t dw_spi_0_close (void)
{
	return dw_spi_close(&dw_spi_0);
}
/** designware spi 0 control */
static int32_t dw_spi_0_control (uint32_t ctrl_cmd, void *param)
{
	switch (ctrl_cmd) {
		case SPI_CMD_MST_SEL_DEV:
			creg_hsdc_set_spi_cs_ctrl((CREG_HSDC_STRUCT_PTR)(HSDC_CREG_REGBASE), \
				CREG_HSDC_SPISEL_0, (CREG_HSDC_SPICSSEL_T)param, CREG_HSDC_SPICSCTRL_CSBYCREGVALUE0);
			break;
		case SPI_CMD_MST_DSEL_DEV:
			creg_hsdc_set_spi_cs_ctrl((CREG_HSDC_STRUCT_PTR)(HSDC_CREG_REGBASE), \
				CREG_HSDC_SPISEL_0, (CREG_HSDC_SPICSSEL_T)param, CREG_HSDC_SPICSCTRL_CSBYCREGVALUE1);
			break;
	}
	return dw_spi_control(&dw_spi_0, ctrl_cmd, param);
}
/** designware spi 0 write */
static int32_t dw_spi_0_write (const void *data, uint32_t len)
{
	return dw_spi_write(&dw_spi_0, data, len);
}
/** designware spi 0 close */
static int32_t dw_spi_0_read (void *data, uint32_t len)
{
	return dw_spi_read(&dw_spi_0, data, len);
}
/** designware spi 0 interrupt routine */
static void dw_spi_0_isr(void *ptr)
{
	dw_spi_isr(&dw_spi_0, ptr);
}
/** install designware spi 0 to system */
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
	spi_abs_base = DW_SPI_0_REGBASE;

	dw_spi_reg_ptr = (DW_SPI_REG *)spi_abs_base;

	/** spi info init */
	dw_spi_info_ptr->spi_ctrl = (void *)dw_spi_ctrl_ptr;
	dw_spi_info_ptr->status = DEV_DISABLED;
	dw_spi_info_ptr->freq = DW_SPI_DEFAULT_FREQ;
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
	dw_spi_ctrl_ptr->dw_apb_bus_freq = DW_SPI_0_CLK;
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
/** @} end of name */

/**
 * \name	HSDK DesignWare SPI 1 Object Instantiation
 * @{
 */
#if (USE_DW_SPI_1)
static void dw_spi_1_isr(void *ptr);
#define DW_SPI_1_REGBASE	(HSDC_SPI1_REGBASE)		/*!< designware spi 1 relative baseaddr */
#define DW_SPI_1_INTNO		(HSDC_SPI1_ISR)			/*!< designware spi 1 interrupt number  */
#define DW_SPI_1_CLK		(HSDC_CLK_SYS_PLL_SPIREF)	/*!< designware spi 1 clock  */
#define DW_SPI_1_RX_SAMPLEDLY	0				/*!< designware spi 1 RXD Sample Delay  */

#define DW_SPI_1_TX_FIFO_LEN	(32)
#define DW_SPI_1_RX_FIFO_LEN	(32)

#define DW_SPI_1_SUPPORTED_MODE	DW_SPI_MASTER_SUPPORTED

DEV_SPI			dw_spi_1;			/*!< designware spi 1 object */
DW_SPI_CTRL		dw_spi_1_ctrl;			/*!< designware spi 1 ctrl */

/** designware spi 1 open */
static int32_t dw_spi_1_open (uint32_t mode, uint32_t param)
{
	return dw_spi_open(&dw_spi_1, mode, param);
}
/** designware spi 1 close */
static int32_t dw_spi_1_close (void)
{
	return dw_spi_close(&dw_spi_1);
}
/** designware spi 1 control */
static int32_t dw_spi_1_control (uint32_t ctrl_cmd, void *param)
{
	switch (ctrl_cmd) {
		case SPI_CMD_MST_SEL_DEV:
			creg_hsdc_set_spi_cs_ctrl((CREG_HSDC_STRUCT_PTR)(HSDC_CREG_REGBASE), \
				CREG_HSDC_SPISEL_1, (CREG_HSDC_SPICSSEL_T)param, CREG_HSDC_SPICSCTRL_CSBYCREGVALUE0);
			break;
		case SPI_CMD_MST_DSEL_DEV:
			creg_hsdc_set_spi_cs_ctrl((CREG_HSDC_STRUCT_PTR)(HSDC_CREG_REGBASE), \
				CREG_HSDC_SPISEL_1, (CREG_HSDC_SPICSSEL_T)param, CREG_HSDC_SPICSCTRL_CSBYCREGVALUE1);
			break;
	}
	return dw_spi_control(&dw_spi_1, ctrl_cmd, param);
}
/** designware spi 1 write */
static int32_t dw_spi_1_write (const void *data, uint32_t len)
{
	return dw_spi_write(&dw_spi_1, data, len);
}
/** designware spi 1 close */
static int32_t dw_spi_1_read (void *data, uint32_t len)
{
	return dw_spi_read(&dw_spi_1, data, len);
}
/** designware spi 1 interrupt routine */
static void dw_spi_1_isr(void *ptr)
{
	dw_spi_isr(&dw_spi_1, ptr);
}
/** install designware spi 1 to system */
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
	spi_abs_base = (uint32_t)DW_SPI_1_REGBASE;
	dw_spi_reg_ptr = (DW_SPI_REG *)spi_abs_base;

	/** spi info init */
	dw_spi_info_ptr->spi_ctrl = (void *)dw_spi_ctrl_ptr;
	dw_spi_info_ptr->status = DEV_DISABLED;
	dw_spi_info_ptr->freq = DW_SPI_DEFAULT_FREQ;
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
	dw_spi_ctrl_ptr->dw_apb_bus_freq = DW_SPI_1_CLK;
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
/** @} end of name */

/**
 * \name	HSDK DesignWare SPI 2 Object Instantiation
 * @{
 */
#if (USE_DW_SPI_2)
static void dw_spi_2_isr(void *ptr);
#define DW_SPI_2_REGBASE	(HSDC_SPI2_REGBASE)		/*!< designware spi 2 relative baseaddr */
#define DW_SPI_2_INTNO		(HSDC_SPI2_ISR)			/*!< designware spi 2 interrupt number  */
#define DW_SPI_2_CLK		(HSDC_CLK_SYS_PLL_SPIREF)	/*!< designware spi 2 clock  */
#define DW_SPI_2_RX_SAMPLEDLY	0				/*!< designware spi 2 RXD Sample Delay  */

#define DW_SPI_2_TX_FIFO_LEN	(32)
#define DW_SPI_2_RX_FIFO_LEN	(32)

#define DW_SPI_2_SUPPORTED_MODE	DW_SPI_MASTER_SUPPORTED

DEV_SPI			dw_spi_2;			/*!< designware spi 2 object */
DW_SPI_CTRL		dw_spi_2_ctrl;			/*!< designware spi 2 ctrl */

/** designware spi 2 open */
static int32_t dw_spi_2_open (uint32_t mode, uint32_t param)
{
	return dw_spi_open(&dw_spi_2, mode, param);
}
/** designware spi 2 close */
static int32_t dw_spi_2_close (void)
{
	return dw_spi_close(&dw_spi_2);
}
/** designware spi 2 control */
static int32_t dw_spi_2_control (uint32_t ctrl_cmd, void *param)
{
	switch (ctrl_cmd) {
		case SPI_CMD_MST_SEL_DEV:
			creg_hsdc_set_spi_cs_ctrl((CREG_HSDC_STRUCT_PTR)(HSDC_CREG_REGBASE), \
				CREG_HSDC_SPISEL_2, (CREG_HSDC_SPICSSEL_T)param, CREG_HSDC_SPICSCTRL_CSBYCREGVALUE0);
			break;
		case SPI_CMD_MST_DSEL_DEV:
			creg_hsdc_set_spi_cs_ctrl((CREG_HSDC_STRUCT_PTR)(HSDC_CREG_REGBASE), \
				CREG_HSDC_SPISEL_2, (CREG_HSDC_SPICSSEL_T)param, CREG_HSDC_SPICSCTRL_CSBYCREGVALUE1);
			break;
	}
	return dw_spi_control(&dw_spi_2, ctrl_cmd, param);
}
/** designware spi 2 write */
static int32_t dw_spi_2_write (const void *data, uint32_t len)
{
	return dw_spi_write(&dw_spi_2, data, len);
}
/** designware spi 2 close */
static int32_t dw_spi_2_read (void *data, uint32_t len)
{
	return dw_spi_read(&dw_spi_2, data, len);
}
/** designware spi 2 interrupt routine */
static void dw_spi_2_isr(void *ptr)
{
	dw_spi_isr(&dw_spi_2, ptr);
}
/** install designware spi 2 to system */
static void dw_spi_2_install(void)
{
	uint32_t spi_abs_base = 0;
	DEV_SPI *dw_spi_ptr = &dw_spi_2;
	DEV_SPI_INFO *dw_spi_info_ptr = &(dw_spi_2.spi_info);
	DW_SPI_CTRL *dw_spi_ctrl_ptr = &dw_spi_2_ctrl;
	DW_SPI_REG *dw_spi_reg_ptr;

	/**
	 * get absolute designware base address
	 */
	spi_abs_base = (uint32_t)DW_SPI_2_REGBASE;
	dw_spi_reg_ptr = (DW_SPI_REG *)spi_abs_base;

	/** spi info init */
	dw_spi_info_ptr->spi_ctrl = (void *)dw_spi_ctrl_ptr;
	dw_spi_info_ptr->status = DEV_DISABLED;
	dw_spi_info_ptr->freq = DW_SPI_DEFAULT_FREQ;
	dw_spi_info_ptr->opn_cnt = 0;
	dw_spi_info_ptr->mode = DEV_MASTER_MODE;
	dw_spi_info_ptr->clk_mode = SPI_CLK_MODE_DEFAULT;
	dw_spi_info_ptr->slave = SPI_SLAVE_NOT_SELECTED;
	dw_spi_info_ptr->dfs = SPI_DFS_DEFAULT;
	dw_spi_info_ptr->dummy = 0xff;

	/* spi ctrl init */
	/* Variables which should be set during object implementation */
	dw_spi_ctrl_ptr->dw_spi_regs = dw_spi_reg_ptr;
	dw_spi_ctrl_ptr->intno = DW_SPI_2_INTNO;
	dw_spi_ctrl_ptr->support_modes = DW_SPI_2_SUPPORTED_MODE;
	dw_spi_ctrl_ptr->tx_fifo_len = DW_SPI_2_TX_FIFO_LEN;
	dw_spi_ctrl_ptr->rx_fifo_len = DW_SPI_2_TX_FIFO_LEN;
	dw_spi_ctrl_ptr->rx_sampledly = DW_SPI_2_RX_SAMPLEDLY;
	dw_spi_ctrl_ptr->dw_apb_bus_freq = DW_SPI_2_CLK;
	dw_spi_ctrl_ptr->dw_spi_int_handler = dw_spi_2_isr;
	/* Variables which always change during spi operation */
	dw_spi_ctrl_ptr->int_status = 0;

	/** spi dev init */
	dw_spi_ptr->spi_open = dw_spi_2_open;
	dw_spi_ptr->spi_close = dw_spi_2_close;
	dw_spi_ptr->spi_control = dw_spi_2_control;
	dw_spi_ptr->spi_write = dw_spi_2_write;
	dw_spi_ptr->spi_read = dw_spi_2_read;
}
#endif /* USE_DW_SPI_2 */
/** @} end of name */

/** get one designware device structure */
DEV_SPI_PTR spi_get_dev(int32_t spi_id)
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
#if (USE_DW_SPI_2)
		case DW_SPI_2_ID:
			return &dw_spi_2;
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
void dw_spi_all_install(void)
{
#if (USE_DW_SPI_0)
	dw_spi_0_install();
#endif
#if (USE_DW_SPI_1)
	dw_spi_1_install();
#endif
#if (USE_DW_SPI_2)
	dw_spi_2_install();
#endif
}
