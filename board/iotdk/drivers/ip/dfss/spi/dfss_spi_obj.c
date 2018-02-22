/* ------------------------------------------
 * Copyright (c) 2016, Synopsys, Inc. All rights reserved.

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
 * \version 2017.03
 * \date 2017-08-25
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */

/**
 * \defgroup	BOARD_IOTDK_DRV_DFSS_SPI_OBJ	IOTDK DFSS SPI Object
 * \ingroup	BOARD_IOTDK_DRIVER
 * \brief	IOTDK DFSS SPI Objects
 */

/**
 * \file
 * \ingroup	BOARD_IOTDK_DRV_DFSS_SPI_OBJ
 * \brief	DFSS SPI object instantiation on iotdk
 */

/**
 * \addtogroup	BOARD_IOTDK_DRV_DFSS_SPI_OBJ
 * @{
 */
#include <string.h>

#include "arc.h"
#include "arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "arc_exception.h"

#include "../../iotdk.h"

#include "dev_spi.h"
#include "spi_master.h"
#include "spi_slave.h"


#define FLAG_TX_READY		(1 << 0) /* interrupt tx */
#define FLAG_RX_READY		(1 << 1) /* interrupt rx */
#define FLAG_BUSY		(1 << 2)
#define FLAG_TX_RX		(1 << 3) /* both tx and rx */
#define FLAG_ERROR		(1 << 4)

typedef volatile struct spi_dev_context
{
	uint8_t	dev_id;
	uint8_t intno_rx;
	uint8_t intno_tx;
	uint8_t intno_idle;
	uint8_t intno_err;
	uint8_t flags;
	IO_CB_FUNC int_rx_cb;
	IO_CB_FUNC int_tx_cb;
	IO_CB_FUNC int_idle_cb;
	IO_CB_FUNC int_err_cb;
	DEV_SPI_INFO *info;
} SPI_DEV_CONTEXT;


