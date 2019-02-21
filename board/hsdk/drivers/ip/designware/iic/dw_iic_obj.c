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

#include "dw_iic.h"
#include "dw_iic_obj.h"

#include "hsdk/hsdk.h"

#define DW_IIC_CLOCK		HSDC_CLK_SYS_PLL_I2CREF

/**
 * \name	HSDK DesignWare IIC 0 Object Instantiation
 * @{
 */
#if (USE_DW_IIC_0)
static void dw_iic_0_isr(void *ptr);
#define DW_IIC_0_REGBASE	(HSDC_I2C0_REGBASE)	/*!< designware iic 0 relative baseaddr */
#define DW_IIC_0_INTNO		(HSDC_I2C0_ISR)		/*!< designware iic 0 interrupt number  */
#define DW_IIC_0_SLVADDR	(0x55)			/*!< iic 0 slave address working in slave mode */
#define DW_IIC_0_TX_FIFO_LEN	(32)
#define DW_IIC_0_RX_FIFO_LEN	(32)
#define DW_IIC_0_MASTER_CODE	(0)
#define DW_IIC_0_TARADDR	(0x55)
#define DW_IIC_0_CLK		(DW_IIC_CLOCK)
#define DW_IIC_0_IC_CAPLOADING	(DW_IIC_CAP_LOADING_100PF)

DEV_IIC			dw_iic_0;			/*!< designware iic object */
DW_IIC_CTRL		dw_iic_0_ctrl;			/*!< designware iic 0 ctrl */

/** designware iic 0 open */
static int32_t dw_iic_0_open (uint32_t mode, uint32_t param)
{
	return dw_iic_open(&dw_iic_0, mode, param);
}
/** designware iic 0 close */
static int32_t dw_iic_0_close (void)
{
	return dw_iic_close(&dw_iic_0);
}
/** designware iic 0 control */
static int32_t dw_iic_0_control (uint32_t ctrl_cmd, void *param)
{
	return dw_iic_control(&dw_iic_0, ctrl_cmd, param);
}
/** designware iic 0 write */
static int32_t dw_iic_0_write (const void *data, uint32_t len)
{
	return dw_iic_write(&dw_iic_0, data, len);
}
/** designware iic 0 close */
static int32_t dw_iic_0_read (void *data, uint32_t len)
{
	return dw_iic_read(&dw_iic_0, data, len);
}
/** designware iic 0 interrupt routine */
static void dw_iic_0_isr(void *ptr)
{
	dw_iic_isr(&dw_iic_0, ptr);
}
/** install designware iic 0 to system */
static void dw_iic_0_install(void)
{
	uint32_t iic_abs_base = 0;
	DEV_IIC_PTR dw_iic_ptr = &dw_iic_0;
	DEV_IIC_INFO_PTR dw_iic_info_ptr = &(dw_iic_0.iic_info);
	DW_IIC_CTRL_PTR dw_iic_ctrl_ptr = &dw_iic_0_ctrl;
	DW_IIC_REG_PTR dw_iic_reg_ptr;

	/* Info init */
	dw_iic_info_ptr->iic_ctrl = (void *)dw_iic_ctrl_ptr;
	dw_iic_info_ptr->opn_cnt = 0;
	dw_iic_info_ptr->addr_mode = IIC_7BIT_ADDRESS;
	dw_iic_info_ptr->tar_addr = DW_IIC_0_TARADDR;
	/*
	 * get absolute designware base address
	 */
	iic_abs_base = (uint32_t)DW_IIC_0_REGBASE;
	dw_iic_reg_ptr = (DW_IIC_REG_PTR)iic_abs_base;

	/* iic ctrl init */
	dw_iic_ctrl_ptr->dw_iic_regs = dw_iic_reg_ptr;
	/* Variables which should be set during object implementation */
	dw_iic_ctrl_ptr->ic_clkhz = DW_IIC_0_CLK;
	dw_iic_ctrl_ptr->ic_caploading= DW_IIC_0_IC_CAPLOADING;
	dw_iic_ctrl_ptr->support_modes = DW_IIC_BOTH_SUPPORTED;
	dw_iic_ctrl_ptr->tx_fifo_len = DW_IIC_0_TX_FIFO_LEN;
	dw_iic_ctrl_ptr->rx_fifo_len = DW_IIC_0_RX_FIFO_LEN;
	dw_iic_ctrl_ptr->iic_master_code = DW_IIC_0_MASTER_CODE;
	dw_iic_ctrl_ptr->retry_cnt = DW_IIC_MAX_RETRY_COUNT;
	dw_iic_ctrl_ptr->intno = DW_IIC_0_INTNO;
	dw_iic_ctrl_ptr->dw_iic_int_handler = dw_iic_0_isr;
	/* Variables which always change during iic operation */
	dw_iic_ctrl_ptr->int_status = 0;
	dw_iic_ctrl_ptr->iic_tx_over = 0;
	dw_iic_ctrl_ptr->iic_rx_over = 0;

	/** iic dev init */
	dw_iic_ptr->iic_open = dw_iic_0_open;
	dw_iic_ptr->iic_close = dw_iic_0_close;
	dw_iic_ptr->iic_control = dw_iic_0_control;
	dw_iic_ptr->iic_write = dw_iic_0_write;
	dw_iic_ptr->iic_read = dw_iic_0_read;
}
#endif /* USE_DW_IIC_0 */
/** @} end of name */

