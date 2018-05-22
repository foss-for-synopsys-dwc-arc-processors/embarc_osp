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

#include "i2c_priv.h"
#include "dev_iic.h"
#include "i2c_master.h"
#include "dfss_iic_obj.h"

// #ifdef DEBUG
// #undef DEBUG
// #endif
#define DEBUG
#include "embARC_debug.h"

#define IC_TX_RX_FIFO_SIZE		16	/* TX/RX FIFO size in hardware */

/* IIC STATUS register */
#define IC_STATUS_TFNF			(0x02)	/* (1 << 1) */
#define IC_STATUS_RFNE			(0x08)	/* (1 << 3) */

/* interrupt callback routines select macros definitions */
#define DFSS_IIC_RDY_SND		(0x1)	/* ready to send callback */
#define DFSS_IIC_RDY_RCV		(0x2)	/* ready to receive callback */

/** check expressions used in DFSS IIC driver implementation */
#define DFSS_IIC_CHECK_EXP(EXPR, ERROR_CODE)		CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

typedef struct iic_dev_context
{
	uint32_t		dev_id;		/* Device ID */
	uint32_t 		reg_base;	/* AUX base address */
	uint32_t 		int_err;	/* Interrupt nunmber ERR */
	uint32_t 		int_rx_avail;	/* Interrupt number RX_AVAIL */
	uint32_t 		int_tx_req;	/* Interrupt TX_REQ */
	uint32_t 		int_stop_det; 	/* Interrupt STOP_DET */
	volatile int32_t 	tx_over;	/* TX done flag */
	volatile int32_t 	rx_over; 	/* RX done flag */
	DEV_IIC_INFO 		*info;		/* IIC device information */
} IIC_DEV_CONTEXT;

IIC_DEV_CONTEXT iic_ctx[3] =
{
// (USE_DFSS_IIC_0)
	{.dev_id = 0,
	.reg_base = AR_IO_I2C_MST0_CON,
	.int_err = IO_I2C_MST0_INT_ERR,
	.int_rx_avail = IO_I2C_MST0_INT_RX_AVAIL,
	.int_tx_req = IO_I2C_MST0_INT_TX_REQ,
	.int_stop_det = IO_I2C_MST0_INT_STOP_DET },
// (USE_DFSS_IIC_1)
	{.dev_id = 1,
	.reg_base = AR_IO_I2C_MST1_CON,
	.int_err = IO_I2C_MST1_INT_ERR,
	.int_rx_avail = IO_I2C_MST1_INT_RX_AVAIL,
	.int_tx_req = IO_I2C_MST1_INT_TX_REQ,
	.int_stop_det = IO_I2C_MST1_INT_STOP_DET },
// (USE_DFSS_IIC_2)
	{2},
};

static inline void dfss_iic_master_write_reg(uint32_t dev_id, uint32_t reg, uint32_t val)
{
	_arc_aux_write(iic_ctx[dev_id].reg_base + reg, val);
}

static inline uint32_t dfss_iic_master_read_reg(uint32_t dev_id, uint32_t reg)
{
	return (_arc_aux_read(iic_ctx[dev_id].reg_base + reg));
}

static inline IIC_DEV_CONTEXT *get_context(uint32_t dev_id)
{
	return &iic_ctx[dev_id];
}

/** test whether iic is ready to write, 1 ready, 0 not ready */
static inline int32_t dfss_iic_master_putready(uint32_t dev_id)
{
	uint32_t status = dfss_iic_master_read_reg(dev_id, I2C_STATUS);
	return ((status & IC_STATUS_TFNF) != 0);
}

/** test whether iic is ready to receive, 1 ready, 0 not ready */
static inline int32_t dfss_iic_master_getready(uint32_t dev_id)
{
	uint32_t status = dfss_iic_master_read_reg(dev_id, I2C_STATUS);
	return ((status & IC_STATUS_RFNE) != 0);
}

/** Disable designware iic bit interrupt with mask */
static inline void dfss_iic_mask_interrupt(uint32_t dev_id, uint32_t mask)
{
	uint32_t intr_mask = dfss_iic_master_read_reg(dev_id, I2C_INTR_MASK);
	dfss_iic_master_write_reg(dev_id, I2C_INTR_MASK, intr_mask&(~mask));
}