static int32_t spi_master_open(SPI_DEV_CONTEXT *ctx, uint32_t mode, uint32_t param)
{
	uint32_t dev_id = ctx->dev_id;
	DEV_SPI_INFO *info = ctx->info;
	io_cb_t callback;
	uint32_t val;

	if (mode != DEV_MASTER_MODE) {
		return E_PAR;
	}

	if (info->opn_cnt > 0) {
		if (param != info->freq) {
			return E_OPNED;
		} else {
			return E_OK;
		}
	}

	if (io_spi_master_open(dev_id)) {
		return E_SYS;
	}

	callback.cb = ctx->int_rx_cb;
	io_spi_master_ioctl(dev_id, IO_SET_CB_RX, &callback);
	callback.cb = ctx->int_tx_cb;
	io_spi_master_ioctl(dev_id, IO_SET_CB_TX, &callback);
	callback.cb = ctx->int_err_cb;
	io_spi_master_ioctl(dev_id, IO_SET_CB_ERR, &callback);

	io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_DFS, &info->dfs);
	val = 1;
	io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_PACKING, &val);

	info->freq = param;
	param = BOARD_SPI_CLK / param;

	io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_BAUD, &param);
	info->opn_cnt++;
	ctx->flags = 0;

	int_enable(ctx->intno_rx);
	int_pri_set(ctx->intno_rx, INT_PRI_MIN + 1);
	int_enable(ctx->intno_tx);
	int_pri_set(ctx->intno_tx, INT_PRI_MIN + 1);
	int_enable(ctx->intno_idle);
	int_pri_set(ctx->intno_idle, INT_PRI_MIN + 1);
	int_enable(ctx->intno_err);
	int_pri_set(ctx->intno_err, INT_PRI_MIN + 1);


	return E_OK;
}
/** DFSS SPI close */
static int32_t spi_master_close(SPI_DEV_CONTEXT *ctx)
{
	DEV_SPI_INFO *info = ctx->info;

	info->opn_cnt--;
	if (info->opn_cnt == 0) {
		io_spi_master_close(ctx->dev_id);
		int_disable(ctx->intno_rx);
		int_disable(ctx->intno_tx);
		int_disable(ctx->intno_idle);
		int_disable(ctx->intno_err);
		info->status = DEV_DISABLED;
		ctx->flags = 0;
	} else {
		return E_OPNED;
	}
	return E_OK;
}
/** DFSS SPI control */
static int32_t spi_master_control(SPI_DEV_CONTEXT *ctx, uint32_t ctrl_cmd, void *param)
{
	uint32_t dev_id = ctx->dev_id;
	DEV_SPI_INFO *info = ctx->info;
	uint32_t val32 = (uint32_t)param;
	DEV_BUFFER *buf = (DEV_BUFFER *)param;
	DEV_SPI_TRANSFER *spi_xfer = &(info->xfer);

	switch (ctrl_cmd) {
		case SPI_CMD_SET_TXINT_BUF:
			if (buf && !(ctx->flags & FLAG_BUSY)) {
				DEV_SPI_XFER_SET_TXBUF(spi_xfer, buf->buf, buf->ofs, buf->len);
				ctx->flags |= FLAG_TX_READY;
			} else {
				return E_NOSPT;
			}
			break;
		case SPI_CMD_SET_TXINT:
			if (val32 && (ctx->flags & FLAG_TX_READY) && !(ctx->flags & FLAG_BUSY)) {
				ctx->flags |= FLAG_BUSY;
				val32 = SPI_TRANSMIT_ONLY_MODE;
				io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
				io_spi_master_write(dev_id, spi_xfer->tx_buf + spi_xfer->tx_ofs,
					&spi_xfer->tx_len);
			} else {
				return E_SYS;
			}
			break;

		case SPI_CMD_SET_RXINT_BUF:
			if (buf && !(ctx->flags & FLAG_BUSY)) {
				DEV_SPI_XFER_SET_RXBUF(spi_xfer, buf->buf, buf->ofs, buf->len);
				ctx->flags |= FLAG_RX_READY;
			} else {
				return E_NOSPT;
			}
			break;

		case SPI_CMD_SET_RXINT:
			if (val32 && (ctx->flags & FLAG_RX_READY) && !(ctx->flags & FLAG_BUSY)) {
				ctx->flags |= FLAG_BUSY;
				val32 = SPI_RECEIVE_ONLY_MODE;
				io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
				io_spi_master_read(dev_id, spi_xfer->rx_buf + spi_xfer->rx_ofs,
					&spi_xfer->rx_len);
			} else {
				return E_SYS;
			}
			break;

		case SPI_CMD_TRANSFER_INT:
			if (param != NULL && !(ctx->flags & FLAG_BUSY)) {
				ctx->flags = FLAG_BUSY | FLAG_TX_RX;
				*spi_xfer = *((DEV_SPI_TRANSFER *)param);

				if (spi_xfer->rx_len == 0) {
					val32 = SPI_TRANSMIT_ONLY_MODE;
					ctx->flags |= FLAG_TX_READY;
					io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
					io_spi_master_write(dev_id, spi_xfer->tx_buf, &spi_xfer->tx_len);
				} else if (spi_xfer->tx_len == 0) {
					val32 = SPI_RECEIVE_ONLY_MODE;
					ctx->flags |= FLAG_RX_READY;
					io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
					io_spi_master_read(dev_id, spi_xfer->rx_buf, &spi_xfer->rx_len);
				} else if (spi_xfer->rx_ofs == spi_xfer->tx_len && spi_xfer->tx_ofs == 0) {
					ctx->flags |= (FLAG_RX_READY | FLAG_TX_READY);
					val32 = SPI_RECEIVE_AFTER_TRANSMIT_MODE;
					io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
					io_spi_master_read(dev_id, spi_xfer->rx_buf, &spi_xfer->rx_len);
					io_spi_master_write(dev_id, spi_xfer->tx_buf, &spi_xfer->tx_len);
				} else {

					if (spi_xfer->tx_ofs != 0 || spi_xfer->rx_ofs != 0) {
						return E_NOSPT;
					}

					ctx->flags |= (FLAG_RX_READY | FLAG_TX_READY);
					DEV_SPI_XFER_INIT(spi_xfer);

					val32 = SPI_TRANSMIT_RECEIVE_MODE;

					io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
					io_spi_master_read(dev_id, spi_xfer->rx_buf, &spi_xfer->tot_len);
					io_spi_master_write(dev_id, spi_xfer->tx_buf, &spi_xfer->tot_len);
				}
			} else {
				return E_NOSPT;
			}
			break;

		case SPI_CMD_TRANSFER_POLLING:
			if (param != NULL && !(ctx->flags & FLAG_BUSY)) {

				if (arc_locked()) {
					/*
					 * not allow to be called in isr or cpu is locked.
					 * Beacue the bottom drvier is simply interrupt driven
					 */
					return E_SYS;
				}

				*spi_xfer = *((DEV_SPI_TRANSFER *)param);


				while (spi_xfer != NULL) {
					if (spi_xfer->rx_len == 0) {
						ctx->flags = FLAG_BUSY;
						val32 = SPI_TRANSMIT_ONLY_MODE;
						io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
						io_spi_master_write(dev_id, spi_xfer->tx_buf, &spi_xfer->tx_len);

						while (ctx->flags & FLAG_BUSY);
					} else if (spi_xfer->tx_len == 0) {
						ctx->flags = FLAG_BUSY;
						val32 = SPI_RECEIVE_ONLY_MODE;

						io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
						io_spi_master_read(dev_id, spi_xfer->rx_buf, &spi_xfer->rx_len);

						while (ctx->flags & FLAG_BUSY);
					} else if (spi_xfer->rx_ofs == spi_xfer->tx_len && spi_xfer->tx_ofs == 0) {
						ctx->flags = FLAG_BUSY | FLAG_TX_RX;
						val32 = SPI_RECEIVE_AFTER_TRANSMIT_MODE;

						io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
						io_spi_master_read(dev_id, spi_xfer->rx_buf, &spi_xfer->rx_len);
						io_spi_master_write(dev_id, spi_xfer->tx_buf, &spi_xfer->tx_len);
						while (ctx->flags & (FLAG_BUSY | FLAG_TX_RX));
					} else {

						if (spi_xfer->tx_ofs != 0 || spi_xfer->rx_ofs != 0) {
							return E_NOSPT;
						}

						DEV_SPI_XFER_INIT(spi_xfer);

						ctx->flags = FLAG_BUSY | FLAG_TX_RX;
						val32 = SPI_TRANSMIT_RECEIVE_MODE;

						io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
						io_spi_master_read(dev_id, spi_xfer->rx_buf, &spi_xfer->tot_len);
						io_spi_master_write(dev_id, spi_xfer->tx_buf, &spi_xfer->tot_len);

						while (ctx->flags & (FLAG_BUSY | FLAG_TX_RX));
					}

					if (ctx->flags & FLAG_ERROR) {
						ctx->flags = 0;
						return E_SYS;
					}
					ctx->flags = 0;
					spi_xfer = spi_xfer->next;
				}

			} else {
				return E_NOSPT;
			}
			break;

		case SPI_CMD_SET_TXCB:
			info->spi_cbs.tx_cb = param;
			break;

		case SPI_CMD_SET_RXCB:
			info->spi_cbs.rx_cb = param;
			break;

		case SPI_CMD_SET_XFERCB:
			info->spi_cbs.xfer_cb = param;
			break;

		case SPI_CMD_SET_ERRCB:
			info->spi_cbs.err_cb = param;
			break;

		case SPI_CMD_SET_CLK_MODE:
			switch (val32) {
				case SPI_CPOL_0_CPHA_0:
					val32 = 0;
					io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SCPL, &val32);
					io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SCPH, &val32);
					break;
				case SPI_CPOL_0_CPHA_1:
					val32 = 0;
					io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SCPL, &val32);
					val32 = 1;
					io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SCPH, &val32);
					break;
				case SPI_CPOL_1_CPHA_0:
					val32 = 1;
					io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SCPL, &val32);
					val32 = 0;
					io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SCPH, &val32);
					break;
				case SPI_CPOL_1_CPHA_1:
					val32 = 1;
					io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SCPL, &val32);
					io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SCPH, &val32);
					break;
			}
			break;

		case SPI_CMD_SET_DFS:
			io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_DFS, &val32);
			break;

		case SPI_CMD_MST_SET_FREQ:
			info->freq = val32;
			val32 = BOARD_SPI_CLK / val32;
			io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_BAUD, &val32);
			break;

		case SPI_CMD_MST_SEL_DEV:
			info->slave = val32;
			val32 = (1 << val32);
			io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SE, &val32);
			break;

		case SPI_CMD_MST_DSEL_DEV:
			val32 = info->slave & (~(1 << val32));
			info->slave = SPI_SLAVE_NOT_SELECTED;
			io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_SE, &val32);
			break;

		default:
			return E_NOSPT;
	}
	return E_OK;
}
/** DFSS SPI write */
static int32_t spi_master_write(SPI_DEV_CONTEXT *ctx, const void *data, uint32_t len)
{
	uint32_t dev_id = ctx->dev_id;
	uint32_t len_  = len;
	uint32_t val32 = SPI_TRANSMIT_ONLY_MODE;

	/* spi busy */
	if (ctx->flags & FLAG_BUSY) {
		return E_NORES;
	}

	if (arc_locked()) {
		/*
		 * not allow to be called in isr or cpu is locked.
		 * Beacue the bottom drvier is simply interrupt driven
		 */
		return E_SYS;
	}

	ctx->flags = FLAG_BUSY;
	io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
	io_spi_master_write(dev_id, (uint8_t *) data, &len_);

	/* wait finished: spi int enable & no cpu lock */
	while (ctx->flags & FLAG_BUSY);

	if (ctx->flags & FLAG_ERROR) {
		ctx->flags = 0;
		return E_SYS;
	}

	ctx->flags = 0;
	return E_OK;
}
/** DFSS SPI  read */
static int32_t spi_master_read(SPI_DEV_CONTEXT *ctx, void *data, uint32_t len)
{
	uint32_t dev_id = ctx->dev_id;
	uint32_t len_ = len;
	uint32_t val32 = SPI_RECEIVE_ONLY_MODE;

	if (ctx->flags & FLAG_BUSY) {
		return E_NORES;
	}

	if (arc_locked()) {
		/*
		 * not allow to be called in isr or cpu is locked.
		 * Beacue the bottom drvier is simply interrupt driven
		 */
		return E_SYS;
	}

	ctx->flags = FLAG_BUSY;

	io_spi_master_ioctl(dev_id, IO_SPI_MASTER_SET_TRANSFER_MODE, &val32);
	io_spi_master_read(dev_id, (uint8_t *)data, &len_);

	while (ctx->flags & FLAG_BUSY);

	if (ctx->flags & FLAG_ERROR) {
		ctx->flags = 0;
		return E_SYS;
	}
	ctx->flags = 0;
	return 0;
}

