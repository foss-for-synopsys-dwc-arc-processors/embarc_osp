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
 * \file
 * \brief dfss iic driver header file
 * \ingroup DEVICE_DFSS_IIC
 */
#ifndef _DFSS_IIC_H_
#define _DFSS_IIC_H_


#include "../ioctl.h"
#include "dev_iic.h"

/**
 * If this header file is included,
 * will indicate that this dfss iic device
 * is used
 */
#define DEVICE_USE_DFSS_IIC

/**
 * \defgroup  DEVICE_DFSS_IIC_INFO  DFSS IIC Related Information
 * \ingroup DEVICE_DFSS_IIC
 * \brief Defines some macros of DFSS IIC need.
 * \details macros like, iic number
 * @{
 */
#define	DFSS_IIC_MASTER_SUPPORTED		(0x1 << 0) /*!< Support DFSS IIC Master Mode */
#define	DFSS_IIC_SLAVE_SUPPORTED		(0x1 << 1) /*!< Support DFSS IIC Slave Mode */
/*!< Support DFSS IIC Both Master and Slave Mode */
#define	DFSS_IIC_BOTH_SUPPORTED			(DFSS_IIC_MASTER_SUPPORTED | DFSS_IIC_SLAVE_SUPPORTED)
/** @} */

#define	DFSS_IIC_INVALID_INTNO			(DEV_INTNO_INVALID)

/* I2C working speeds. */
#define	I2C_STANDARD_SPEED			(0x01)
#define	I2C_FAST_SPEED				(0x02)
#define DFSS_IIC_MAX_RETRY_COUNT		(200000)

/* I2C devices common control commands */
#define	IO_I2C_MASTER_SET_HOLD_TIME_RX		(0x06)
#define	IO_I2C_MASTER_SET_HOLD_TIME_TX		(0x07)
#define	IO_I2C_MASTER_SET_SPKLEN		(0x08)
#define	IO_I2C_MASTER_SET_RX_THRESHOLD		(0x09)
#define	IO_I2C_MASTER_SET_TX_THRESHOLD		(0x0a)
#define	IO_I2C_MASTER_SET_10BIT_ADDR		(0x0b)
/* I2C master control commands */
#define IO_I2C_MASTER_SET_TARGET_ADDR		(0x20)
#define IO_I2C_MASTER_SET_SPEED			(0x21)
#define IO_I2C_MASTER_SET_SS_SCL_HCNT		(0x22)
#define IO_I2C_MASTER_SET_SS_SCL_LCNT		(0x23)
#define IO_I2C_MASTER_SET_FS_SCL_HCNT		(0x24)
#define IO_I2C_MASTER_SET_FS_SCL_LCNT		(0x25)

#define DFSS_IIC_GINT_DISABLED			(0)		/*!< dfss interrupt disabled for control iic irq/fiq */
#define DFSS_IIC_GINT_ENABLE			(1<<0)		/*!< dfss interrupt enabled for control iic irq/fiq */
#define DFSS_IIC_TXINT_ENABLE			(1<<1)		/*!< dfss interrupt enabled for control transmit process */
#define DFSS_IIC_RXINT_ENABLE			(1<<2)		/*!< dfss interrupt enabled for control transmit process */

enum {
	DFSS_IIC_CAP_LOADING_100PF = 0,
	DFSS_IIC_CAP_LOADING_400PF
};

/**
 * \brief	DFSS IIC control structure definition
 * \details	implement of dev_iic_info::iic_ctrl
 */
typedef struct dfss_iic_ctrl {
	/* Variables which should be set during object implementation */
	uint32_t dev_id;			/* Device ID */
	uint32_t ic_clkhz;			/*!< IC clock in HZ */
	uint32_t ic_caploading;			/*!< I2C Bus cap loading pf */
	uint32_t support_modes;			/*!< supported iic modes */
	uint32_t tx_fifo_len;			/*!< transmit fifo length */
	uint32_t rx_fifo_len;			/*!< receive fifo length */
	uint32_t iic_master_code;		/*!< value for IC_HS_MADDR */
	uint32_t retry_cnt;			/*!< retry count for TX or RX */
	uint32_t intno;				/*!< iic interrupt vector number */
	INT_HANDLER dfss_iic_int_handler;	/*!< iic interrupt handler */
	/* Variables which always change during iic operation */
	uint32_t int_status;			/*!< iic interrupt status */
	uint32_t iic_tx_over;			/*!< iic tx overflow count */
	uint32_t iic_rx_over;			/*!< iic rx overflow count */
} DFSS_IIC_CTRL, *DFSS_IIC_CTRL_PTR;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup	DEVICE_DFSS_IIC_FUNCDLR	DFSS IIC Function Declaration
 * \ingroup	DEVICE_DFSS_IIC
 * \brief	Contains declarations of dfss iic functions.
 * \details	This are only used in iic object implementation source file
 * @{
 */
extern int32_t dfss_iic_open (DEV_IIC *iic_obj, uint32_t mode, uint32_t param);
extern int32_t dfss_iic_close (DEV_IIC *iic_obj);
extern int32_t dfss_iic_control(DEV_IIC *iic_obj, uint32_t ctrl_cmd, void *param);
extern int32_t dfss_iic_write(DEV_IIC *iic_obj, const void *data, uint32_t len);
extern int32_t dfss_iic_read(DEV_IIC *iic_obj, void * data, uint32_t len);
extern void dfss_iic_isr(DEV_IIC *iic_obj, void *ptr);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* I2C_MASTER_H_ */