static void iic_rx_cb(uint32_t dev_id)
{
	IIC_DEV_CONTEXT *ctx = get_context(dev_id);
	ctx->rx_over = 1;
}

static void iic_tx_cb(uint32_t dev_id)
{
	IIC_DEV_CONTEXT *ctx = get_context(dev_id);
	ctx->tx_over = 1;
}

static void iic_err_cb(uint32_t dev_id)
{
	IIC_DEV_CONTEXT *ctx = get_context(dev_id);
	ctx->tx_over = -1;
	ctx->rx_over = -1;
}

/* enable IIC mater */
static void dfss_iic_master_enable_device(uint32_t dev_id)
{
	IIC_DEV_CONTEXT *ctx = get_context(dev_id);
	DEV_IIC_INFO *info = ctx->info;
	uint32_t enable = dfss_iic_master_read_reg(dev_id, I2C_ENABLE);

	if (((enable & 0x1) & DEV_ENABLED) == 0)
	{
		dfss_iic_master_write_reg(dev_id, I2C_ENABLE, (enable | 0x1));
		info->status |= DEV_ENABLED;
	}
}

/* disable IIC master */
static void dfss_iic_master_disable_device(uint32_t dev_id)
{
	uint32_t enable = dfss_iic_master_read_reg(dev_id, I2C_ENABLE);

	dfss_iic_master_write_reg(dev_id, I2C_ENABLE, (enable & (~(0x1))));

	while ((0x1 & dfss_iic_master_read_reg(dev_id, I2C_ENABLE_STATUS)) != 0);
}

/* reset IIC master */
static void dfss_iic_master_reset_device(uint32_t dev_id)
{
	IIC_DEV_CONTEXT *ctx = get_context(dev_id);
	DEV_IIC_INFO *info = ctx->info;

	io_i2c_master_close(dev_id);
	info->next_cond = IIC_MODE_STOP;
	info->cur_state = IIC_FREE;
	info->err_state = IIC_ERR_NONE;
	ctx->tx_over = 0;
	ctx->rx_over = 0;
	io_i2c_master_open(dev_id);
}

/** Disable iic master interrupt for transmit or receive */
static void dfss_iic_master_dis_cbr(uint32_t dev_id, uint32_t cbrtn)
{
	switch (cbrtn)
	{
		case DFSS_IIC_RDY_SND:
			dfss_iic_mask_interrupt(dev_id, R_TX_EMPTY);
			break;
		case DFSS_IIC_RDY_RCV:
			dfss_iic_mask_interrupt(dev_id, R_TX_EMPTY | R_RX_FULL);
			break;
		default:
			break;
	}
}

/* flush TX FIFO */
static void dfss_iic_master_flush_tx(uint32_t dev_id)
{
	(void)dev_id;
}

/* flush RX FIFO */
static void dfss_iic_master_flush_rx(uint32_t dev_id)
{
	(void)dev_id;
}

/* Get available transmit fifo count */
static int32_t dfss_iic_master_get_txavail(uint32_t dev_id)
{
	uint32_t flr = dfss_iic_master_read_reg(dev_id, I2C_TXFLR);
	return (int32_t)(IC_TX_RX_FIFO_SIZE - flr);
}

/* Get available receive fifo count */
static int32_t dfss_iic_master_get_rxavail(uint32_t dev_id)
{
	uint32_t flr = dfss_iic_master_read_reg(dev_id, I2C_RXFLR);
	return (int32_t)flr;
}

static int32_t dfss_iic_master_close(uint32_t dev_id)
{
	int32_t ret = E_OK;
	IIC_DEV_CONTEXT *ctx = get_context(dev_id);
	DEV_IIC_INFO *info = ctx->info;

	if (info->opn_cnt > 0)
	{
		info->opn_cnt = 0;
		io_i2c_master_close(dev_id);
		int_disable(ctx->int_err);
		int_disable(ctx->int_rx_avail);
		int_disable(ctx->int_tx_req);
		int_disable(ctx->int_stop_det);
	}
	else
	{
		ret = E_CLSED;
	}

	return ret;
}