/**
 * \name	IOTDK DFSS SPI 0 Object Instantiation
 * @{
 */
#if (USE_DFSS_SPI_0)
static void spi_tx_cb0(uint32_t dev_id);
static void spi_rx_cb0(uint32_t dev_id);
static void spi_err_cb0(uint32_t dev_id);

static DEV_SPI dfss_spi_0; /*!< dfss spi object */

static SPI_DEV_CONTEXT spi_context0 = {
	DFSS_SPI_0_ID,
	IO_SPI_MST0_INT_RX_AVAIL,
	IO_SPI_MST0_INT_TX_REQ,
	IO_SPI_MST0_INT_IDLE,
	IO_SPI_MST0_INT_ERR,
	0,
	spi_rx_cb0,
	spi_tx_cb0,
	spi_err_cb0,
};

static void spi_tx_cb0(uint32_t dev_id)
{
	SPI_DEV_CONTEXT *ctx = &spi_context0;
	DEV_SPI_INFO *info = ctx->info;
	DEV_SPI_TRANSFER *spi_xfer = &(info->xfer);

	if (ctx->flags & FLAG_TX_RX) {
		if (spi_xfer->rx_len == 0) {
			ctx->flags &= ~FLAG_TX_RX;

			if (ctx->flags & FLAG_TX_READY && info->spi_cbs.xfer_cb) {
				info->spi_cbs.xfer_cb(info);
			}
		}
	} else if (ctx->flags & FLAG_TX_READY && info->spi_cbs.tx_cb) {
		info->spi_cbs.tx_cb(info);
	}

	ctx->flags &= ~(FLAG_TX_READY | FLAG_BUSY);

}