/**
 * \name	HSDK DesignWare IIC 1 Object Instantiation
 * @{
 */
#if (USE_DW_IIC_1)
static void dw_iic_1_isr(void *ptr);
#define DW_IIC_1_REGBASE	(HSDC_I2C1_REGBASE)	/*!< designware iic 1 relative baseaddr */
#define DW_IIC_1_INTNO		(HSDC_I2C1_ISR)		/*!< designware iic 1 interrupt number  */
#define DW_IIC_1_SLVADDR	(0x56)			/*!< iic 1 slave address working in slave mode */
#define DW_IIC_1_TX_FIFO_LEN	(32)
#define DW_IIC_1_RX_FIFO_LEN	(32)
#define DW_IIC_1_MASTER_CODE	(1)
#define DW_IIC_1_TARADDR	(0x56)
#define DW_IIC_1_CLK		(DW_IIC_CLOCK)
#define DW_IIC_1_IC_CAPLOADING	(DW_IIC_CAP_LOADING_100PF)

DEV_IIC		dw_iic_1;				/*!< designware iic 1 object */
DW_IIC_CTRL 	dw_iic_1_ctrl;				/*!< designware iic 1 ctrl */

/** designware iic 1 open */
static int32_t dw_iic_1_open (uint32_t mode, uint32_t param)
{
	return dw_iic_open(&dw_iic_1, mode, param);
}
/** designware iic 1 close */
static int32_t dw_iic_1_close (void)
{
	return dw_iic_close(&dw_iic_1);
}
/** designware iic 1 control */
static int32_t dw_iic_1_control (uint32_t ctrl_cmd, void *param)
{
	return dw_iic_control(&dw_iic_1, ctrl_cmd, param);
}
/** designware iic 1 write */
static int32_t dw_iic_1_write (const void *data, uint32_t len)
{
	return dw_iic_write(&dw_iic_1, data, len);
}
/** designware iic 1 close */
static int32_t dw_iic_1_read (void *data, uint32_t len)
{
	return dw_iic_read(&dw_iic_1, data, len);
}
/** designware iic 1 interrupt routine */
static void dw_iic_1_isr(void *ptr)
{
	dw_iic_isr(&dw_iic_1, ptr);
}
/** install designware iic 1 to system */
static void dw_iic_1_install(void)
{
	uint32_t iic_abs_base = 0;
	DEV_IIC_PTR dw_iic_ptr = &dw_iic_1;
	DEV_IIC_INFO_PTR dw_iic_info_ptr = &(dw_iic_1.iic_info);
	DW_IIC_CTRL_PTR dw_iic_ctrl_ptr = &dw_iic_1_ctrl;
	DW_IIC_REG_PTR dw_iic_reg_ptr;

	/* Info init */
	dw_iic_info_ptr->iic_ctrl = (void *)dw_iic_ctrl_ptr;
	dw_iic_info_ptr->opn_cnt = 0;
	dw_iic_info_ptr->addr_mode = IIC_7BIT_ADDRESS;
	dw_iic_info_ptr->tar_addr = DW_IIC_1_TARADDR;
	/*
	 * get absolute designware base address
	 */
	iic_abs_base = (uint32_t)DW_IIC_1_REGBASE;
	dw_iic_reg_ptr = (DW_IIC_REG_PTR)iic_abs_base;

	/* iic ctrl init */
	dw_iic_ctrl_ptr->dw_iic_regs = dw_iic_reg_ptr;
	/* Variables which should be set during object implementation */
	dw_iic_ctrl_ptr->ic_clkhz = DW_IIC_1_CLK;
	dw_iic_ctrl_ptr->ic_caploading= DW_IIC_1_IC_CAPLOADING;
	dw_iic_ctrl_ptr->support_modes = DW_IIC_BOTH_SUPPORTED;
	dw_iic_ctrl_ptr->tx_fifo_len = DW_IIC_1_TX_FIFO_LEN;
	dw_iic_ctrl_ptr->rx_fifo_len = DW_IIC_1_RX_FIFO_LEN;
	dw_iic_ctrl_ptr->iic_master_code = DW_IIC_1_MASTER_CODE;
	dw_iic_ctrl_ptr->retry_cnt = DW_IIC_MAX_RETRY_COUNT;
	dw_iic_ctrl_ptr->intno = DW_IIC_1_INTNO;
	dw_iic_ctrl_ptr->dw_iic_int_handler = dw_iic_1_isr;
	/* Variables which always change during iic operation */
	dw_iic_ctrl_ptr->int_status = 0;
	dw_iic_ctrl_ptr->iic_tx_over = 0;
	dw_iic_ctrl_ptr->iic_rx_over = 0;

	/** iic dev init */
	dw_iic_ptr->iic_open = dw_iic_1_open;
	dw_iic_ptr->iic_close = dw_iic_1_close;
	dw_iic_ptr->iic_control = dw_iic_1_control;
	dw_iic_ptr->iic_write = dw_iic_1_write;
	dw_iic_ptr->iic_read = dw_iic_1_read;
}
#endif /* USE_DW_IIC_1 */
/** @} end of name */

