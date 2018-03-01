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
 * \ingroup	DEVICE_DFSS_IIC
 * \brief	dfss IIC driver hardware description related header file
 * \details	detailed hardware related definitions of DFSS IIC driver
 */

#ifndef _DEVICE_DFSS_IIC_HAL_H_
#define _DEVICE_DFSS_IIC_HAL_H_

#include "dfss_iic_hal_cfg.h"
#include "../ioctl.h"

/** Enable DFSS IIC */
#define DFSS_IIC_ENABLE			(1)
/** Disable DFSS IIC */
#define DFSS_IIC_DISABLE		(0)

/*  EIA I2C device registers */
#define I2C_CON				(0x00)
#define I2C_TAR				(0x04)	// master only
#define I2C_SAR				(0x08)	// slave only
#define I2C_DATA_CMD			(0x10)
#define I2C_SS_SCL_HCNT			(0x14)
#define I2C_SS_SCL_LCNT			(0x18)
#define I2C_FS_SCL_HCNT			(0x1c)
#define I2C_FS_SCL_LCNT			(0x20)
#define I2C_INTR_STAT			(0x2c)
#define I2C_INTR_MASK			(0x30)
#define I2C_RAW_INTR_STAT		(0X34)
#define I2C_RX_TL			(0x38)
#define I2C_TX_TL			(0x3c)
#define I2C_CLR_INTR			(0x40)
#define I2C_CLR_RX_UNDER		(0x44)
#define I2C_CLR_RX_OVER			(0x48)
#define I2C_CLR_TX_OVER			(0x4c)
#define I2C_CLR_RD_REQ			(0x50)
#define I2C_CLR_TX_ABRT			(0x54)
#define I2C_CLR_STOP_DET		(0x60)
#define I2C_ENABLE			(0x6c)
#define I2C_STATUS			(0x70)
#define I2C_TXFLR			(0x74)
#define I2C_RXFLR			(0x78)
#define I2C_SDA_HOLD			(0x7c)
#define I2C_TX_ABRT_SOURCE		(0x80)
#define I2C_SDA_SETUP			(0x94)	// slave only
#define	I2C_ENABLE_STATUS		(0x9c)
#define I2C_FS_SPKLEN			(0xa0)
#define I2C_CLKEN			(0xc0)

/** Fields of IC_CON register */
/* DW_APB I2C IP Config Dependencies. */
#if DFSS_IIC_ALLOW_RESTART
#define I2C_CON_RESTART_EN		(1 << 5)
#else
#define I2C_CON_RESTART_EN		(0x00)
#endif

/* Master Addressing Mode Config */
#if DFSS_IIC_MST_10_BIT_ADDR_SUPPORT
#define MST_10_BIT_ADDR_MODE		(1 << 4)
#define I2C_10BITADDR_MASTER		(1 << 12)
#else
#define MST_10_BIT_ADDR_MODE		(0x00)
#define I2C_10BITADDR_MASTER		(0x00)
#endif

/* Slave Addressing Mode Config */
#if DFSS_IIC_SLV_10_BIT_ADDR_SUPPORT
#define SLV_10_BIT_ADDR_MODE		(1 << 3)
#else
#define SLV_10_BIT_ADDR_MODE		(0x00)
#endif

#if DFSS_IIC_SPECIAL_START_BYTE
#define I2C_TAR_SPECIAL			(1 << 11)
#define I2C_TAR_GC_OR_START		(1 << 10)
#else
#define I2C_TAR_SPECIAL			(0x00)
#define I2C_TAR_GC_OR_START		(0x00)
#endif

/** 7bit I2C address mask for target address register */
#define I2C_TAR_7BIT_ADDR_MASK		(0x7F)
/** 7bit I2C address mask for slave address register */
#define I2C_SAR_7BIT_ADDR_MASK		(0x7F)
/** 10bit I2C address mask for target address register */
#define I2C_TAR_10BIT_ADDR_MASK		(0x3FF)
/** 10bit I2C address mask for slave address register */
#define I2C_SAR_10BIT_ADDR_MASK		(0x3FF)

/** Speed modes of I2C_CON */
#define I2C_CON_SPEED_MASK		(0x03 << 1)
#define I2C_CON_SPEED_STANDARD		(0x01 << 1)
#define I2C_CON_SPEED_FAST		(0x02 << 1)

/** Working mode of I2C_CON */
#define I2C_CON_MST_SLV_MODE_MASK	(0x41)
#define I2C_CON_ENA_MASTER_MODE		(0x41)
#define I2C_CON_ENA_SLAVE_MODE		(0)

/* I2C interrupt control */
#define I2C_INT_DISABLE_ALL		(0x0)
#define I2C_INT_ENABLE_ALL		(0x7FF)

/* Interrupt Register Fields */
#define I2C_INTR_STAT_GEN_CALL		(1 << 11)
#define I2C_INTR_STAT_START_DET		(1 << 10)
#define I2C_INTR_STAT_STOP_DET		(1 << 9)
#define I2C_INTR_STAT_ACTIVITY		(1 << 8)
#define I2C_INTR_STAT_RX_DONE		(1 << 7)
#define I2C_INTR_STAT_TX_ABRT		(1 << 6)
#define I2C_INTR_STAT_RD_REQ		(1 << 5)
#define I2C_INTR_STAT_TX_EMPTY		(1 << 4)
#define I2C_INTR_STAT_TX_OVER		(1 << 3)
#define I2C_INTR_STAT_RX_FULL		(1 << 2)
#define I2C_INTR_STAT_RX_OVER		(1 << 1)
#define I2C_INTR_STAT_RX_UNDER		(1 << 0)