static void spi_rx_cb0(uint32_t dev_id)
{
	SPI_DEV_CONTEXT *ctx = &spi_context0;
	DEV_SPI_INFO *info = ctx->info;

	if (ctx->flags & FLAG_TX_RX) {
		if (ctx->flags & FLAG_RX_READY && info->spi_cbs.xfer_cb) {
			info->spi_cbs.xfer_cb(info);
		}
		ctx->flags &= ~(FLAG_RX_READY | FLAG_TX_READY | FLAG_BUSY | FLAG_TX_RX);

	} else {
		if (ctx->flags & FLAG_RX_READY && info->spi_cbs.rx_cb) {
			info->spi_cbs.rx_cb(info);
		}

		ctx->flags &= ~(FLAG_RX_READY | FLAG_BUSY);
	}
}

static void spi_err_cb0(uint32_t dev_id)
{
	SPI_DEV_CONTEXT *ctx = &spi_context0;
	DEV_SPI_INFO *info = ctx->info;

	ctx->flags = FLAG_ERROR;

	if (info->spi_cbs.err_cb) {
		info->spi_cbs.err_cb(info);
	}
}


/** subsystem spi 0 open */
static int32_t dfss_spi_0_open(uint32_t mode, uint32_t param)
{
	return spi_master_open(&spi_context0, mode, param);
}
/** subsystem spi 0 close */
static int32_t dfss_spi_0_close (void)
{
	return spi_master_close(&spi_context0);
}
/** subsystem spi 0 control */
static int32_t dfss_spi_0_control(uint32_t ctrl_cmd, void *param)
{
	return spi_master_control(&spi_context0, ctrl_cmd, param);
}
/** subsystem spi 0 write */
static int32_t dfss_spi_0_write(const void *data, uint32_t len)
{
	return spi_master_write(&spi_context0, data, len);
}
/** subsystem spi 0 close */
static int32_t dfss_spi_0_read(void *data, uint32_t len)
{
	return spi_master_read(&spi_context0, data, len);
}
/** subsystem spi 0 interrupt routine */

