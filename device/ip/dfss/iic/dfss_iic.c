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
 * \defgroup	DEVICE_DW_IIC	Designware IIC Driver
 * \ingroup	DEVICE_DW
 * \brief	Designware IIC Driver Implementation
 */

/**
 * \file
 * \brief	Designware iic driver
 * \ingroup	DEVICE_DW_IIC
 */
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "dfss_iic_hal.h"
#include "dfss_iic.h"
#include "io_config.h"
#include "apexextensions.h"
#include "embARC_error.h"

/** check expressions used in DFSS IIC driver implementation */
#define DFSS_IIC_CHECK_EXP(EXPR, ERROR_CODE)		CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

#ifndef DISABLE_DEVICE_OBJECT_VALID_CHECK
/** valid check of iic info object */
#define VALID_CHK_IIC_INFO_OBJECT(iicinfo_obj_ptr)		{			\
			DFSS_IIC_CHECK_EXP((iicinfo_obj_ptr)!=NULL, E_OBJ);		\
			DFSS_IIC_CHECK_EXP(((iicinfo_obj_ptr)->iic_ctrl)!=NULL, E_OBJ);	\
		}
#endif

/* interrupt callback routines select macros definitions */
#define DFSS_IIC_RDY_SND		(0x1)	/* ready to send callback */
#define DFSS_IIC_RDY_RCV		(0x2)	/* ready to receive callback */

/* Notes:
 *
 */

#ifdef IO_I2C_MST0_PRESENT
#define I2C_DEV_PRESENT
#endif /* IO_I2C_MST0_PRESENT */
#ifdef IO_I2C_MST1_PRESENT
#define I2C_DEV_PRESENT
#endif /* IO_I2C_MST1_PRESENT */
#ifdef IO_I2C_MST2_PRESENT
#define I2C_DEV_PRESENT
#endif /* IO_I2C_MST2_PRESENT */
#ifdef IO_I2C_MST3_PRESENT
#define I2C_DEV_PRESENT
#endif /* IO_I2C_MST3_PRESENT */

#ifdef I2C_DEV_PRESENT

#define I2C_MAX_CNT	(4)

typedef _Interrupt void (*I2C_ISR) ();

/* Private data structure maintained by the driver */
typedef struct i2c_info {
	uint8_t		instID;
	/* TX & RX Buffer and lengths */
	uint8_t		*rx_data;
	uint8_t		handling_tx;
	uint8_t		handling_rx;
	uint8_t		stop_detected;
	uint8_t		rd_req_detected;
	uint8_t		*tx_data;
	uint16_t	fifo_depth;
	uint16_t	dmarxchanid, dmatxchanid;
	uint32_t	reg_base;		// base address of device register set
	uint32_t	tx_size;
	uint32_t	tx_count;
	uint32_t	*p_txsize;
	uint32_t	rx_size;
	uint32_t	rx_count;
	uint32_t	*p_rxsize;
	uint32_t	rx_req_count;
	/* Callbacks */
	IO_CB_FUNC	tx_cb;
	IO_CB_FUNC	rx_cb;
	IO_CB_FUNC	err_cb;
	/* Interrupt numbers and handlers */
	uint8_t		vector_err;
	uint8_t		vector_rx_avail;
	uint8_t		vector_tx_req;
	uint8_t		vector_stop_det;
	uint8_t		vector_rd_req;		// slave only
	uint8_t		vector_restart_det;	// slave only
	I2C_ISR		isr_err;
	I2C_ISR		isr_rx_avail;
	I2C_ISR		isr_tx_req;
	I2C_ISR		isr_stop_det;
	I2C_ISR		isr_rd_req;
} i2c_info_t, *i2c_info_pt;

/* I2C master devices private data structures */
static i2c_info_pt i2c_handles[I2C_MAX_CNT] = { 0 };

static i2c_info_t   i2c_master_devs[] = {
#ifdef IO_I2C_MST0_PRESENT
		{
			.instID = 0,
			.reg_base = AR_IO_I2C_MST0_CON,
			.dmarxchanid = DMA_NONE,
			.dmatxchanid = DMA_NONE,
			.fifo_depth = IO_I2C_MST0_FS,
			.vector_err = IO_I2C_MST0_INT_ERR,
			// .isr_err = i2c_mst0_err_ISR,
			.vector_rx_avail = IO_I2C_MST0_INT_RX_AVAIL,
			// .isr_rx_avail = i2c_mst0_rx_avail_ISR,
			.vector_tx_req = IO_I2C_MST0_INT_TX_REQ,
			// .isr_tx_req = i2c_mst0_tx_req_ISR,
			.vector_stop_det = IO_I2C_MST0_INT_STOP_DET,
			// .isr_stop_det = i2c_mst0_stop_det_ISR
		},
#endif
#ifdef IO_I2C_MST1_PRESENT
		{
			.instID = 1,
			.reg_base = AR_IO_I2C_MST1_CON,
			.dmarxchanid = DMA_NONE,
			.dmatxchanid = DMA_NONE,
			.fifo_depth = IO_I2C_MST1_FS,
			.vector_err = IO_I2C_MST1_INT_ERR,
			// .isr_err = i2c_mst1_err_ISR,
			.vector_rx_avail = IO_I2C_MST1_INT_RX_AVAIL,
			// .isr_rx_avail = i2c_mst1_rx_avail_ISR,
			.vector_tx_req = IO_I2C_MST1_INT_TX_REQ,
			// .isr_tx_req = i2c_mst1_tx_req_ISR,
			.vector_stop_det = IO_I2C_MST1_INT_STOP_DET,
			// .isr_stop_det = i2c_mst1_stop_det_ISR
		},
#endif
#ifdef IO_I2C_MST2_PRESENT
 		{
 			.instID = 2,
 			.reg_base = AR_IO_I2C_MST2_CON,
 			.dmarxchanid = DMA_NONE,
 			.dmatxchanid = DMA_NONE,
 			.fifo_depth = IO_I2C_MST2_FS,
 			.vector_err = IO_I2C_MST2_INT_ERR,
 			// .isr_err = i2c_mst2_err_ISR,
 			.vector_rx_avail = IO_I2C_MST2_INT_RX_AVAIL,
 			// .isr_rx_avail = i2c_mst2_rx_avail_ISR,
 			.vector_tx_req = IO_I2C_MST2_INT_TX_REQ,
 			// .isr_tx_req = i2c_mst2_tx_req_ISR,
 			.vector_stop_det = IO_I2C_MST2_INT_STOP_DET,
 			// .isr_stop_det = i2c_mst2_stop_det_ISR
 		},
#endif
#ifdef IO_I2C_MST3_PRESENT
		{
			.instID = 3,
			.reg_base = AR_IO_I2C_MST3_CON,
			.dmarxchanid = DMA_NONE,
			.dmatxchanid = DMA_NONE,
			.fifo_depth = IO_I2C_MST3_FS,
			.vector_err = IO_I2C_MST3_INT_ERR,
			.isr_err = i2c_mst3_err_ISR,
			.vector_rx_avail = IO_I2C_MST3_INT_RX_AVAIL,
			.isr_rx_avail = i2c_mst3_rx_avail_ISR,
			.vector_tx_req = IO_I2C_MST3_INT_TX_REQ,
			.isr_tx_req = i2c_mst3_tx_req_ISR,
			.vector_stop_det = IO_I2C_MST3_INT_STOP_DET,
			.isr_stop_det = i2c_mst3_stop_det_ISR
		},
#endif
		{ .instID = I2C_MAX_CNT }
	};