static int32_t dfss_iic_master_control(uint32_t dev_id, uint32_t ctrl_cmd, void *param)
{
	int32_t ercd = E_OK;
	uint32_t val32;
	uint32_t arg;
	IIC_DEV_CONTEXT *ctx = get_context(dev_id);
	DEV_IIC_INFO *iic_info_ptr = ctx->info;
	io_cb_t callback;

	switch (ctrl_cmd)
	{
		case IIC_CMD_GET_STATUS:
			DFSS_IIC_CHECK_EXP((param!=NULL) && CHECK_ALIGN_4BYTES(param), E_PAR);
			*((int32_t *)param) = iic_info_ptr->status;
			break;
		case IIC_CMD_ENA_DEV:
			dfss_iic_master_enable_device(dev_id);
			break;
		case IIC_CMD_DIS_DEV:
			dfss_iic_master_disable_device(dev_id);
			break;
		case IIC_CMD_RESET:
			dfss_iic_master_reset_device(dev_id);
			break;
		case IIC_CMD_FLUSH_TX:
			dfss_iic_master_flush_tx(dev_id);
			break;
		case IIC_CMD_FLUSH_RX:
			dfss_iic_master_flush_rx(dev_id);
			break;
		case IIC_CMD_SET_ADDR_MODE:
			val32 = (uint32_t)param;
			DFSS_IIC_CHECK_EXP((val32==IIC_7BIT_ADDRESS) || (val32==IIC_10BIT_ADDRESS), E_PAR);
			if (val32==IIC_10BIT_ADDRESS)
			{
				arg = 1;
				io_i2c_master_ioctl(dev_id, IO_I2C_MASTER_SET_10BIT_ADDR, &arg);
				iic_info_ptr->addr_mode = IIC_10BIT_ADDRESS;
			}
			else
			{
				arg = 0;
				io_i2c_master_ioctl(dev_id, IO_I2C_MASTER_SET_10BIT_ADDR, &arg);
				iic_info_ptr->addr_mode = IIC_7BIT_ADDRESS;
			}
			iic_info_ptr->addr_mode = val32;
			break;
		case IIC_CMD_GET_RXAVAIL:
			DFSS_IIC_CHECK_EXP((param!=NULL) && CHECK_ALIGN_4BYTES(param), E_PAR);
			*((int32_t *)param) = dfss_iic_master_get_rxavail(dev_id);
			break;
		case IIC_CMD_GET_TXAVAIL:
			DFSS_IIC_CHECK_EXP((param!=NULL) && CHECK_ALIGN_4BYTES(param), E_PAR);
			*((int32_t *)param) = dfss_iic_master_get_txavail(dev_id);
			break;
		case IIC_CMD_SET_TXCB:
			DFSS_IIC_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
			callback.cb = param;
			io_i2c_master_ioctl(dev_id, IO_SET_CB_TX, &callback);
			break;
		case IIC_CMD_SET_RXCB:
			DFSS_IIC_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
			callback.cb = param;
			io_i2c_master_ioctl(dev_id, IO_SET_CB_RX, &callback);
			break;
		case IIC_CMD_SET_ERRCB:
			DFSS_IIC_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
			callback.cb = param;
			io_i2c_master_ioctl(dev_id, IO_SET_CB_ERR, &callback);
			break;
		case IIC_CMD_ABORT_TX:
			ercd = E_NOSPT;
			break;
		case IIC_CMD_ABORT_RX:
			ercd = E_NOSPT;
			break;
		case IIC_CMD_SET_TXINT:
			val32 = (uint32_t)param;
			if (val32 == 0)
			{
				dfss_iic_master_dis_cbr(dev_id, DFSS_IIC_RDY_SND);
			}
			else
			{
				dfss_iic_master_dis_cbr(dev_id, DFSS_IIC_RDY_SND);
			}
			break;
		case IIC_CMD_SET_RXINT:
			val32 = (uint32_t)param;
			if (val32 == 0)
			{
				dfss_iic_master_dis_cbr(dev_id, DFSS_IIC_RDY_RCV);
			}
			else
			{
				dfss_iic_master_dis_cbr(dev_id, DFSS_IIC_RDY_RCV);
			}
			break;
		case IIC_CMD_SET_TXINT_BUF:
			ercd = E_NOSPT;
			break;
		case IIC_CMD_SET_RXINT_BUF:
			ercd = E_NOSPT;
			break;
		case IIC_CMD_MST_SET_SPEED_MODE:
			val32 = (uint32_t)param;
			DFSS_IIC_CHECK_EXP((val32>=IIC_SPEED_STANDARD) && (val32<=IIC_SPEED_FAST), E_PAR);
			if (val32 == IIC_SPEED_STANDARD)
			{
				arg = 1;
				io_i2c_master_ioctl(dev_id, IO_I2C_MASTER_SET_SPEED, &arg);
				iic_info_ptr->speed_mode = IIC_SPEED_STANDARD;
			}
			else
			{
				arg = 2;
				io_i2c_master_ioctl(dev_id, IO_I2C_MASTER_SET_SPEED, &arg);
				iic_info_ptr->speed_mode = IIC_SPEED_FAST;
			}
			break;
		case IIC_CMD_MST_SET_TAR_ADDR:
			if (iic_info_ptr->addr_mode == IIC_7BIT_ADDRESS) {
				val32 = ((uint32_t)param) & IIC_7BIT_ADDRESS_MASK;
			} else {
				val32 = ((uint32_t)param) & IIC_10BIT_ADDRESS_MASK;
			}
			if (val32 != iic_info_ptr->tar_addr)
			{
				arg = val32;
				io_i2c_master_ioctl(dev_id, IO_I2C_MASTER_SET_TARGET_ADDR, &arg);
				iic_info_ptr->tar_addr = val32;
			}
			break;
		case IIC_CMD_MST_SET_NEXT_COND:
			break;
		default:
			ercd = E_NOSPT;
			break;
	}

error_exit:
	return ercd;
}


