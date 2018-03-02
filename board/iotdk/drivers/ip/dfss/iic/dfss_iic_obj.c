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
 * \version 2017.03
 * \date 2017-09-04
 * \author Qiang Gu(Qiang.Gu@synopsys.com)
--------------------------------------------- */

/**
 * \defgroup	BOARD_IOTDK_DRV_DFSS_IIC_OBJ	Huangshan DFSS IIC Object
 * \ingroup	BOARD_IOTDK_DRIVER
 * \brief	Huangshan DFSS IIC Objects
 * \details
 *		Realize the Huangshan DFSS IIC object using DFSS IIC device driver,
 */

/**
 * \file
 * \ingroup	BOARD_IOTDK_DRV_DFSS_IIC_OBJ
 * \brief	DFSS IIC object instantiation on iotdk
 */

/**
 * \addtogroup	BOARD_IOTDK_DRV_DFSS_IIC_OBJ
 * @{
 */
#include "arc.h"
#include "arc_builtin.h"
#include "arc_exception.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "../../iotdk.h"
#include "io_config.h"
#include "apexextensions.h"

#include "dev_iic.h"
#include "dfss_iic.h"
#include "dfss_iic_obj.h"

#define DEBUG
#include "embARC_debug.h"

#define IIC_TX_RX_FIFO_SIZE		16	/* TX/RX FIFO size in hardware */

#if (USE_DFSS_IIC_0)
/**
 * \name	DFSS IIC 0 Object Instantiation
 * @{
 */
static void dfss_iic_0_isr(void *ptr);
#define DFSS_IIC_0_TARADDR		(0)
#define DFSS_IIC_0_MASTER_CODE		(0)
#define DFSS_IIC_0_IC_CAPLOADING	(DFSS_IIC_CAP_LOADING_100PF)

DEV_IIC		dfss_iic_0;		/*!< DFSS IIC object */
DFSS_IIC_CTRL	dfss_iic_0_ctrl;	/*!< DFSS iic 0 ctrl */

/** DFSS IIC 0 open */
static int32_t dfss_iic_0_open (uint32_t mode, uint32_t param)
{
	return dfss_iic_open(&dfss_iic_0, mode, param);
}
/** DFSS IIC 0 close */
static int32_t dfss_iic_0_close (void)
{
	return dfss_iic_close(&dfss_iic_0);
}
/** DFSS IIC 0 control */
static int32_t dfss_iic_0_control (uint32_t ctrl_cmd, void *param)
{
	return dfss_iic_control(&dfss_iic_0, ctrl_cmd, param);
}
/** DFSS IIC 0 write */
static int32_t dfss_iic_0_write (const void *data, uint32_t len)
{
	return dfss_iic_write(&dfss_iic_0, data, len);
}
/** DFSS IIC 0 read */
static int32_t dfss_iic_0_read (void *data, uint32_t len)
{
	return dfss_iic_read(&dfss_iic_0, data, len);
}
/** DFSS iic 0 interrupt routine */
static void dfss_iic_0_isr(void *ptr)
{
	dfss_iic_isr(&dfss_iic_0, ptr);
}
/** install DFSS IIC 0 to system */
static void dfss_iic_0_install(void)
{
	DEV_IIC *dfss_iic_ptr = &dfss_iic_0;
	DEV_IIC_INFO *dfss_iic_info_ptr = &(dfss_iic_0.iic_info);
	DFSS_IIC_CTRL *dfss_iic_ctrl_ptr = &dfss_iic_0_ctrl;

	/* Info init */
	dfss_iic_info_ptr->iic_ctrl = (void *)dfss_iic_ctrl_ptr;
	dfss_iic_info_ptr->opn_cnt = 0;
	dfss_iic_info_ptr->addr_mode = IIC_7BIT_ADDRESS;
	dfss_iic_info_ptr->tar_addr = DFSS_IIC_0_TARADDR;

	/* Variables which should be set during object implementation */
	dfss_iic_ctrl_ptr->dev_id = 0;
	dfss_iic_ctrl_ptr->ic_caploading= DFSS_IIC_0_IC_CAPLOADING;
	dfss_iic_ctrl_ptr->support_modes = DFSS_IIC_BOTH_SUPPORTED;
	dfss_iic_ctrl_ptr->tx_fifo_len = IIC_TX_RX_FIFO_SIZE;
	dfss_iic_ctrl_ptr->rx_fifo_len = IIC_TX_RX_FIFO_SIZE;
	dfss_iic_ctrl_ptr->iic_master_code = DFSS_IIC_0_MASTER_CODE;
	dfss_iic_ctrl_ptr->retry_cnt = DFSS_IIC_MAX_RETRY_COUNT;
	dfss_iic_ctrl_ptr->dfss_iic_int_handler = dfss_iic_0_isr;

	/* Variables which always change during iic operation */
	dfss_iic_ctrl_ptr->int_status = 0;
	dfss_iic_ctrl_ptr->iic_tx_over = 0;
	dfss_iic_ctrl_ptr->iic_rx_over = 0;

	/** iic dev init */
	dfss_iic_ptr->iic_open = dfss_iic_0_open;
	dfss_iic_ptr->iic_close = dfss_iic_0_close;
	dfss_iic_ptr->iic_control = dfss_iic_0_control;
	dfss_iic_ptr->iic_write = dfss_iic_0_write;
	dfss_iic_ptr->iic_read = dfss_iic_0_read;

}
#endif /* USE_DFSS_IIC_0 */
/** @} end of name */