#define REG_WRITE( reg, x )	_arc_sr((unsigned)(x), (unsigned)(dev->reg_base + reg))
#define REG_READ( reg )		_arc_lr((unsigned)(dev->reg_base + reg))
#define REG_WRITE_BITS( reg, x, y, len, pos )	REG_WRITE( reg, ( (((x) & ~( (~(0xffffffff << len)) << pos )) \
										| (((y) << pos) & ((~(0xffffffff << len)) << pos)))))
/**
 * \defgroup	DEVICE_DFSS_IIC_STATIC	DFSS IIC Driver Static Functions
 * \ingroup	DEVICE_DFSS_IIC
 * \brief	Static or inline functions, variables for DFSS IIC handle iic operations,
 * 	only used in this file.
 * @{
 */

/** Disable dfss iic device */
Inline void dfss_iic_disable(uint32_t dev_id)
{
	uint32_t res;
	i2c_info_pt dev = i2c_handles[dev_id];
	res = REG_READ( I2C_ENABLE );
	REG_WRITE( I2C_ENABLE, (res & ~(0x1)) );
}
/** Enable dfss iic device */
Inline void dfss_iic_enable(uint32_t dev_id)
{
	uint32_t res;
	i2c_info_pt dev = i2c_handles[dev_id];
	res = REG_READ( I2C_ENABLE );
	REG_WRITE( I2C_ENABLE, (res | 0x1) );
}
/** Clear all dfss iic interrupt */
Inline void dfss_iic_clear_interrupt_all(uint32_t dev_id)
{
	i2c_info_pt dev = i2c_handles[dev_id];
	(void)REG_READ(I2C_CLR_INTR);
}

/** Enable dfss iic bit interrupt with mask */
Inline void dfss_iic_unmask_interrupt(uint32_t dev_id,uint32_t mask)
{
	uint32_t res;
	i2c_info_pt dev = i2c_handles[dev_id];
	res = REG_READ(I2C_INTR_MASK);
	REG_WRITE(I2C_INTR_MASK,res | mask);
}

/** Enable dfss iic bit interrupt with mask */
Inline void dfss_iic_mask_interrupt(uint32_t dev_id,uint32_t mask)
{
	uint32_t res;
	i2c_info_pt dev = i2c_handles[dev_id];
	res = REG_READ(I2C_INTR_MASK);
	REG_WRITE(I2C_INTR_MASK,res & (~mask));
}

/** Set dfss iic transfer target address for addressing any iic slave device as a master */
Inline void dfss_iic_set_taraddr(DEV_IIC_INFO *iic_info_ptr,uint32_t dev_id,uint32_t param)
{
	uint32_t val32,res;
	i2c_info_pt dev = i2c_handles[dev_id];
	res = REG_READ(I2C_TAR);
	if (iic_info_ptr->addr_mode == IIC_7BIT_ADDRESS) {
		res &= (~IIC_7BIT_ADDRESS_MASK);
		val32 = ((uint32_t)param) & IIC_7BIT_ADDRESS_MASK;
	} else {
		res &= (IIC_10BIT_ADDRESS_MASK);
		val32 = ((uint32_t)param) & IIC_10BIT_ADDRESS_MASK;
	}
	dfss_iic_disable(dev_id);
	while ((I2C_ENABLE_STATUS_IC_EN & REG_READ( I2C_ENABLE_STATUS )) != 0);
	REG_WRITE(I2C_TAR, val32 | res);
	iic_info_ptr->tar_addr = val32;
	dfss_iic_enable(dev_id);
}

/** Select speed mode, and return proper speed mode configuration */
Inline uint32_t dfss_iic_select_speedmode(uint32_t speedmode)
{
	uint32_t speedcfg;
	/* The iotdev is only supported fast and standard now */
	if (speedmode == IIC_SPEED_STANDARD) {
		speedcfg = I2C_CON_SPEED_STANDARD;
	} else if (speedmode == IIC_SPEED_FAST) {
		speedcfg = I2C_CON_SPEED_FAST;
	} else {
		speedcfg = I2C_CON_SPEED_FAST;
	}
	return speedcfg;
}

/** Set designware iic speed mode */
Inline void dfss_iic_set_speedmode(uint32_t dev_id, uint32_t speedmode)
{
	uint32_t ic_con_val;
	i2c_info_pt dev = i2c_handles[dev_id];

	dfss_iic_disable(dev_id);
	ic_con_val = REG_READ(I2C_CON) & (~I2C_CON_SPEED_MASK);
	ic_con_val |= dfss_iic_select_speedmode(speedmode);
	while ((I2C_ENABLE_STATUS_IC_EN & REG_READ( I2C_ENABLE_STATUS )) != 0) ;
	REG_WRITE(I2C_CON, ic_con_val);
	dfss_iic_enable(dev_id);
}