/** install subsystem spi 0 to system */
static void dfss_spi_0_install(void)
{
	DEV_SPI *dfss_spi_ptr = &dfss_spi_0;
	DEV_SPI_INFO *info = &(dfss_spi_0.spi_info);

	spi_context0.info = info;
	/** spi info init */
	info->status = DEV_DISABLED;
	info->freq = 1000000;
	info->opn_cnt = 0;
	info->mode = DEV_MASTER_MODE;
	info->clk_mode = SPI_CLK_MODE_DEFAULT;
	info->slave = SPI_SLAVE_NOT_SELECTED;
	info->dfs = SPI_DFS_DEFAULT;
	info->dummy = 0xff;
	info->spi_ctrl = (void *)&spi_context0;

	/** spi dev init */
	dfss_spi_ptr->spi_open = dfss_spi_0_open;
	dfss_spi_ptr->spi_close = dfss_spi_0_close;
	dfss_spi_ptr->spi_control = dfss_spi_0_control;
	dfss_spi_ptr->spi_write = dfss_spi_0_write;
	dfss_spi_ptr->spi_read = dfss_spi_0_read;
}
#endif /* USE_DFSS_SPI_0 */


/**
 * \name	IOTDK DFSS SPI 0 Object Instantiation
 * @{
 */
#if (USE_DFSS_SPI_1)
static void spi_tx_cb1(uint32_t dev_id);
static void spi_rx_cb1(uint32_t dev_id);
static void spi_err_cb1(uint32_t dev_id);

static DEV_SPI dfss_spi_1; /*!< dfss spi object */

static SPI_DEV_CONTEXT spi_context1 = {
	DFSS_SPI_1_ID,
	IO_SPI_MST1_INT_RX_AVAIL,
	IO_SPI_MST1_INT_TX_REQ,
	IO_SPI_MST1_INT_IDLE,
	IO_SPI_MST1_INT_ERR,
	0,
	spi_rx_cb1,
	spi_tx_cb1,
	spi_err_cb1,
};