/* param: speed mode, 1-standard mode, 2-fast mode, 2 as default in hardware */
static int32_t dfss_iic_master_open(uint32_t dev_id, uint32_t param)
{
	int32_t ercd = E_OK;
	IIC_DEV_CONTEXT *ctx = get_context(dev_id);
	DEV_IIC_INFO *info = ctx->info;
	io_cb_t callback;
	int32_t int_e = 0;
	uint32_t val32 = 0;
	uint32_t arg;

	DFSS_IIC_CHECK_EXP((param>=IIC_SPEED_STANDARD) && (param<=IIC_SPEED_FAST), E_PAR);

	if (info->opn_cnt == 0)
	{
		DFSS_IIC_CHECK_EXP(io_i2c_master_open(dev_id) == 0, E_SYS);
		val32 = (uint32_t)param;
		if (val32 == IIC_SPEED_STANDARD)
		{
			arg = 1;
			io_i2c_master_ioctl(dev_id, IO_I2C_MASTER_SET_SPEED, &arg);
			info->speed_mode = IIC_SPEED_STANDARD;
		}
		else
		{
			arg = 2;
			io_i2c_master_ioctl(dev_id, IO_I2C_MASTER_SET_SPEED, &arg);
			info->speed_mode = IIC_SPEED_FAST;
		}

		callback.cb = &iic_rx_cb;
		io_i2c_master_ioctl(dev_id, IO_SET_CB_RX, &callback);
		callback.cb = &iic_tx_cb;
		io_i2c_master_ioctl(dev_id, IO_SET_CB_TX, &callback);
		callback.cb = &iic_err_cb;
		io_i2c_master_ioctl(dev_id, IO_SET_CB_ERR, &callback);

		info->opn_cnt++;
		info->addr_mode = IIC_7BIT_ADDRESS;
		info->mode = DEV_MASTER_MODE;
		info->tar_addr &= IIC_7BIT_ADDRESS_MASK;

		ctx->tx_over = 1;
		ctx->rx_over = 1;

		/* enable interrupt */
		int_e = int_e | int_enable(ctx->int_err);
		int_e = int_e | int_enable(ctx->int_rx_avail);
		int_e = int_e | int_enable(ctx->int_tx_req);
		int_e = int_e | int_enable(ctx->int_stop_det);
		if (int_e != 0)
		{
			ercd = E_PAR;
		}
	}
	else
	{
		ercd = E_OPNED;
	}

error_exit:
	return ercd;
}