/** Set dfss iic slave address as a slave */
Inline void dfss_iic_set_slvaddr(uint32_t dev_id, uint32_t address)
{
	uint32_t res;
	i2c_info_pt dev = i2c_handles[dev_id];
	dfss_iic_disable(dev_id);
	res = REG_READ(I2C_SAR);
	REG_WRITE(I2C_SAR,(address & I2C_SAR_10BIT_ADDR_MASK) | (res));
	dfss_iic_enable(dev_id);
}
/** Set designware working mode as master or slave */
Inline void dfss_iic_set_working_mode(uint32_t dev_id, uint32_t mode)
{
	uint32_t ic_con_val;
	i2c_info_pt dev = i2c_handles[dev_id];
	dfss_iic_disable(dev_id);
	ic_con_val = REG_READ(I2C_CON) & (~I2C_CON_MST_SLV_MODE_MASK);
	if (mode == DEV_MASTER_MODE) {
		ic_con_val |= I2C_CON_ENA_MASTER_MODE;
	} else {
		ic_con_val |= I2C_CON_ENA_SLAVE_MODE;
	}
	REG_WRITE(I2C_CON,ic_con_val);
	dfss_iic_enable(dev_id);
}

Inline void dfss_iic_flush_tx(uint32_t dev_id)
{
	(void)dev_id;
}

Inline void dfss_iic_flush_rx(uint32_t dev_id)
{
	(void)dev_id;
}

/** Set dfss iic transfer in 7bit of 10bit addressing mode as a master */
Inline void dfss_iic_set_mstaddr_mode(uint32_t dev_id, uint32_t mode)
{
	uint32_t res;
	i2c_info_pt	  dev = i2c_handles[dev_id];
	dfss_iic_disable(dev_id);
	if (mode == IIC_7BIT_ADDRESS) {
		res = REG_READ(I2C_CON);
		res &= ~(MST_10_BIT_ADDR_MODE);
		REG_WRITE(I2C_CON,res);
	} else {
		res = REG_READ(I2C_CON);
		res |= MST_10_BIT_ADDR_MODE;
		REG_WRITE(I2C_CON,res);
	}
	dfss_iic_enable(dev_id);
}

/** Set dfss iic transfer in 7bit of 10bit addressing mode as a slave */
Inline void dfss_iic_set_slvaddr_mode(uint32_t dev_id, uint32_t mode)
{
	uint32_t res;
	i2c_info_pt dev = i2c_handles[dev_id];
	dfss_iic_disable(dev_id);
	if (mode == IIC_7BIT_ADDRESS) {
		res = REG_READ(I2C_CON);
		res &= ~SLV_10_BIT_ADDR_MODE;
		REG_WRITE(I2C_CON,res);
	} else {
		res = REG_READ(I2C_CON);
		res |= SLV_10_BIT_ADDR_MODE;
		REG_WRITE(I2C_CON,res);
	}
	dfss_iic_enable(dev_id);
}

/** test whether iic is ready to write, 1 ready, 0 not ready */
Inline int32_t dfss_iic_master_putready(uint32_t dev_id)
{
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t status = REG_READ(I2C_STATUS);
	return ((status & I2C_STATUS_TFNF) != 0);
}

/** test whether iic is ready to read, 1 ready, 0 not ready */
Inline int32_t dfss_iic_master_getready(uint32_t dev_id)
{
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t status = REG_READ(I2C_STATUS);
	return ((status & I2C_STATUS_RFNE) != 0);
}

/** test whether iic is ready to read, 1 ready, 0 not ready */
Inline int32_t dfss_iic_master_putover(uint32_t dev_id)
{
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t status = REG_READ(I2C_STATUS);
	return ((status & I2C_STATUS_TFE) != 0);
}

/** enable designware iic */
static void dfss_iic_enable_device(DEV_IIC_INFO *iic_info_ptr,uint32_t dev_id)
{
	if ((iic_info_ptr->status & DEV_ENABLED) == 0) {
		dfss_iic_enable(dev_id);
		iic_info_ptr->status |= DEV_ENABLED;
	}
}

/** disable designware iic */
static void dfss_iic_disable_device(DEV_IIC_INFO *iic_info_ptr,uint32_t dev_id)
{
	uint32_t i;
	i2c_info_pt dev = i2c_handles[dev_id];

	for (i=0; i<DFSS_IIC_DISABLE_MAX_T_POLL_CNT; i++) {
		dfss_iic_disable(dev_id);
		if ((REG_READ(I2C_ENABLE_STATUS) & REG_READ(I2C_ENABLE_STATUS_IC_EN)) == 0) {
			break;
		}
	}
	iic_info_ptr->status &= ~DEV_ENABLED;
}

static void dfss_iic_reset_device(DEV_IIC_INFO *iic_info_ptr,uint32_t dev_id)
{
	DFSS_IIC_CTRL *iic_ctrl_ptr = (DFSS_IIC_CTRL *)(iic_info_ptr->iic_ctrl);

	dfss_iic_disable_device(iic_info_ptr, dev_id);
	dfss_iic_clear_interrupt_all(dev_id);
	iic_info_ptr->next_cond = IIC_MODE_STOP;
	iic_info_ptr->cur_state = IIC_FREE;
	iic_info_ptr->err_state = IIC_ERR_NONE;
	iic_ctrl_ptr->iic_tx_over = 0;
	iic_ctrl_ptr->iic_rx_over = 0;
	dfss_iic_enable_device(iic_info_ptr, dev_id);
}

/** Disable iic master interrupt for transmit or receive */
static void dfss_iic_mst_dis_cbr(DFSS_IIC_CTRL *iic_ctrl_ptr,uint32_t dev_id, uint32_t cbrtn)
{
	switch (cbrtn) {
		case DFSS_IIC_RDY_SND:
			dfss_iic_mask_interrupt(dev_id, I2C_INT_MST_TX_ENABLE);
			iic_ctrl_ptr->int_status &= ~DFSS_IIC_TXINT_ENABLE;
			break;
		case DFSS_IIC_RDY_RCV:
			dfss_iic_mask_interrupt(dev_id, I2C_INT_MST_RX_ENABLE);
			iic_ctrl_ptr->int_status &= ~DFSS_IIC_RXINT_ENABLE;
			break;
		default:
			break;
	}
}