/**
 * \name	HSDK DesignWare IIC 2 Object Instantiation
 * @{
 */
#if (USE_DW_IIC_2)
static void dw_iic_2_isr(void *ptr);
#define DW_IIC_2_REGBASE	(HSDC_I2C2_REGBASE)	/*!< designware iic 2 relative baseaddr */
#define DW_IIC_2_INTNO		(HSDC_I2C2_ISR)		/*!< designware iic 2 interrupt number  */
#define DW_IIC_2_SLVADDR	(0x57)			/*!< iic 2 slave address working in slave mode */
#define DW_IIC_2_TX_FIFO_LEN	(32)
#define DW_IIC_2_RX_FIFO_LEN	(32)
#define DW_IIC_2_MASTER_CODE	(1)
#define DW_IIC_2_TARADDR	(0x57)
#define DW_IIC_2_CLK		(DW_IIC_CLOCK)
#define DW_IIC_2_IC_CAPLOADING	(DW_IIC_CAP_LOADING_100PF)

DEV_IIC		dw_iic_2;				/*!< designware iic 2 object */
DW_IIC_CTRL 	dw_iic_2_ctrl;				/*!< designware iic 2 ctrl */

/** designware iic 2 open */
static int32_t dw_iic_2_open (uint32_t mode, uint32_t param)
{
	return dw_iic_open(&dw_iic_2, mode, param);
}
/** designware iic 2 close */
static int32_t dw_iic_2_close (void)
{
	return dw_iic_close(&dw_iic_2);
}
/** designware iic 2 control */
static int32_t dw_iic_2_control (uint32_t ctrl_cmd, void *param)
{
	return dw_iic_control(&dw_iic_2, ctrl_cmd, param);
}
/** designware iic 2 write */
static int32_t dw_iic_2_write (const void *data, uint32_t len)
{
	return dw_iic_write(&dw_iic_2, data, len);
}
/** designware iic 2 close */
static int32_t dw_iic_2_read (void *data, uint32_t len)
{
	return dw_iic_read(&dw_iic_2, data, len);
}
/** designware iic 2 interrupt routine */
static void dw_iic_2_isr(void *ptr)
{
	dw_iic_isr(&dw_iic_2, ptr);
}
/** install designware iic 2 to system */
static void dw_iic_2_install(void)
{
	uint32_t iic_abs_base = 0;
	DEV_IIC_PTR dw_iic_ptr = &dw_iic_2;
	DEV_IIC_INFO_PTR dw_iic_info_ptr = &(dw_iic_2.iic_info);
	DW_IIC_CTRL_PTR dw_iic_ctrl_ptr = &dw_iic_2_ctrl;
	DW_IIC_REG_PTR dw_iic_reg_ptr;

	/* Info init */
	dw_iic_info_ptr->iic_ctrl = (void *)dw_iic_ctrl_ptr;
	dw_iic_info_ptr->opn_cnt = 0;
	dw_iic_info_ptr->addr_mode = IIC_7BIT_ADDRESS;
	dw_iic_info_ptr->tar_addr = DW_IIC_2_TARADDR;
	/*
	 * get absolute designware base address
	 */
	iic_abs_base = (uint32_t)DW_IIC_2_REGBASE;
	dw_iic_reg_ptr = (DW_IIC_REG_PTR)iic_abs_base;

	/* iic ctrl init */
	dw_iic_ctrl_ptr->dw_iic_regs = dw_iic_reg_ptr;
	/* Variables which should be set during object implementation */
	dw_iic_ctrl_ptr->ic_clkhz = DW_IIC_2_CLK;
	dw_iic_ctrl_ptr->ic_caploading= DW_IIC_2_IC_CAPLOADING;
	dw_iic_ctrl_ptr->support_modes = DW_IIC_BOTH_SUPPORTED;
	dw_iic_ctrl_ptr->tx_fifo_len = DW_IIC_2_TX_FIFO_LEN;
	dw_iic_ctrl_ptr->rx_fifo_len = DW_IIC_2_RX_FIFO_LEN;
	dw_iic_ctrl_ptr->iic_master_code = DW_IIC_2_MASTER_CODE;
	dw_iic_ctrl_ptr->retry_cnt = DW_IIC_MAX_RETRY_COUNT;
	dw_iic_ctrl_ptr->intno = DW_IIC_2_INTNO;
	dw_iic_ctrl_ptr->dw_iic_int_handler = dw_iic_2_isr;
	/* Variables which always change during iic operation */
	dw_iic_ctrl_ptr->int_status = 0;
	dw_iic_ctrl_ptr->iic_tx_over = 0;
	dw_iic_ctrl_ptr->iic_rx_over = 0;

	/** iic dev init */
	dw_iic_ptr->iic_open = dw_iic_2_open;
	dw_iic_ptr->iic_close = dw_iic_2_close;
	dw_iic_ptr->iic_control = dw_iic_2_control;
	dw_iic_ptr->iic_write = dw_iic_2_write;
	dw_iic_ptr->iic_read = dw_iic_2_read;
}
#endif /* USE_DW_IIC_2 */
/** @} end of name */

/** get one designware device structure */
DEV_IIC_PTR iic_get_dev(int32_t iic_id)
{
	static uint32_t install_flag = 0;

	/* intall device objects */
	if (install_flag == 0) {
		install_flag = 1;
		dw_iic_all_install();
	}

	switch (iic_id) {
#if (USE_DW_IIC_0)
		case DW_IIC_0_ID:
			return &dw_iic_0;
			break;
#endif
#if (USE_DW_IIC_1)
		case DW_IIC_1_ID:
			return &dw_iic_1;
			break;
#endif
#if (USE_DW_IIC_2)
		case DW_IIC_2_ID:
			return &dw_iic_2;
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
void dw_iic_all_install(void)
{
#if (USE_DW_IIC_0)
	dw_iic_0_install();
#endif
#if (USE_DW_IIC_1)
	dw_iic_1_install();
#endif
#if (USE_DW_IIC_2)
	dw_iic_2_install();
#endif
}

/** @} end of group BOARD_HSDK_DRV_DW_IIC_OBJ */