static int32_t dfss_iic_master_write(uint32_t dev_id, const void *data, uint32_t len)
{
	IIC_DEV_CONTEXT *ctx = get_context(dev_id);
	uint32_t xlen;

	if ((ctx->rx_over != 1) && (ctx->tx_over != 1))
	{
		return E_NORES;
	}
	xlen = len;
	ctx->tx_over = 0;	// busy
	io_i2c_master_write(dev_id, (uint8_t *)data, &xlen);
	while (!ctx->tx_over);

	return len;
}

static int32_t dfss_iic_master_read(uint32_t dev_id, const void *data, uint32_t len)
{
	IIC_DEV_CONTEXT *ctx = get_context(dev_id);
	uint32_t xlen;

	if ((ctx->rx_over != 1) && (ctx->tx_over != 1))
	{
		return E_NORES;
	}
	xlen = len;
	ctx->rx_over = 0;	// busy
	io_i2c_master_read(dev_id, (uint8_t *)data, &xlen);
	while (!ctx->rx_over);

	return len;
}

#if (USE_DFSS_IIC_0)
/**
 * \name	DFSS IIC 0 Object Instantiation
 * @{
 */

#define DFSS_IIC_0_TARADDR	(0)

static DEV_IIC			dfss_iic_0;		/*!< DFSS IIC object */

/** DFSS IIC 0 open */
static int32_t dfss_iic_0_open (uint32_t mode, uint32_t param)
{
	(void)mode;
	return dfss_iic_master_open(0, param);
}
/** DFSS IIC 0 close */
static int32_t dfss_iic_0_close (void)
{
	return dfss_iic_master_close(0);
}
/** DFSS IIC 0 control */
static int32_t dfss_iic_0_control (uint32_t ctrl_cmd, void *param)
{
	return dfss_iic_master_control(0, ctrl_cmd, param);
}
/** DFSS IIC 0 write */
static int32_t dfss_iic_0_write (const void *data, uint32_t len)
{
	return dfss_iic_master_write(0, data, len);
}
/** DFSS IIC 0 read */
static int32_t dfss_iic_0_read (void *data, uint32_t len)
{
	return dfss_iic_master_read(0, data, len);
}
/** install DFSS IIC 0 to system */
static void dfss_iic_0_install(void)
{
	DEV_IIC *dfss_iic_ptr = &dfss_iic_0;
	DEV_IIC_INFO *dfss_iic_info_ptr = &(dfss_iic_0.iic_info);
	IIC_DEV_CONTEXT *ctx = get_context(0);

	ctx->info = &(dfss_iic_0.iic_info);
	/* Info init */
	dfss_iic_info_ptr->iic_ctrl = NULL;
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
/** @} end of name */

#if (USE_DFSS_IIC_1)
/**
 * \name	DFSS IIC 1 Object Instantiation
 * @{
 */

#define DFSS_IIC_1_TARADDR	(0)

static DEV_IIC			dfss_iic_1;		/*!< DFSS IIC object */

/** DFSS IIC 1 open */
static int32_t dfss_iic_1_open (uint32_t mode, uint32_t param)
{
	(void)mode;
	return dfss_iic_master_open(1, param);
}
/** DFSS IIC 1 close */
static int32_t dfss_iic_1_close (void)
{
	return dfss_iic_master_close(1);
}
/** DFSS IIC 1 control */
static int32_t dfss_iic_1_control (uint32_t ctrl_cmd, void *param)
{
	return dfss_iic_master_control(1, ctrl_cmd, param);
}
/** DFSS IIC 1 write */
static int32_t dfss_iic_1_write (const void *data, uint32_t len)
{
	return dfss_iic_master_write(1, data, len);
}
/** DFSS IIC 1 read */
static int32_t dfss_iic_1_read (void *data, uint32_t len)
{
	return dfss_iic_master_read(1, data, len);
}
/** install DFSS IIC 1 to system */
static void dfss_iic_1_install(void)
{
	DEV_IIC *dfss_iic_ptr = &dfss_iic_1;
	DEV_IIC_INFO *dfss_iic_info_ptr = &(dfss_iic_1.iic_info);
	IIC_DEV_CONTEXT *ctx = get_context(1);

	ctx->info = &(dfss_iic_1.iic_info);
	/* Info init */
	dfss_iic_info_ptr->iic_ctrl = NULL;
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