/** Disable iic slave interrupt for transmit or receive */
static void dfss_iic_slv_dis_cbr(DFSS_IIC_CTRL *iic_ctrl_ptr, uint32_t dev_id, uint32_t cbrtn)
{
	switch (cbrtn) {
		case DFSS_IIC_RDY_SND:
			dfss_iic_mask_interrupt(dev_id, I2C_INT_SLV_TX_ENABLE);
			iic_ctrl_ptr->int_status &= ~DFSS_IIC_TXINT_ENABLE;
			break;
		case DFSS_IIC_RDY_RCV:
			dfss_iic_mask_interrupt(dev_id, I2C_INT_SLV_RX_ENABLE);
			iic_ctrl_ptr->int_status &= ~DFSS_IIC_RXINT_ENABLE;
			break;
		default:
			break;
	}
}

/** Enable iic master interrupt for transmit or receive */
static void dfss_iic_mst_ena_cbr(DFSS_IIC_CTRL *iic_ctrl_ptr, uint32_t dev_id,uint32_t cbrtn)
{
	switch (cbrtn) {
		case DFSS_IIC_RDY_SND:
			iic_ctrl_ptr->int_status |= DFSS_IIC_TXINT_ENABLE;
			dfss_iic_unmask_interrupt(dev_id, I2C_INT_MST_TX_ENABLE);
			break;
		case DFSS_IIC_RDY_RCV:
			iic_ctrl_ptr->int_status |= DFSS_IIC_RXINT_ENABLE;
			dfss_iic_unmask_interrupt(dev_id, I2C_INT_MST_RX_ENABLE);
			break;
		default:
			break;
	}
}

/** Enable iic slave interrupt for transmit or receive */
static void dfss_iic_slv_ena_cbr(DFSS_IIC_CTRL *iic_ctrl_ptr, uint32_t dev_id, uint32_t cbrtn)
{
	switch (cbrtn) {
		case DFSS_IIC_RDY_SND:
			iic_ctrl_ptr->int_status |= DFSS_IIC_TXINT_ENABLE;
			dfss_iic_unmask_interrupt(dev_id, I2C_INT_SLV_TX_ENABLE);
			break;
		case DFSS_IIC_RDY_RCV:
			iic_ctrl_ptr->int_status |= DFSS_IIC_RXINT_ENABLE;
			dfss_iic_unmask_interrupt(dev_id, I2C_INT_SLV_RX_ENABLE);
			break;
		default:
			break;
	}
}

/**
 * \brief		disable dfss iic send or receive interrupt
 * \param[in]	DEV_IIC_INFO 	*iic_info_ptr
 * \param[in]	cbrtn		control code of callback routine of send or receive
 */
static void dfss_iic_dis_cbr(DEV_IIC_INFO *iic_info_ptr, uint32_t dev_id, uint32_t cbrtn)
{
	DFSS_IIC_CTRL *iic_ctrl_ptr = (DFSS_IIC_CTRL *)(iic_info_ptr->iic_ctrl);

	if (iic_info_ptr->mode == DEV_MASTER_MODE) {
		dfss_iic_mst_dis_cbr(iic_ctrl_ptr, dev_id, cbrtn);
	} else {
		dfss_iic_slv_dis_cbr(iic_ctrl_ptr, dev_id, cbrtn);
	}
}

/**
 * \brief	enable DesignWare IIC send or receive interrupt
 * \param[in]	DEV_IIC_INFO 	*iic_info_ptr
 * \param[in]	cbrtn		control code of callback routine of send or receive
 */
static void dfss_iic_ena_cbr(DEV_IIC_INFO *iic_info_ptr, uint32_t dev_id, uint32_t cbrtn)
{
	DFSS_IIC_CTRL *iic_ctrl_ptr = (DFSS_IIC_CTRL *)(iic_info_ptr->iic_ctrl);

	if (iic_info_ptr->mode == DEV_MASTER_MODE) {
		dfss_iic_mst_ena_cbr(iic_ctrl_ptr,dev_id, cbrtn);
	} else {
		dfss_iic_slv_ena_cbr(iic_ctrl_ptr,dev_id, cbrtn);
	}
}

/**
 * \brief	enable dfss iic interrupt
 * \param	iic_info_ptr	iic information structure pointer
 */
static void dfss_iic_enable_interrupt(DEV_IIC_INFO *iic_info_ptr)
{
	(void)iic_info_ptr;
}

/**
 * \brief	disable dfss iic interrupt
 * \param	iic_info_ptr	iic information structure pointer
 */
static void dfss_iic_disable_interrupt(DEV_IIC_INFO *iic_info_ptr)
{
	(void)iic_info_ptr;
}

/** abort current interrupt transmit transfer */
static void dfss_iic_abort_tx(DEV_IIC *iic_obj)
{
	(void)iic_obj;
}

/** abort current interrupt receive transfer */
static void dfss_iic_abort_rx(DEV_IIC *iic_obj)
{
	(void)iic_obj;
}

/** Get available transmit fifo count */
static int32_t dfss_iic_get_txavail(uint32_t dev_id)
{
	int32_t tx_avail = 0;
	i2c_info_pt dev = i2c_handles[dev_id];
	tx_avail = dev->fifo_depth - REG_READ(I2C_TXFLR);
	return tx_avail;
}

/** Get available receive fifo count */
static int32_t dfss_iic_get_rxavail(uint32_t dev_id)
{
	int32_t rx_avail = 0;
	i2c_info_pt dev = i2c_handles[dev_id];
	rx_avail = REG_READ(I2C_RXFLR);
	return rx_avail;
}

/** Check error for IIC master device */
static int32_t dfss_iic_mst_chkerr(DFSS_IIC_CTRL *iic_ctrl_ptr)
{
	uint32_t status;
	int32_t ercd = IIC_ERR_NONE;
	i2c_info_pt dev = i2c_handles[iic_ctrl_ptr->dev_id];

	status = REG_READ(I2C_RAW_INTR_STAT);
	if (status & I2C_INTR_STAT_TX_ABRT) {
		status = REG_READ(I2C_TX_ABRT_SOURCE);
		if (status & I2C_MST_ABRT_LOST_BUS) {
			ercd = IIC_ERR_LOST_BUS;
		} else if (status & I2C_MST_ABRT_ADDR_NOACK) {
			ercd = IIC_ERR_ADDR_NOACK;
		} else if (status & I2C_MST_ABRT_DATA_NOACK) {
			ercd = IIC_ERR_DATA_NOACK;
		} else {
			ercd = IIC_ERR_UNDEF;
		}
		status = REG_READ(I2C_CLR_TX_ABRT);
	} else {
		if (status & I2C_INTR_STAT_TX_OVER) {
			iic_ctrl_ptr->iic_tx_over ++;
			status = REG_READ(I2C_CLR_TX_OVER);
		}
		if (status & (I2C_INTR_STAT_RX_OVER | I2C_INTR_STAT_RX_UNDER)) {
			iic_ctrl_ptr->iic_rx_over ++;
			status = REG_READ(I2C_CLR_RX_OVER);
			status = REG_READ(I2C_CLR_RX_UNDER);
		}
	}
	return ercd;
}