/* Interrupt enable mask as master */
#define I2C_INT_MST_TX_ENABLE		(I2C_INTR_STAT_TX_EMPTY|I2C_INTR_STAT_TX_OVER|I2C_INTR_STAT_TX_ABRT)
#define I2C_INT_MST_RX_ENABLE		(I2C_INTR_STAT_TX_EMPTY|I2C_INTR_STAT_RX_FULL|I2C_INTR_STAT_RX_OVER|I2C_INTR_STAT_RX_UNDER|I2C_INTR_STAT_TX_ABRT)

/* Interrupt enable mask as master */
#define I2C_INT_SLV_COMMON_ENABLE	(I2C_INTR_STAT_START_DET|I2C_INTR_STAT_STOP_DET)
#define I2C_INT_SLV_TX_ENABLE		(I2C_INTR_STAT_RD_REQ|I2C_INTR_STAT_TX_ABRT)
#define I2C_INT_SLV_RX_ENABLE		(I2C_INTR_STAT_RX_FULL|I2C_INTR_STAT_RX_OVER|I2C_INTR_STAT_RX_UNDER)

/* I2C_ENABLE_STATUS Bits */
#define I2C_ENABLE_STATUS_IC_EN		(1 << 0)
#define I2C_ENABLE_STATUS_SLV_DIS	(1 << 1)
#define I2C_ENABLE_STATUS_SLV_RX_LOST	(1 << 2)

/* IIC TX & RX threshold settings */
#define I2C_TX_THRESHOLD		(0)
#define I2C_RX_THRESHOLD		(0)

/* DFSS IIC (STATUS) Status Register Fields. */
#define I2C_STATUS_ACTIVITY		(0x01)
#define I2C_STATUS_TFNF			(0x01 << 1)
#define I2C_STATUS_TFE			(0x01 << 2)
#define I2C_STATUS_RFNE			(0x01 << 3)
#define I2C_STATUS_RFF			(0x01 << 4)
#define I2C_STATUS_MASTER_ACT		(0x01 << 5)
#define I2C_STATUS_SLAVE_ACT		(0x01 << 6)

/* I2C_TX_ABRT_SOURCE Register Bit Fields */
#define I2C_TX_ABRT_7B_ADDR_NOACK	(1 << 0)
#define I2C_TX_ABRT_10ADDR1_NOACK	(1 << 1)
#define I2C_TX_ABRT_10ADDR2_NOACK	(1 << 2)
#define I2C_TX_ABRT_TXDATA_NOACK	(1 << 3)
#define I2C_TX_ABRT_GCALL_NOACK		(1 << 4)
#define I2C_TX_ABRT_GCALL_READ		(1 << 5)
#define I2C_TX_ABRT_HS_ACKDET		(1 << 6)
#define I2C_TX_ABRT_SBYTE_ACKDET	(1 << 7)
#define I2C_TX_ABRT_HS_NORSTRT		(1 << 8)
#define I2C_TX_ABRT_SBYTE_NORSTRT	(1 << 9)
#define I2C_TX_ABRT_10B_RD_NORSTRT	(1 << 10)
#define I2C_TX_ABRT_MASTER_DIS		(1 << 11)
#define I2C_TX_ABRT_ARB_LOST		(1 << 12)
#define I2C_TX_ABRT_SLVFLUSH_TXFIFO	(1 << 13)
#define I2C_TX_ABRT_SLV_ARBLOST		(1 << 14)
#define I2C_TX_ABRT_SLVRD_INTX		(1 << 15)

/* Combined bits for iic abort source as master */
#define I2C_MST_ABRT_ADDR_NOACK		(I2C_TX_ABRT_7B_ADDR_NOACK | I2C_TX_ABRT_10ADDR1_NOACK)
#define I2C_MST_ABRT_LOST_BUS		(I2C_TX_ABRT_ARB_LOST)
#define I2C_MST_ABRT_DATA_NOACK		(I2C_TX_ABRT_TXDATA_NOACK)

/* I2C IOCTLs default values	 	*/
#define I2C_SS_SCL_HIGH_COUNT		(0x0190)//WHY? hw-default is 0x30
#define I2C_SS_SCL_LOW_COUNT		(0x01d6)//WHY? hw-default is 0x38
#define I2C_FS_SCL_HIGH_COUNT		(0x003c)
#define I2C_FS_SCL_LOW_COUNT		(0x0082)

/* Other macros. */
#define I2C_RESTART_CMD			(0x01 << 10)
#define I2C_STOP_CMD			(0x01 << 9)
#define I2C_READ_CMD			(0x01 << 8)
#define I2C_NONE_CMD			(0)
#define	DMA_NONE			(0xff)

/** @} */

#endif /* _DEVICE_DW_IIC_HAL_H_ */