#if (USE_DFSS_IIC_1)
/**
 * \name	DFSS IIC 1 Object Instantiation
 * @{
 */
static void dfss_iic_1_isr(void *ptr);
#define DFSS_IIC_1_TARADDR	(0)
#define DFSS_IIC_1_MASTER_CODE	(0)
#define DFSS_IIC_1_IC_CAPLOADING	(DFSS_IIC_CAP_LOADING_100PF)

DEV_IIC		dfss_iic_1;		/*!< DFSS IIC object */
DFSS_IIC_CTRL	dfss_iic_1_ctrl;	/*!< DFSS iic 1 ctrl */

/** DFSS IIC 1 open */
static int32_t dfss_iic_1_open (uint32_t mode, uint32_t param)
{
	return dfss_iic_open(&dfss_iic_1, mode, param);
}
/** DFSS IIC 1 close */
static int32_t dfss_iic_1_close (void)
{
	return dfss_iic_close(&dfss_iic_1);
}
/** DFSS IIC 1 control */
static int32_t dfss_iic_1_control (uint32_t ctrl_cmd, void *param)
{
	return dfss_iic_control(&dfss_iic_1, ctrl_cmd, param);
}
/** DFSS IIC 1 write */
static int32_t dfss_iic_1_write (const void *data, uint32_t len)
{
	return dfss_iic_write(&dfss_iic_1, data, len);
}
/** DFSS IIC 1 read */
static int32_t dfss_iic_1_read (void *data, uint32_t len)
{
	return dfss_iic_read(&dfss_iic_1, data, len);
}
/** DFSS iic 1 interrupt routine */
static void dfss_iic_1_isr(void *ptr)
{
	dfss_iic_isr(&dfss_iic_0, ptr);
}
/** install DFSS IIC 1 to system */
static void dfss_iic_1_install(void)
{
	DEV_IIC *dfss_iic_ptr = &dfss_iic_1;
	DEV_IIC_INFO *dfss_iic_info_ptr = &(dfss_iic_1.iic_info);
	DFSS_IIC_CTRL *dfss_iic_ctrl_ptr = &dfss_iic_1_ctrl;

	/* Info init */
	dfss_iic_info_ptr->iic_ctrl = (void *)dfss_iic_ctrl_ptr;
	dfss_iic_info_ptr->opn_cnt = 0;
	dfss_iic_info_ptr->addr_mode = IIC_7BIT_ADDRESS;
	dfss_iic_info_ptr->tar_addr = DFSS_IIC_0_TARADDR;

	/* Variables which should be set during object implementation */
	dfss_iic_ctrl_ptr->dev_id = 1;
	dfss_iic_ctrl_ptr->ic_caploading= DFSS_IIC_1_IC_CAPLOADING;
	dfss_iic_ctrl_ptr->support_modes = DFSS_IIC_BOTH_SUPPORTED;
	dfss_iic_ctrl_ptr->tx_fifo_len = IIC_TX_RX_FIFO_SIZE;
	dfss_iic_ctrl_ptr->rx_fifo_len = IIC_TX_RX_FIFO_SIZE;
	dfss_iic_ctrl_ptr->iic_master_code = DFSS_IIC_1_MASTER_CODE;
	dfss_iic_ctrl_ptr->retry_cnt = DFSS_IIC_MAX_RETRY_COUNT;
	dfss_iic_ctrl_ptr->dfss_iic_int_handler = dfss_iic_1_isr;

	/* Variables which always change during iic operation */
	dfss_iic_ctrl_ptr->int_status = 0;
	dfss_iic_ctrl_ptr->iic_tx_over = 0;
	dfss_iic_ctrl_ptr->iic_rx_over = 0;

	/** iic dev init */
	dfss_iic_ptr->iic_open = dfss_iic_1_open;
	dfss_iic_ptr->iic_close = dfss_iic_1_close;
	dfss_iic_ptr->iic_control = dfss_iic_1_control;
	dfss_iic_ptr->iic_write = dfss_iic_1_write;
	dfss_iic_ptr->iic_read = dfss_iic_1_read;
}
#endif /* USE_DFSS_IIC_1 */
/** @} end of name */

/** get one designware device structure */
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

/** @} end of group BOARD_IOTDK_DRV_DFSS_IIC_OBJ */