/** IIC Master transmit called in interrupt */
static void dfss_iic_mst_int_write(DEV_IIC *iic_obj)
{
	(void)iic_obj;
}

/** IIC Master receive called in interrupt */
static void dfss_iic_mst_int_read(DEV_IIC *iic_obj)
{
	(void)iic_obj;
}

/** IIC Slave transmit called in interrupt */
static void dfss_iic_slv_int_process(DEV_IIC *iic_obj)
{
	(void)iic_obj;
}

/**
 * \brief	Control iic by ctrl command
 * \param[in]		iic_obj	iic device object pointer
 * \param[in]		ctrl_cmd	\ref DEVICE_HAL_IIC_CTRLCMD "control command", to change or get some thing related to iic
 * \param[in,out]	param		parameters that maybe argument of the command,
 * 					or return values of the command, must not be NULL
 * \retval	E_OK	Control device successfully
 * \retval	E_CLSED	Device is not opened
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	Parameter is not valid for current control command
 * \retval	E_SYS	Control device failed, due to hardware issues, such as device is disabled
 * \retval	E_CTX	Control device failed, due to different reasons like in transfer state
 * \retval	E_NOSPT	Control command is not supported or not valid
 */
int32_t dfss_iic_control(DEV_IIC *iic_obj, uint32_t ctrl_cmd, void *param)
{
	int32_t ercd = E_OK;
	uint32_t val32;
	io_cb_t callback;
	DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
	DFSS_IIC_CTRL *iic_ctrl_ptr = (DFSS_IIC_CTRL *)(iic_info_ptr->iic_ctrl);
	i2c_info_pt dev = i2c_handles[iic_ctrl_ptr->dev_id];

	VALID_CHK_IIC_INFO_OBJECT(iic_info_ptr);

	/* check whether current device is disabled */
	if ((iic_info_ptr->status & DEV_ENABLED) == 0) {
		/** When device is disabled,
		 * only IIC_CMD_ENA_DEV, IIC_CMD_DIS_DEV, IIC_CMD_GET_STATUS, IIC_CMD_RESET
		 * are available, other commands will return E_SYS
		 */
		if ((ctrl_cmd != IIC_CMD_ENA_DEV) && \
			(ctrl_cmd != IIC_CMD_DIS_DEV) && \
			(ctrl_cmd != IIC_CMD_GET_STATUS) && \
			(ctrl_cmd != IIC_CMD_RESET) ) {
			return E_SYS;
		}
	}

	switch (ctrl_cmd) {
		case IIC_CMD_GET_STATUS:
			DFSS_IIC_CHECK_EXP((param!=NULL) && CHECK_ALIGN_4BYTES(param), E_PAR);
			*((int32_t *)param) = iic_info_ptr->status;
			break;
		case IIC_CMD_ENA_DEV:
			dfss_iic_enable_device(iic_info_ptr,iic_ctrl_ptr->dev_id);
			break;
		case IIC_CMD_DIS_DEV:
			dfss_iic_disable_device(iic_info_ptr,iic_ctrl_ptr->dev_id);
			break;
		case IIC_CMD_RESET:
			dfss_iic_reset_device(iic_info_ptr,iic_ctrl_ptr->dev_id);
			break;
		case IIC_CMD_FLUSH_TX:
			dfss_iic_flush_tx(iic_ctrl_ptr->dev_id);
			break;
		case IIC_CMD_FLUSH_RX:
			dfss_iic_flush_rx(iic_ctrl_ptr->dev_id);
			break;
		case IIC_CMD_SET_ADDR_MODE:
			val32 = (uint32_t)param;
			DFSS_IIC_CHECK_EXP((val32==IIC_7BIT_ADDRESS) || (val32==IIC_10BIT_ADDRESS), E_PAR);
			if (iic_info_ptr -> mode == DEV_MASTER_MODE) {
				dfss_iic_set_mstaddr_mode(iic_ctrl_ptr->dev_id,val32);
			} else {
				dfss_iic_set_slvaddr_mode(iic_ctrl_ptr->dev_id, val32);
			}
			iic_info_ptr->addr_mode = val32;
			break;
		case IIC_CMD_GET_RXAVAIL:
			DFSS_IIC_CHECK_EXP((param!=NULL) && CHECK_ALIGN_4BYTES(param), E_PAR);
			*((int32_t *)param) = dfss_iic_get_rxavail(iic_ctrl_ptr->dev_id);
			break;
		case IIC_CMD_GET_TXAVAIL:
			DFSS_IIC_CHECK_EXP((param!=NULL) && CHECK_ALIGN_4BYTES(param), E_PAR);
			*((int32_t *)param) = dfss_iic_get_txavail(iic_ctrl_ptr->dev_id);
			break;
		case IIC_CMD_SET_TXCB:
			DFSS_IIC_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
			callback.cb = param;
			dev->tx_cb  = ((io_cb_t *)(&callback))->cb;
			break;
		case IIC_CMD_SET_RXCB:
			DFSS_IIC_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
			callback.cb = param;
			dev->rx_cb  = ((io_cb_t *)(&callback))->cb;
			break;
		case IIC_CMD_SET_ERRCB:
			DFSS_IIC_CHECK_EXP(CHECK_ALIGN_4BYTES(param), E_PAR);
			callback.cb = param;
			dev->err_cb = ((io_cb_t *)(&callback))->cb;
			break;
		case IIC_CMD_ABORT_TX:
			ercd = E_NOSPT;
			break;
		case IIC_CMD_ABORT_RX:
			ercd = E_NOSPT;
			break;
		case IIC_CMD_SET_TXINT:
			val32 = (uint32_t)param;
			if (val32 == 0) {
				dfss_iic_dis_cbr(iic_info_ptr, iic_ctrl_ptr->dev_id, DFSS_IIC_RDY_SND);
			} else {
				dfss_iic_dis_cbr(iic_info_ptr, iic_ctrl_ptr->dev_id, DFSS_IIC_RDY_SND);
			}
			break;
		case IIC_CMD_SET_RXINT:
			val32 = (uint32_t)param;
			if (val32 == 0) {
				dfss_iic_dis_cbr(iic_info_ptr, iic_ctrl_ptr->dev_id, DFSS_IIC_RDY_RCV);
			} else {
				dfss_iic_dis_cbr(iic_info_ptr, iic_ctrl_ptr->dev_id, DFSS_IIC_RDY_RCV);
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
			DFSS_IIC_CHECK_EXP((val32 >= IIC_SPEED_STANDARD) && (val32 <= IIC_SPEED_ULTRA), E_PAR);
			dfss_iic_set_speedmode(iic_ctrl_ptr->dev_id, val32);
			break;
		case IIC_CMD_MST_SET_TAR_ADDR:
			dfss_iic_set_taraddr(iic_info_ptr,iic_ctrl_ptr->dev_id,(uint32_t)param);
			break;
		case IIC_CMD_MST_SET_NEXT_COND:
			DFSS_IIC_CHECK_EXP(iic_info_ptr->mode == DEV_MASTER_MODE, E_NOSPT);
			val32 = (uint32_t)param;
			DFSS_IIC_CHECK_EXP((val32==IIC_MODE_STOP) || (val32==IIC_MODE_RESTART), E_PAR);
			iic_info_ptr->next_cond = (uint32_t)param;
			break;
		/* Slave mode only commands */
		case IIC_CMD_SLV_SET_SLV_ADDR:
			ercd = E_NOSPT;
			break;
		case IIC_CMD_SLV_GET_SLV_STATE:
			ercd = E_NOSPT;
			break;
		default:
			ercd = E_NOSPT;
			break;
	}

error_exit:
	return ercd;
}

/* API functions,init dfss IIC device into master mode*/
static uint32_t dfss_i2c_master_open( uint32_t dev_id )
{
	uint32_t h=0;
	i2c_info_pt dev;

	h = 0;
	while ((i2c_master_devs[h].instID != dev_id) && (i2c_master_devs[h].instID != I2C_MAX_CNT)) {
		h++;
	}
	if ((i2c_master_devs[h].instID == I2C_MAX_CNT) || (0 != i2c_handles[dev_id])) {
		/* dev_id not part of design, or still open */
		return 1;
	}
	i2c_handles[dev_id] = &i2c_master_devs[h];
	dev = i2c_handles[dev_id];

	/* enable clock */
	REG_WRITE( I2C_CLKEN, 0x1 );

	/* mask all interrupts */
	REG_WRITE( I2C_INTR_MASK, 0 );

	/* initialize */
	dev->handling_tx = dev->handling_rx = 0;
	dev->tx_data = dev->rx_data = NULL;

	/* set interrupt vector */
	_setvecti( dev->vector_err, dev->isr_err);
	_setvecti( dev->vector_rx_avail, dev->isr_rx_avail);
	_setvecti( dev->vector_tx_req, dev->isr_tx_req);
	_setvecti( dev->vector_stop_det, dev->isr_stop_det);

	/* enable device */
	REG_WRITE( I2C_ENABLE, 0x1 );

	/* unmask error interrupt: stop-detection, tx-abort, rx-over */
	REG_WRITE( I2C_INTR_MASK, I2C_INTR_STAT_STOP_DET | I2C_INTR_STAT_TX_ABRT | I2C_INTR_STAT_RX_OVER);

	return 0;
}

static void dfss_i2c_master_close( uint32_t dev_id )
{
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t enable = REG_READ( I2C_ENABLE );

	/* clear all interrupts */
	REG_WRITE( I2C_INTR_MASK, 0 );
	REG_READ( I2C_CLR_INTR );

	/* abort (and wait) */
	REG_WRITE( I2C_ENABLE, (enable | (0x1 << 1)) );
	while ( (REG_READ( I2C_TX_ABRT_SOURCE ) & (0x1 << 16)) == 0 );

	/* disable */
	REG_WRITE( I2C_ENABLE, 0 );

	/* reset regs */
	REG_WRITE( I2C_CON, 0x65 );
	REG_WRITE( I2C_TAR, 0x55 );
	REG_WRITE( I2C_SS_SCL_HCNT, 0x30 );
	REG_WRITE( I2C_SS_SCL_LCNT, 0x38 );
	REG_WRITE( I2C_FS_SCL_HCNT, 0 );
	REG_WRITE( I2C_FS_SCL_LCNT, 0 );
	REG_WRITE( I2C_RX_TL, 0 );
	REG_WRITE( I2C_TX_TL, 0 );
	REG_WRITE( I2C_SDA_HOLD, 0x1 );
	REG_WRITE( I2C_FS_SPKLEN, 0x1 );

	/* gate the I2C clock */
	REG_WRITE( I2C_CLKEN, 0 );

	_setvecti( dev->vector_err, NULL);
	_setvecti( dev->vector_rx_avail, NULL);
	_setvecti( dev->vector_tx_req, NULL);
	_setvecti( dev->vector_stop_det, NULL);

	dev->rx_cb = NULL;
	dev->tx_cb = NULL;
	dev->err_cb = NULL;

	i2c_handles[dev_id] = 0;
}

/**
 * \brief	read data through DFSS IIC
 * \param[in]	dev_id	the id of device
 * \param[out]	data	data that need to read (data must be uint8_t type)
 * \param[in]	size	data count need to read
 * \retval	>0	Byte count that was successfully received for poll method,
 * 			it might can't send that much due to \ref \ref dev_iic_info::err_state "different error state".
 * \retval	E_PAR	Parameter is not valid
 */
static int32_t dfss_i2c_master_read(DEV_IIC *iic_obj, uint32_t dev_id, uint8_t * data, uint32_t * size)
{
	volatile int32_t ercd = E_OK;
	DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);

	uint32_t free,cnt_res,i;
	i2c_info_pt dev = i2c_handles[dev_id];
	uint32_t cnt_read = 0;
	uint32_t cnt = *size;
	cnt_res = cnt;

	while (cnt_res) {
		free = dev->fifo_depth - REG_READ(I2C_TXFLR);
		if ( cnt > free) cnt = free;
		cnt_res -= cnt;
		if (!cnt_res) {
			for ( i = 0;i < cnt-1;i++) {
				REG_WRITE(I2C_DATA_CMD, I2C_READ_CMD);
			}
			if (iic_info_ptr->next_cond == IIC_MODE_STOP) {
				REG_WRITE(I2C_DATA_CMD, I2C_READ_CMD | I2C_STOP_CMD);
			} else {
				REG_WRITE(I2C_DATA_CMD, I2C_READ_CMD);
			}
		} else {
			for ( i = 0;i < cnt;i++) {
				REG_WRITE(I2C_DATA_CMD, I2C_READ_CMD);
			}
		}
		i = 0;
		cnt_read = 0;
		while (cnt_read < cnt) {
			if (i++ > DFSS_IIC_MAX_RETRY_COUNT) return (ercd = -1);
			if (dfss_iic_master_getready(dev_id)) {
				data[cnt_read] = REG_READ(I2C_DATA_CMD);
				cnt_read++;
				i= 0;
			}
		}
		data += cnt;
		ercd += cnt;
		cnt = cnt_res;
	}
error_exit:
	return ercd;
}