static void spi_tx_cb1(uint32_t dev_id)
{
	SPI_DEV_CONTEXT *ctx = &spi_context1;
	DEV_SPI_INFO *info = ctx->info;
	DEV_SPI_TRANSFER *spi_xfer = &(info->xfer);

	if (ctx->flags & FLAG_TX_RX) {
		if (spi_xfer->rx_len == 0) {
			ctx->flags &= ~FLAG_TX_RX;

			if (ctx->flags & FLAG_TX_READY && info->spi_cbs.xfer_cb) {
				info->spi_cbs.xfer_cb(info);
			}
		}
	} else if (ctx->flags & FLAG_TX_READY && info->spi_cbs.tx_cb) {
		info->spi_cbs.tx_cb(info);
	}

	ctx->flags &= ~(FLAG_TX_READY | FLAG_BUSY);
}

static void spi_rx_cb1(uint32_t dev_id)
{
	SPI_DEV_CONTEXT *ctx = &spi_context1;
	DEV_SPI_INFO *info = ctx->info;

	if (ctx->flags & FLAG_TX_RX) {
		if (ctx->flags & FLAG_RX_READY && info->spi_cbs.xfer_cb) {
			info->spi_cbs.xfer_cb(info);
		}
		ctx->flags &= ~(FLAG_RX_READY | FLAG_TX_READY | FLAG_BUSY | FLAG_TX_RX);

	} else {
		if (ctx->flags & FLAG_RX_READY && info->spi_cbs.rx_cb) {
			info->spi_cbs.rx_cb(info);
		}

		ctx->flags &= ~(FLAG_RX_READY | FLAG_BUSY);
	}
}

static void spi_err_cb1(uint32_t dev_id)
{
	SPI_DEV_CONTEXT *ctx = &spi_context1;
	DEV_SPI_INFO *info = ctx->info;

	ctx->flags = FLAG_ERROR;
	if (info->spi_cbs.err_cb) {
		info->spi_cbs.err_cb(info);
	}
}


/** subsystem spi 1 open */
static int32_t dfss_spi_1_open(uint32_t mode, uint32_t param)
{
	return spi_master_open(&spi_context1, mode, param);
}
/** subsystem spi 1 close */
static int32_t dfss_spi_1_close (void)
{
	return spi_master_close(&spi_context1);
}
/** subsystem spi 1 control */
static int32_t dfss_spi_1_control(uint32_t ctrl_cmd, void *param)
{
	return spi_master_control(&spi_context1, ctrl_cmd, param);
}
/** subsystem spi 1 write */
static int32_t dfss_spi_1_write(const void *data, uint32_t len)
{
	return spi_master_write(&spi_context1, data, len);
}
/** subsystem spi 1 close */
static int32_t dfss_spi_1_read(void *data, uint32_t len)
{
	return spi_master_read(&spi_context1, data, len);
}

/** install subsystem spi 1 to system */
static void dfss_spi_1_install(void)
{
	DEV_SPI *dfss_spi_ptr = &dfss_spi_1;
	DEV_SPI_INFO *info = &(dfss_spi_1.spi_info);

	spi_context1.info = info;
	/** spi info init */
	info->status = DEV_DISABLED;
	info->freq = 1000000;
	info->opn_cnt = 0;
	info->mode = DEV_MASTER_MODE;
	info->clk_mode = SPI_CLK_MODE_DEFAULT;
	info->slave = SPI_SLAVE_NOT_SELECTED;
	info->dfs = SPI_DFS_DEFAULT;
	info->dummy = 0xff;
	info->spi_ctrl = (void *)&spi_context1;

	/** spi dev init */
	dfss_spi_ptr->spi_open = dfss_spi_1_open;
	dfss_spi_ptr->spi_close = dfss_spi_1_close;
	dfss_spi_ptr->spi_control = dfss_spi_1_control;
	dfss_spi_ptr->spi_write = dfss_spi_1_write;
	dfss_spi_ptr->spi_read = dfss_spi_1_read;
}
#endif /* USE_DFSS_SPI_1 */
/** @} end of name */

/** get one subsystem device structure */
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

/** @} end of group BOARD_IOTDK_DRV_DFSS_SPI_OBJ */