/**
 * \brief	poll transmit data through DFSS IIC as master
 * \param[in]	dev_id	the id of iic device
 * \param[in]	data	data that need to send (data must be uint8_t type)
 * \param[in]	*size	data length need to send
 * \retval	>0	Byte count that was successfully sent for poll method,
 * 			it might can't send that much due to \ref \ref dev_iic_info::err_state "different error state".
 * \retval	E_PAR	Parameter is not valid
 */
static int32_t dfss_i2c_master_write(DEV_IIC *iic_obj, uint32_t dev_id, uint8_t * data, uint32_t * size)
{
	volatile int32_t ercd = E_OK;
	DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);

	uint32_t free,cnt_res,i;
	uint32_t cnt = *size;
	i2c_info_pt	  dev = i2c_handles[dev_id];
	cnt_res = cnt;
	while (cnt_res) {
		free = dev->fifo_depth - REG_READ(I2C_TXFLR);
		if (cnt > free) cnt = free;
		cnt_res -= cnt;
		if (!cnt_res) {
			for (i = 0;i < cnt-1;i++) {
				REG_WRITE(I2C_DATA_CMD, data[i]);
			}
			if (!(iic_info_ptr->next_cond)) {
				REG_WRITE(I2C_DATA_CMD, data[i] | I2C_STOP_CMD);
			} else {
				REG_WRITE(I2C_DATA_CMD, data[i]);
			}
		} else {
			for (i = 0;i < cnt;i++) {
				REG_WRITE(I2C_DATA_CMD, data[i]);
			}
		}
		data += cnt;
		ercd += cnt;
		cnt = cnt_res;
	}

error_exit:
	return ercd;
}

/** @} end of group DEVICE_DFSS_IIC_STATIC */

/**
 * \defgroup	DEVICE_DW_IIC_IMPLEMENT	DFSS IIC Driver Function API Implement
 * \ingroup		DEVICE_DFSS_IIC
 * \brief	implement device hal iic api with DFSS IIC
 * @{
 */

/**
 * \brief	open a DFSS iic device
 * \param[in]	iic_obj	iic device object pointer
 * \param[in]	mode	iic working mode (master or slave)
 * \param[in]	param	When mode is \ref DEV_MASTER_MODE, param stands for \ref dev_iic_info::speed_mode "speed mode",
 *			when mode is \ref DEV_SLAVE_MODE, param stands for \ref dev_iic_info::slv_addr "slave device 7bit address"
 * \retval	E_OK	Open successfully without any issues
 * \retval	E_OPNED	If device was opened before with different parameters,
 *			then just increase the \ref dev_iic_info::opn_cnt "opn_cnt" and return \ref E_OPNED
 * \retval	E_OBJ	Device object is not valid
 * \retval	E_SYS	Device is opened for different mode before, if you want to open it with different mode, you need to fully close it first.
 * \retval	E_PAR	Parameter is not valid
 * \retval	E_NOSPT	Open settings are not supported
 */
int32_t dfss_iic_open (DEV_IIC *iic_obj, uint32_t mode, uint32_t param)
{
	int32_t ercd = E_OK;
	uint32_t val32 = 0;
	DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
	DFSS_IIC_CTRL *iic_ctrl_ptr = (DFSS_IIC_CTRL *)(iic_info_ptr->iic_ctrl);
	/* START ERROR CHECK */
	VALID_CHK_IIC_INFO_OBJECT(iic_info_ptr);
	DFSS_IIC_CHECK_EXP((mode==DEV_MASTER_MODE)||(mode==DEV_SLAVE_MODE), E_PAR);
	/* END OF ERROR CHECK */
	if (mode == DEV_MASTER_MODE) {
		DFSS_IIC_CHECK_EXP((param>=IIC_SPEED_STANDARD) && (param<=IIC_SPEED_ULTRA), E_PAR);
		if (iic_info_ptr->opn_cnt == 0) {
			DFSS_IIC_CHECK_EXP(dfss_i2c_master_open(iic_ctrl_ptr->dev_id) == 0, E_SYS);
			val32 = (uint32_t)param;
			dfss_iic_set_speedmode(iic_ctrl_ptr->dev_id,val32);
			iic_info_ptr->opn_cnt++;
			iic_info_ptr->addr_mode = IIC_7BIT_ADDRESS;
			iic_info_ptr->mode = mode;
			iic_info_ptr->tar_addr &= IIC_7BIT_ADDRESS_MASK;
			iic_info_ptr->status = DEV_ENABLED;
			iic_info_ptr->cur_state = IIC_FREE;
			iic_info_ptr->err_state = IIC_ERR_NONE;
			iic_info_ptr->next_cond = IIC_MODE_STOP;
			iic_info_ptr->extra = NULL;

			iic_ctrl_ptr->iic_tx_over = 0;
			iic_ctrl_ptr->iic_rx_over = 0;
		} else {
			ercd = E_OPNED;
		}
	} else {
		/* slave open */
		ercd = E_NOSPT;
		goto error_exit;
	}
	return E_OK;

error_exit:
	return ercd;
}


/**
 * \brief	Close a DFSS iic device
 * \param[in]		iic_obj	iic device object pointer
 * \retval	E_OK	Close successfully without any issues(including secenary that device is already closed)
 * \retval	E_OPNED	Device is still opened, the device \ref dev_iic_info::opn_cnt "opn_cnt" decreased by 1
 * \retval	E_OBJ	Device object is not valid
 */
int32_t dfss_iic_close (DEV_IIC *iic_obj)
{
	int32_t ercd = E_OK;
	DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
	DFSS_IIC_CTRL *iic_ctrl_ptr = (DFSS_IIC_CTRL *)(iic_info_ptr->iic_ctrl);

	/* START ERROR CHECK */
	VALID_CHK_IIC_INFO_OBJECT(iic_info_ptr);
	DFSS_IIC_CHECK_EXP(iic_info_ptr->opn_cnt > 0, E_OK);
	/* END OF ERROR CHECK */
		/* END OF ERROR CHECK */
	if (iic_info_ptr->mode == DEV_MASTER_MODE) {
		if (iic_info_ptr->opn_cnt > 0) {
			iic_info_ptr->opn_cnt = 0;
			dfss_i2c_master_close(iic_ctrl_ptr->dev_id);
			iic_info_ptr->status = DEV_DISABLED;
			iic_info_ptr->next_cond = IIC_MODE_STOP;
			iic_info_ptr->extra = NULL;
		} else {
			ercd = E_CLSED;
		}
		return ercd;
	} else {
		/* slave close */
		ercd = E_NOSPT;
		goto error_exit;
	}
error_exit:
	return ercd;
}

/**
 * \brief	write data through DFSS IIC
 * \param[in]	iic_obj	iic device object pointer
 * \param[out]	data	data that need to write (data must be uint8_t type)
 * \param[in]	len	data count need to write
 * \retval	>0	Byte count that was successfully wirt for poll method,
 * 			it might can't send that much due to \ref \ref dev_iic_info::err_state "different error state".
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_CTX	Device is still in transfer state
 * \retval	E_PAR	Parameter is not valid
 * \retval	E_SYS	Can't receive data from hardware due to hardware issues, such as device is disabled
 */
int32_t dfss_iic_write(DEV_IIC *iic_obj, const void *data, uint32_t len)
{
	volatile int32_t ercd = E_OK;
	uint32_t i = 0;
	DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
	DFSS_IIC_CTRL *iic_ctrl_ptr = (DFSS_IIC_CTRL *)(iic_info_ptr->iic_ctrl);

	/* START ERROR CHECK */
	VALID_CHK_IIC_INFO_OBJECT(iic_info_ptr);
	DFSS_IIC_CHECK_EXP(iic_info_ptr->opn_cnt > 0, E_CLSED);
	DFSS_IIC_CHECK_EXP(iic_info_ptr->status & DEV_ENABLED, E_SYS);
	DFSS_IIC_CHECK_EXP(data!=NULL, E_PAR);
	DFSS_IIC_CHECK_EXP(len>0, E_PAR);
	/* END OF ERROR CHECK */

	while (dfss_iic_master_putready(iic_ctrl_ptr->dev_id) == 0) {
		if (i++ > DFSS_IIC_MAX_RETRY_COUNT) return -1;
	}
	if (iic_info_ptr->mode == DEV_MASTER_MODE) { /* Master mode write data */
		ercd = dfss_i2c_master_write(iic_obj, iic_ctrl_ptr->dev_id, (uint8_t *)data, &len);
	} else {
		/* slave write */
		ercd = E_NOSPT;
		goto error_exit;
	}
	/* Issue a read request */
	while (dfss_iic_master_putover(iic_ctrl_ptr->dev_id) == 0) {
		if (i++ > DFSS_IIC_MAX_RETRY_COUNT) return -1;
	}
error_exit:
	return ercd;
}

/**
 * \brief	read data through DFSS IIC
 * \param[in]	iic_obj	iic device object pointer
 * \param[out]	data	data that need to read (data must be uint8_t type)
 * \param[in]	len	data count need to read
 * \retval	>0	Byte count that was successfully received for poll method,
 * 			it might can't receive that much due to \ref \ref dev_iic_info::err_state "different error state".
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_CTX	Device is still in transfer state
 * \retval	E_PAR	Parameter is not valid
 * \retval	E_SYS	Can't receive data from hardware due to hardware issues, such as device is disabled
 */
int32_t dfss_iic_read(DEV_IIC *iic_obj, void *data, uint32_t len)
{
	volatile int32_t ercd;
	uint32_t i = 0;

	DEV_IIC_INFO *iic_info_ptr = &(iic_obj->iic_info);
	DFSS_IIC_CTRL *iic_ctrl_ptr = (DFSS_IIC_CTRL *)(iic_info_ptr->iic_ctrl);

	/* START ERROR CHECK */
	VALID_CHK_IIC_INFO_OBJECT(iic_info_ptr);
	DFSS_IIC_CHECK_EXP(iic_info_ptr->opn_cnt > 0, E_CLSED);
	DFSS_IIC_CHECK_EXP(iic_info_ptr->status & DEV_ENABLED, E_SYS);
	DFSS_IIC_CHECK_EXP(data!=NULL, E_PAR);
	DFSS_IIC_CHECK_EXP((len)>0, E_PAR);
	/* END OF ERROR CHECK */

	/* Issue a read request */
	while (dfss_iic_master_putready(iic_ctrl_ptr->dev_id) == 0) {
		if (i++ > DFSS_IIC_MAX_RETRY_COUNT) return -1;
	}

	if (iic_info_ptr->mode == DEV_MASTER_MODE) { /* Master mode receive data */
		ercd = dfss_i2c_master_read(iic_obj, iic_ctrl_ptr->dev_id, (uint8_t *)data, &len);
	} else {
		/* slave read */
		ercd = E_NOSPT;
		goto error_exit;
	}
error_exit:
	return ercd;
}

void dfss_iic_isr(DEV_IIC *iic_obj, void *ptr)
{
	/*  interrupt process */
}

/** @} end of group DEVICE_DW_IIC_IMPLEMENT */

#endif /*I2C_DEV_PRESENT */