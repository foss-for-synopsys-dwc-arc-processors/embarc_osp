/* ==========================================================================
 * Synopsys DesignWare Sensor and Control IP Subsystem IO Software Driver and
 * documentation (hereinafter, "Software") is an Unsupported proprietary work
 * of Synopsys, Inc. unless otherwise expressly agreed to in writing between
 * Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================== */
/*==========================================================================
 * Library DW_DFSS-2.1.13
 * ========================================================================== */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "i3c_master.h"
#include "i3c_priv.h"
#include "io_config.h"
#include "apexextensions.h"
#include <stdio.h>
#include <string.h>

/****************************************************************************
*
* Configuration Constants
*
****************************************************************************/
#ifdef IO_I3C0_PRESENT
#define I3C_DEV_PRESENT
#endif
#ifdef IO_I3C1_PRESENT
#define I3C_DEV_PRESENT
#endif
#ifdef IO_I3C2_PRESENT
#define I3C_DEV_PRESENT
#endif
#ifdef IO_I3C3_PRESENT
#define I3C_DEV_PRESENT
#endif

#ifdef I3C_DEV_PRESENT

#define I3C_DMA_TX 0            // global enable flag for DMA TX support
#define I3C_DMA_RX 1            // global enable flag for DMA RX support

#define I3C_MAX_CNT 4           // maximum number of I3C devices for data structure allocation (1 .. 4)

#define I3C_DEBUG_PRINT 0       // global enable flag for debug output. Use with caution, might affect test results!

/****************************************************************************
*
* Macros
*
****************************************************************************/

// For all macros, the 'reg' value should be one of the
// I3C_* constants defined in the i3c_priv.h header

// Write full width value 'x' to register 'reg'
#define     I3C_REG_WRITE(reg, x)   _sr((unsigned)(x), (unsigned)(dev->reg_base + reg))

// Read full width value from register 'reg'
#define     I3C_REG_READ(reg)     ((unsigned)  _lr((unsigned)(dev->reg_base + reg)))

// Write a bit range to register 'reg'
// 'x' is the full width initial value,
// 'y' is the new desired value of the field
// 'pos' is the field offset in the register
// 'len' is the field length in the register
#define     I3C_REG_WRITE_BITS(reg, x, y, len, pos)   I3C_REG_WRITE(reg, ((((x)          & ~((~(0xffffffff << len)) << pos)) \
									   | (((y) << pos)  &  ((~(0xffffffff << len)) << pos)))))

// Set field value in a register variable (without actually writing it anywhere)
// 'field' is the field name defined in the i3c_priv.h header with
//         <reg_name>_POS_<field name> constants
// 'x' is the initial full-width value
// 'y' is the new field value
#define     I3C_REG_SET_FIELD(reg, field, x, y)    (((x)                         & ~(reg##_MSK_##field)) \
						    | (((y) << (reg##_POS_##field)) &  (reg##_MSK_##field)))

// Set field value in a register (actually writing it to the register)
// 'field' is the field name defined in the i3c_priv.h header with
//         <reg_name>_POS_<field name> constants
// 'x' is the initial full-width value
// 'y' is the new field value
#define     I3C_REG_WRITE_FIELD(reg, field, x, y) I3C_REG_WRITE(reg, (((x)                         & ~(reg##_MSK_##field)) \
								      | (((y) << (reg##_POS_##field)) &  (reg##_MSK_##field))))

// Extracts particular field from a variable holding a register value
// 'field' is the field name defined in the i3c_priv.h header with
//         <reg_name>_POS_<field name> constants
// 'var' is the register variable value
#define     I3C_REG_GET_FIELD(reg, field, var)    (((var) & (reg##_MSK_##field)) >> (reg##_POS_##field))

// Same as I3C_REG_GET_FIELD, but reads the value from the register
// rather than using a variable
#define     I3C_REG_READ_FIELD(reg, field)        ((I3C_REG_READ(reg) & (reg##_MSK_##field)) >> (reg##_POS_##field))

// enable/disable debug printout. Use with caution.
// (void)sizeof is just a way to get rid of a function declaration at compile time without warnings

#if I3C_DEBUG_PRINT
#define I3C_PRINT printf
#else
#define I3C_PRINT(...) (void)sizeof(__VA_ARGS__)
#endif

/****************************************************************************
*
* Private Functions (Declaration)
*
****************************************************************************/

static void i3c_mst_device_ISR_proc(uint32_t dev_id);
static void i3c_mst_rx_complete_ISR_proc(uint32_t dev_id);
static void i3c_mst_tx_req_ISR_proc(uint32_t dev_id);
static void i3c_mst_resp_ready_ISR_proc(uint32_t dev_id);

#ifdef __Xdmac
static void i3c_mst_dma_err_ISR_proc(uint32_t dev_id);
static void i3c_mst_dma_rx_complete_ISR_proc(i3c_info_pt dev);
static void i3c_mst_dma_tx_complete_ISR_proc(i3c_info_pt dev);
static void i3c_start_dma_rx(i3c_info_pt dev, uint32_t burst);
static void i3c_start_dma_tx(i3c_info_pt dev, uint32_t burst);
static void i3c_update_dma_rx_status(i3c_info_pt dev);
static void i3c_update_dma_tx_status(i3c_info_pt dev);
#endif

static void i3c_fill_txfifo(uint32_t dev_id);
static void i3c_retrieve_rxfifo(uint32_t dev_id);

static void i3c_start_tx(uint32_t dev_id);
static void i3c_start_rx(uint32_t dev_id);

static void i3c_reset_and_init(uint32_t dev_id);
static void i3c_abort(uint32_t dev_id);

#if I3C_DEBUG_PRINT
static void print_all_registers(uint32_t dev_id);
static void print_status_registers(uint32_t dev_id);
#endif

#ifdef __Xdmac

#define DECLARE_I3C_ISR(dev_id)						\
	static _Interrupt void i3c_mst##dev_id##_device_ISR();		\
	static _Interrupt void i3c_mst##dev_id##_dma_err_ISR();		\
	static _Interrupt void i3c_mst##dev_id##_dma_rx_complete_ISR();	\
	static _Interrupt void i3c_mst##dev_id##_dma_tx_complete_ISR();

#else

#define DECLARE_I3C_ISR(dev_id)	\
	static _Interrupt void i3c_mst##dev_id##_device_ISR();

#endif

#ifdef IO_I3C0_PRESENT
DECLARE_I3C_ISR(0)
#endif
#ifdef IO_I3C1_PRESENT
DECLARE_I3C_ISR(1)
#endif
#ifdef IO_I3C2_PRESENT
DECLARE_I3C_ISR(2)
#endif
#ifdef IO_I3C3_PRESENT
DECLARE_I3C_ISR(3)
#endif
/****************************************************************************
*
* Private Data Structures
*
****************************************************************************/
#ifdef __Xdmac
#define FILL_I3C_INFO(dev_id)					      \
	.instID = dev_id,					      \
	.reg_base = IO_I3C##dev_id##_BASE_ADDR,			      \
	.vector = IO_I3C##dev_id##_INTR,			      \
	.fifo_depth = IO_I3C##dev_id##_DATA_BUFFER_DEPTH,	      \
	.isr = i3c_mst##dev_id##_device_ISR,			      \
	.isr_dma_err = i3c_mst##dev_id##_dma_err_ISR,		      \
	.isr_dma_rx_complete = i3c_mst##dev_id##_dma_rx_complete_ISR, \
	.isr_dma_tx_complete = i3c_mst##dev_id##_dma_tx_complete_ISR
#else
#define FILL_I3C_INFO(dev_id)				  \
	.instID = dev_id,				  \
	.reg_base = IO_I3C##dev_id##_BASE_ADDR,		  \
	.vector = IO_I3C##dev_id##_INTR,		  \
	.fifo_depth = IO_I3C##dev_id##_DATA_BUFFER_DEPTH, \
	.isr = i3c_mst##dev_id##_device_ISR
#endif
static i3c_info_pt i3c_handles[I3C_MAX_CNT] = { 0 };

static i3c_info_t i3c_master_devs[] = {
#ifdef IO_I3C0_PRESENT
	{
		FILL_I3C_INFO(0),

#if (defined(IO_I3C0_DMA_RX) && I3C_DMA_RX)
		.dmarxchanid = IO_I3C0_DMA_RX,
#else
		.dmarxchanid = DMA_NONE,
#endif
#if (defined(IO_I3C0_DMA_TX) && I3C_DMA_TX)
		.dmatxchanid = IO_I3C0_DMA_TX,
#else
		.dmatxchanid = DMA_NONE,
#endif

	},
#endif

#ifdef IO_I3C1_PRESENT
	{
		FILL_I3C_INFO(1),

#if (defined(IO_I3C1_DMA_RX) && I3C_DMA_RX)
		.dmarxchanid = IO_I3C1_DMA_RX,
#else
		.dmarxchanid = DMA_NONE,
#endif
#if (defined(IO_I3C1_DMA_TX) && I3C_DMA_TX)
		.dmatxchanid = IO_I3C1_DMA_TX,
#else
		.dmatxchanid = DMA_NONE,
#endif

	},
#endif

#ifdef IO_I3C2_PRESENT
	{
		FILL_I3C_INFO(2),

#if (defined(IO_I3C2_DMA_RX) && I3C_DMA_RX)
		.dmarxchanid = IO_I3C2_DMA_RX,
#else
		.dmarxchanid = DMA_NONE,
#endif
#if (defined(IO_I3C2_DMA_TX) && I3C_DMA_TX)
		.dmatxchanid = IO_I3C2_DMA_TX,
#else
		.dmatxchanid = DMA_NONE,
#endif
	},
#endif

#ifdef IO_I3C3_PRESENT
	{
		FILL_I3C_INFO(3),

#if (defined(IO_I3C3_DMA_RX) && I3C_DMA_RX)
		.dmarxchanid = IO_I3C3_DMA_RX,
#else
		.dmarxchanid = DMA_NONE,
#endif
#if (defined(IO_I3C3_DMA_TX) && I3C_DMA_TX)
		.dmatxchanid = IO_I3C3_DMA_TX,
#else
		.dmatxchanid = DMA_NONE,
#endif
	},
#endif

	{
		.instID = I3C_MAX_CNT
	}
};

#ifdef __Xdmac

static const int I3C_DEV_CNT =
	sizeof(i3c_master_devs) / sizeof(i3c_master_devs[0]);

static _Uncached uint32_t i3c_dmatx_descriptor[4 * I3C_DEV_CNT];
static _Uncached uint32_t i3c_dmarx_descriptor[4 * I3C_DEV_CNT];

#endif

#if I3C_DEBUG_PRINT
// Register lists used for printing out the register state
static const uint32_t reg_offsets_short[] = {
	I3C_DEVICE_CTRL,
	I3C_QUEUE_THLD_CTRL,
	I3C_DATA_BUFFER_THLD_CTRL,
	I3C_INTR_STATUS,
	I3C_QUEUE_STATUS_LEVEL,
	I3C_DATA_BUFFER_STATUS_LEVEL,
	I3C_PRESENT_STATE,
	I3C_CLKEN
};

static const uint32_t reg_count_short =
	sizeof(reg_offsets_short) / sizeof(reg_offsets_short[0]);

static const char *reg_names_short[reg_count_short] = {
	"I3C_DEVICE_CTRL",
	"I3C_QUEUE_THLD_CTRL",
	"I3C_DATA_BUFFER_THLD_CTRL",
	"I3C_INTR_STATUS",
	"I3C_QUEUE_STATUS_LEVEL",
	"I3C_DATA_BUFFER_STATUS_LEVEL",
	"I3C_PRESENT_STATE",
	"I3C_CLKEN"
};

static const uint32_t reg_offsets[] = {
	I3C_DEVICE_CTRL,
	I3C_DEVICE_ADDR,
	I3C_HW_CAPABILITY,
	I3C_IBI_QUEUE_STATUS,
	I3C_QUEUE_THLD_CTRL,
	I3C_DATA_BUFFER_THLD_CTRL,
	I3C_IBI_QUEUE_CTRL,
	I3C_IBI_MR_P2P_REQ_REJECT,
	I3C_IBI_SIR_REQ_REJECT,
	I3C_RESET_CTRL,
	I3C_INTR_STATUS,
	I3C_INTR_STATUS_EN,
	I3C_INTR_SIGNAL_EN,
	I3C_QUEUE_STATUS_LEVEL,
	I3C_DATA_BUFFER_STATUS_LEVEL,
	I3C_PRESENT_STATE,
	I3C_DEVICE_ADDR_TABLE_POINTER,
	I3C_DEV_CHAR_TABLE_POINTER,
	I3C_VENDOR_SPECIFIC_REG_POINTER,
	I3C_DEVICE_CTRL_EXTENDED,
	I3C_SCL_HCNT_TIMING,
	I3C_SCL_LCNT_TIMING,
	I3C_SCL_EXT_LCNT_TIMING,
	I3C_BUS_FREE_TIMING,
	I3C_BUS_IDLE_TIMING,
	I3C_VER_ID,
	I3C_VER_TYPE,
	I3C_EXTENDED_CAPABILITY,
	I3C_DEV_CHAR_TABLE1_LOC1,
	I3C_DEV_CHAR_TABLE1_LOC2,
	I3C_DEV_CHAR_TABLE1_LOC3,
	I3C_DEV_CHAR_TABLE1_LOC4,
	I3C_DEV_CHAR_TABLE2_LOC1,
	I3C_DEV_CHAR_TABLE2_LOC2,
	I3C_DEV_CHAR_TABLE2_LOC3,
	I3C_DEV_CHAR_TABLE2_LOC4,
	I3C_DEV_ADDR_TABLE_LOC1,
	I3C_DEV_ADDR_TABLE_LOC2,
	I3C_CLKEN
};

static const uint32_t reg_count =
	sizeof(reg_offsets) / sizeof(reg_offsets[0]);
static const char *reg_names[reg_count] = {
	"I3C_DEVICE_CTRL",
	"I3C_DEVICE_ADDR",
	"I3C_HW_CAPABILITY",
	"I3C_IBI_QUEUE_STATUS",
	"I3C_QUEUE_THLD_CTRL",
	"I3C_DATA_BUFFER_THLD_CTRL",
	"I3C_IBI_QUEUE_CTRL",
	"I3C_IBI_MR_P2P_REQ_REJECT",
	"I3C_IBI_SIR_REQ_REJECT",
	"I3C_RESET_CTRL",
	"I3C_INTR_STATUS",
	"I3C_INTR_STATUS_EN",
	"I3C_INTR_SIGNAL_EN",
	"I3C_QUEUE_STATUS_LEVEL",
	"I3C_DATA_BUFFER_STATUS_LEVEL",
	"I3C_PRESENT_STATE",
	"I3C_DEVICE_ADDR_TABLE_POINTER",
	"I3C_DEV_CHAR_TABLE_POINTER",
	"I3C_VENDOR_SPECIFIC_REG_POINTER",
	"I3C_DEVICE_CTRL_EXTENDED",
	"I3C_SCL_HCNT_TIMING",
	"I3C_SCL_LCNT_TIMING",
	"I3C_SCL_EXT_LCNT_TIMING",
	"I3C_BUS_FREE_TIMING",
	"I3C_BUS_IDLE_TIMING",
	"I3C_VER_ID",
	"I3C_VER_TYPE",
	"I3C_EXTENDED_CAPABILITY",
	"I3C_DEV_CHAR_TABLE1_LOC1",
	"I3C_DEV_CHAR_TABLE1_LOC2",
	"I3C_DEV_CHAR_TABLE1_LOC3",
	"I3C_DEV_CHAR_TABLE1_LOC4",
	"I3C_DEV_CHAR_TABLE2_LOC1",
	"I3C_DEV_CHAR_TABLE2_LOC2",
	"I3C_DEV_CHAR_TABLE2_LOC3",
	"I3C_DEV_CHAR_TABLE2_LOC4",
	"I3C_DEV_ADDR_TABLE_LOC1",
	"I3C_DEV_ADDR_TABLE_LOC2",
	"I3C_CLKEN"
};
#endif

/****************************************************************************
*
* Private Functions (Definition)
*
****************************************************************************/

#define DEFINE_I3C_ISR_ID(dev_id, isr)			       \
	static _Interrupt void i3c_mst##dev_id##_##isr##_ISR() \
	{						       \
		i3c_mst_##isr##_ISR_proc(dev_id);	       \
	}

#define DEFINE_I3C_ISR_PTR(dev_id, isr)			       \
	static _Interrupt void i3c_mst##dev_id##_##isr##_ISR() \
	{						       \
		i3c_mst_##isr##_ISR_proc(i3c_handles[dev_id]); \
	}

#ifdef __Xdmac

#define DEFINE_I3C_ALL_ISR(dev_id)		    \
	DEFINE_I3C_ISR_ID(dev_id, device)	    \
	DEFINE_I3C_ISR_ID(dev_id, dma_err)	    \
	DEFINE_I3C_ISR_PTR(dev_id, dma_rx_complete) \
	DEFINE_I3C_ISR_PTR(dev_id, dma_tx_complete)

#else

#define DEFINE_I3C_ALL_ISR(dev_id) \
	DEFINE_I3C_ISR_ID(dev_id, device)

#endif

#ifdef IO_I3C0_PRESENT
DEFINE_I3C_ALL_ISR(0)
#endif
#ifdef IO_I3C1_PRESENT
DEFINE_I3C_ALL_ISR(1)
#endif
#ifdef IO_I3C2_PRESENT
DEFINE_I3C_ALL_ISR(2)
#endif
#ifdef IO_I3C3_PRESENT
DEFINE_I3C_ALL_ISR(3)
#endif
static void i3c_mst_device_ISR_proc(uint32_t dev_id)
{
	i3c_info_pt dev = i3c_handles[dev_id];
	uint32_t status = I3C_REG_READ(I3C_INTR_STATUS);

	// ignore the interrupts we didn't ask for
	status &= I3C_REG_READ(I3C_INTR_SIGNAL_EN);

	// keep RX/TX threshold interrupts handled first!

	if (status & I3C_INTR_STATUS_MSK_RX_THLD_STAT) { // number of entries in receive buffer is >= RX_BUF_THLD
		i3c_mst_rx_complete_ISR_proc(dev_id);
	}

	if (status & I3C_INTR_STATUS_MSK_TX_THLD_STAT) { // number of empty locations in transmit buffer >= TX_EMPTY_BUF_THLD
		i3c_mst_tx_req_ISR_proc(dev_id);
	}

	if (status & I3C_INTR_STATUS_MSK_DEFSLV_P2PTRGT_STAT) { // A DEFSLV or SETP2PTGT CCC is received

		// TODO: ISR handler
		I3C_REG_WRITE(I3C_INTR_STATUS, I3C_INTR_STATUS_MSK_DEFSLV_P2PTRGT_STAT); // Clear interrupt status
	}

	if (status & I3C_INTR_STATUS_MSK_TRANSFER_ERR_STAT) { // An error has occurred during transfer

		// resume from halt
		uint32_t reg_val = I3C_REG_READ(I3C_DEVICE_CTRL);
		I3C_REG_WRITE_FIELD(I3C_DEVICE_CTRL, RESUME, reg_val, 1);

		// TODO: ISR handler
		I3C_REG_WRITE(I3C_INTR_STATUS, I3C_INTR_STATUS_MSK_TRANSFER_ERR_STAT); // Clear interrupt status
	}

	if (status & I3C_INTR_STATUS_MSK_DYN_ADDR_ASSGN_STAT) { // Dynamic Address Assigned Status

		// TODO: ISR handler
		I3C_REG_WRITE(I3C_INTR_STATUS, I3C_INTR_STATUS_MSK_DYN_ADDR_ASSGN_STAT); // Clear interrupt status
	}

	if (status & I3C_INTR_STATUS_MSK_CMD_RETRY_FAILED_STAT) { // Transfer Retry Failed Status

		if (NULL != dev->err_cb) {
			dev->err_cb(dev_id);
		}
		// TODO: ISR handler
		I3C_REG_WRITE(I3C_INTR_STATUS, I3C_INTR_STATUS_MSK_CMD_RETRY_FAILED_STAT); // Clear interrupt status
	}

	if (status & I3C_INTR_STATUS_MSK_CCC_TABLE_UPDT_STAT) { // CCC Table Updated Status

		// TODO: ISR handler
		I3C_REG_WRITE(I3C_INTR_STATUS, I3C_INTR_STATUS_MSK_CCC_TABLE_UPDT_STAT); // Clear interrupt status
	}

	if (status & I3C_INTR_STATUS_MSK_TRANSFER_ABORT_STAT) { // A Transfer is aborted

		if (NULL != dev->err_cb) {
			dev->err_cb(dev_id);
		}
		// TODO: ISR handler
		I3C_REG_WRITE(I3C_INTR_STATUS, I3C_INTR_STATUS_MSK_TRANSFER_ABORT_STAT); // Clear interrupt status
	}

	if (status & I3C_INTR_STATUS_MSK_RESP_READY_STAT_INTR) { // number of entries in response queue is >= RESP_BUF_THLD
		i3c_mst_resp_ready_ISR_proc(dev_id);
	}

	if (status & I3C_INTR_STATUS_MSK_CMD_QUEUE_READY_STAT) { // number of empty locations in command queue is >= CMD_EMPTY_BUF_THLD

		// TODO: ISR handler
	}
	if (status & I3C_INTR_STATUS_MSK_IBI_THLD_STAT) { // number of entries in IBI buffer is >= IBI_BUF_THLD

		// TODO: ISR handler
	}

}

#ifdef __Xdmac

static void i3c_mst_dma_err_ISR_proc(uint32_t dev_id)
{
	i3c_info_pt dev = i3c_handles[dev_id];

	I3C_PRINT("I3C DMA Error\n");

	if (dev->err_cb != NULL) {
		dev->err_cb(dev_id);
	}
}

static void i3c_mst_dma_rx_complete_ISR_proc(i3c_info_pt dev)
{
	// DMA RX transfer done
	_dma_chan_enable((0x1 << dev->dmarxchanid), 0);
	_dma_int_clear((0x1 << dev->dmarxchanid));

	if (dev->handling_rx == 1) {
		i3c_update_dma_rx_status(dev);
		i3c_start_dma_rx(dev, 0);
	}
}

static void i3c_mst_dma_tx_complete_ISR_proc(i3c_info_pt dev)
{
	// DMA TX transfer done
	_dma_chan_enable((0x1 << dev->dmatxchanid), 0);
	_dma_int_clear((0x1 << dev->dmatxchanid));

	if (dev->handling_tx == 1) {
		i3c_update_dma_tx_status(dev);
		i3c_start_dma_tx(dev, 0);
		if ((dev->tx_dma_count == 0) && (dev->tx_count != dev->tx_size)) {
			// after DMA part is done
			// unmask TX_THLD interrupt if there is data left to be sent
			uint32_t val = I3C_REG_READ(I3C_INTR_SIGNAL_EN);
			val =
				I3C_REG_SET_FIELD(I3C_INTR_SIGNAL_EN, TX_THLD_SIGNAL_EN,
						  val, 1);
			I3C_REG_WRITE(I3C_INTR_SIGNAL_EN, val);
		}
	}
}

static void i3c_update_dma_rx_status(i3c_info_pt dev)
{
	if (dev->rx_dma_count != 0) {
		uint32_t dma_left, dma_ctrl;

		// Get the amount of data transferred by DMA
		if (dev->dmarxchanid < DMAC_MEM_CHAN_CNT) {
			_dma_chan_desc_get_ctrl(dev->dmarxchanid, &dma_ctrl);
		} else {
			_dma_chan_desc_aux_get_ctrl(dev->dmarxchanid, &dma_ctrl);
		}

		if ((dma_ctrl & DMACTRLx_OP_MASK) == 0) { // transfer fully executed
			dma_left = 0;
			dev->rx_count = dev->rx_dma_count;
		} else {
			dma_left = ((dma_ctrl >> DMACTRLx_SIZE_POS) + 1) & 0x1FFF;
			dev->rx_count = dev->rx_dma_count - dma_left;
		}

		// I3C_PRINT("i3c_update_dma_rx_status: Bytes left after DMA: %d, dma_left = %d, avail = %d\n", dev->rx_size - dev->rx_count, dma_left);
	}

}

static void i3c_update_dma_tx_status(i3c_info_pt dev)
{
	if (dev->tx_dma_count != 0) {
		uint32_t dma_left, dma_ctrl;

		// Get the amount of data transferred by DMA
		if (dev->dmatxchanid < DMAC_MEM_CHAN_CNT) {
			_dma_chan_desc_get_ctrl(dev->dmatxchanid, &dma_ctrl);
		} else {
			_dma_chan_desc_aux_get_ctrl(dev->dmatxchanid, &dma_ctrl);
		}

		if ((dma_ctrl & DMACTRLx_OP_MASK) == 0) { // transfer fully executed
			dma_left = 0;
			dev->tx_count = dev->tx_dma_count;
		} else {
			dma_left = ((dma_ctrl >> DMACTRLx_SIZE_POS) + 1) & 0x1FFF;
			dev->tx_count = dev->tx_dma_count - dma_left;
		}

		// I3C_PRINT("i3c_update_dma_tx_status: Bytes left after DMA: %d, dma_left = %d, avail = %d\n", dev->tx_size - dev->tx_count, dma_left);
	}

}

static void i3c_start_dma_rx(i3c_info_pt dev, uint32_t burst)
{
	uint16_t dma_size = (dev->rx_size - dev->rx_count) & ~0x3;

	if (dma_size == 0) {
		dev->rx_dma_count = 0;
		return;
	}
	// limitation of DMA controller. The rest will be handled on DMA completion
	if (dma_size > 8192) {
		dma_size = 8192;
	}

	dev->rx_dma_count = dev->rx_count + dma_size;

	if (burst == 0) { // use the previous value
		burst =
			(dev->
			 dmarxdescriptor[0] & DMACTRLx_BSIZE_MASK) >>
			DMACTRLx_BSIZE_POS;
	}

	dev->dmarxdescriptor[0] =
		I3C_MST_DMA_RX_CTRL | DMACTRLx_BSIZE(burst) |
		DMACTRLx_SIZE(dma_size - 1);
	dev->dmarxdescriptor[2] =
		(uint32_t) dev->rx_data + dev->rx_count + ((dma_size - 1) & ~0x3);

	// init DMA
	if (dev->dmarxchanid < DMAC_MEM_CHAN_CNT) {
		_dma_chan_desc(dev->dmarxchanid, &(dev->dmarxdescriptor[0]));
	} else {
		_dma_chan_desc_aux(dev->dmarxchanid, &(dev->dmarxdescriptor[0]));
	}

	// I3C_PRINT("i3c_start_dma_rx -> rx_size = %d, rx_count = %d, dma_size = %d, burst = %d, rx_dma_count = %d, end_address = %X\n", dev->rx_size, dev->rx_count, dma_size, burst, dev->rx_dma_count, dev->dmarxdescriptor[2]);

	_dma_chan_enable((0x1 << dev->dmarxchanid), 1);
}

static void i3c_start_dma_tx(i3c_info_pt dev, uint32_t burst)
{
	uint16_t dma_size = (dev->tx_size - dev->tx_count) & ~0x3;

	if (dma_size == 0) {
		dev->tx_dma_count = 0;
		return;
	}
	// limitation of DMA controller. The rest will be handled on DMA completion
	if (dma_size > 8192) {
		dma_size = 8192;
	}

	dev->tx_dma_count = dev->tx_count + dma_size;

	if (burst == 0) { // use the previous value
		burst =
			(dev->
			 dmatxdescriptor[0] & DMACTRLx_BSIZE_MASK) >>
			DMACTRLx_BSIZE_POS;
	}

	dev->dmatxdescriptor[0] =
		I3C_MST_DMA_TX_CTRL | DMACTRLx_BSIZE(burst) |
		DMACTRLx_SIZE(dma_size - 1);
	dev->dmatxdescriptor[2] =
		(uint32_t) dev->tx_data + dev->tx_count + ((dma_size - 1) & ~0x3);

	// init DMA
	if (dev->dmatxchanid < DMAC_MEM_CHAN_CNT) {
		_dma_chan_desc(dev->dmatxchanid, &(dev->dmatxdescriptor[0]));
	} else {
		_dma_chan_desc_aux(dev->dmatxchanid, &(dev->dmatxdescriptor[0]));
	}

	// I3C_PRINT("i3c_start_dma_rx -> rx_size = %d, rx_count = %d, dma_size = %d, burst = %d, rx_dma_count = %d, end_address = %X\n", dev->rx_size, dev->rx_count, dma_size, burst, dev->rx_dma_count, dev->dmarxdescriptor[2]);

	_dma_chan_enable((0x1 << dev->dmatxchanid), 1);
}

#endif

static void i3c_mst_resp_ready_ISR_proc(uint32_t dev_id)
{
	i3c_info_pt dev = i3c_handles[dev_id];
	uint32_t resp, err, tid;

	resp = I3C_REG_READ(I3C_RESPONSE_QUEUE_PORT);

	err = I3C_REG_GET_FIELD(I3C_RESPONSE_QUEUE_PORT, ERR_STATUS, resp);
	tid = I3C_REG_GET_FIELD(I3C_RESPONSE_QUEUE_PORT, TID, resp);

	// I3C_PRINT("Response: TID: 0x%08X ERR: 0x%08X\n", tid, err);
	switch (tid) {
	case I3C_TX_TID:
		if (err == I3C_RESPONSE_QUEUE_PORT_ERR_STATUS_NO_ERROR) {
			I3C_PRINT
				("i3c_mst_resp_ready_ISR_proc - TX transaction successfully completed\n");
			// TX transaction successfully completed
			if (dev->handling_tx == 1) {
				// Mask the TX buffer threshold
				uint32_t val = I3C_REG_READ(I3C_INTR_SIGNAL_EN);
				I3C_REG_WRITE_FIELD(I3C_INTR_SIGNAL_EN, TX_THLD_SIGNAL_EN,
						    val, 0);

				dev->handling_tx = 0;

				dev->tx_data = NULL;
				if (dev->tx_cb != NULL) {
					dev->tx_cb(dev_id);
				}
				// handle pending rx, if any
				if (dev->rx_data != NULL) {
					i3c_start_rx(dev_id);
				}
			}
		} else {
			I3C_PRINT("Response: TX Error: 0x%08X\n", err);
			if (dev->err_cb != NULL) {
				dev->err_cb(dev_id);
			}
		}
		break;

	case I3C_RX_TID:
		if (err == I3C_RESPONSE_QUEUE_PORT_ERR_STATUS_NO_ERROR) {
			// RX transaction successfully completed
			// I3C_PRINT("RX done\n");
			if (dev->handling_rx == 1) {
#ifdef __Xdmac
				// Stop DMA RX channel
				_dma_chan_enable((0x1 << dev->dmarxchanid), 0);

				// Update the number of bytes transferred
				i3c_update_dma_rx_status(dev);

				dev->rx_dma_count = 0;
#endif

				// Mask the RX buffer threshold and RX completion interrupts
				uint32_t val;
				val = I3C_REG_READ(I3C_INTR_SIGNAL_EN);
				val =
					I3C_REG_SET_FIELD(I3C_INTR_SIGNAL_EN,
							  RX_THLD_SIGNAL_EN, val, 0);
				val =
					I3C_REG_SET_FIELD(I3C_INTR_SIGNAL_EN,
							  RESP_READY_SIGNAL_EN, val, 0);
				I3C_REG_WRITE(I3C_INTR_SIGNAL_EN, val);

				// Read remaining data from the FIFO
				i3c_retrieve_rxfifo(dev_id);

				// Report the actual data length read
				*(dev->p_rxsize) =
					I3C_REG_GET_FIELD(I3C_RESPONSE_QUEUE_PORT,
							  DATA_LENGTH_OR_DEV_COUNT, resp);

				I3C_PRINT("read done, read %d bytes\n", *(dev->p_rxsize));
				dev->handling_rx = 0;

				dev->rx_data = NULL;

				if (dev->rx_cb != NULL) {
					dev->rx_cb(dev_id);
				}
				// handle pending TX, if any
				if (dev->tx_data != NULL) {
					i3c_start_tx(dev_id);
				}
			}
		} else {
			I3C_PRINT("Response: RX Error: 0x%08X\n", err);
			if (dev->err_cb != NULL) {
				dev->err_cb(dev_id);
			}
		}
		break;

	default:
		I3C_PRINT("Response: Unknown TID = 0x%04X\n", tid);
		break;
	}
}

static void i3c_mst_rx_complete_ISR_proc(uint32_t dev_id)
{
	i3c_info_pt dev = i3c_handles[dev_id];

	I3C_PRINT("rx_thld\n");
	if (dev->handling_rx == 1) {
		i3c_retrieve_rxfifo(dev_id);
	}
}

static void i3c_mst_tx_req_ISR_proc(uint32_t dev_id)
{
	i3c_info_pt dev = i3c_handles[dev_id];

	if (dev->handling_tx == 1) {
		i3c_fill_txfifo(dev_id);
	}

}

static void i3c_fill_txfifo(uint32_t dev_id)
{
	i3c_info_pt dev = i3c_handles[dev_id];
	int32_t cnt, i;
	uint32_t free;
	uint32_t data;

	// byte count yet to be sent
	cnt = dev->tx_size - dev->tx_count;

	if (cnt == 0) {
		return;
	}
	// free byte count in the TX FIFO
	free =
		4 * I3C_REG_READ_FIELD(I3C_DATA_BUFFER_STATUS_LEVEL,
				       TX_BUF_EMPTY_LOC);

	if (cnt > free) {
		cnt = free;
	}
	// Feed the TX FIFO, reversing the byte order
	i = 0;
	data = 0;
	do {
		data |=
			((uint32_t) (dev->tx_data[dev->tx_count++])) << ((i & 0x3) <<
									 3);
		++i;
		if ((i & 0x3) == 0) {
			I3C_REG_WRITE(I3C_TX_DATA_PORT, data);
			data = 0;
		}
	} while (i < cnt);

	// push out any bytes left
	if ((i & 0x3) != 0) {
		I3C_REG_WRITE(I3C_TX_DATA_PORT, data);
	}
	// If no more bytes left, mask the TX threshold interrupt
	if (dev->tx_count == dev->tx_size) {
		uint32_t val = I3C_REG_READ(I3C_INTR_SIGNAL_EN);
		I3C_REG_WRITE_FIELD(I3C_INTR_SIGNAL_EN, TX_THLD_SIGNAL_EN, val, 0);
	}
}

static void i3c_retrieve_rxfifo(uint32_t dev_id)
{
	i3c_info_pt dev = i3c_handles[dev_id];
	int32_t cnt, i;
	uint32_t avail;
	uint32_t data;

	// number of bytes we still expect to receive
	cnt = dev->rx_size - dev->rx_count;

	// number of bytes available in the RX FIFO
	avail =
		4 * I3C_REG_READ_FIELD(I3C_DATA_BUFFER_STATUS_LEVEL, RX_BUF_BLR);

	if (cnt > avail) {
		cnt = avail;
	}
	// at this point 'cnt' = min of (available, required)
	I3C_PRINT("retrieve %d bytes\n", cnt);

	if (cnt == 0) {
		return;
	}
	// read data from the RX FIFO, reversing the byte order
	i = 0;
	do {
		if ((i & 0x3) == 0) {
			data = I3C_REG_READ(I3C_RX_DATA_PORT);
		}

		dev->rx_data[dev->rx_count++] = (data >> ((i & 0x3) << 3)) & 0xFF;
		++i;
	} while (i < cnt);

	// if we don't expect any more data, mask the RX threshold interrupt
	if (dev->rx_count == dev->rx_size) {
		uint32_t val = I3C_REG_READ(I3C_INTR_SIGNAL_EN);
		I3C_REG_WRITE_FIELD(I3C_INTR_SIGNAL_EN, RX_THLD_SIGNAL_EN, val, 0);
	}
}

static void i3c_start_tx(uint32_t dev_id)
{
	i3c_info_pt dev = i3c_handles[dev_id];
	uint32_t val;
	uint32_t dma_flag = 0;

	dev->handling_tx = 1;

	if (dev->tx_size <= 3) {
		// push Short Data Argument into command queue
		val =
			I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, CMD_ATTR, 0,
					  I3C_R_CMD_ATTR_SHORT_DATA_ARG);

		switch (dev->tx_size) {
		case 3:
			val =
				I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, DATA_BYTE_3, val,
						  dev->tx_data[2]);
			val |= I3C_COMMAND_QUEUE_PORT_MSK_BYTE_STRB_3;
		// no break
		case 2:
			val =
				I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, DATA_BYTE_2, val,
						  dev->tx_data[1]);
			val |= I3C_COMMAND_QUEUE_PORT_MSK_BYTE_STRB_2;
		// no break
		case 1:
			val =
				I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, DATA_BYTE_1, val,
						  dev->tx_data[0]);
			val |= I3C_COMMAND_QUEUE_PORT_MSK_BYTE_STRB_1;
			break;
		default:
			break;
		}

		dev->tx_count = dev->tx_size;

		I3C_REG_WRITE(I3C_COMMAND_QUEUE_PORT, val);
	} else {

#ifdef __Xdmac

		if (dev->dmatxchanid != DMA_NONE) {
			dma_flag = 1;
		}
#endif

		// push Transfer Argument into command queue

		val =
			I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, CMD_ATTR, 0,
					  I3C_R_CMD_ATTR_TRANSFER_ARG);
		val =
			I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, DMA, val, dma_flag);
		val =
			I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, DATA_LENGTH, val,
					  dev->tx_size);
		I3C_REG_WRITE(I3C_COMMAND_QUEUE_PORT, val);
	}

	// clear interrupts
	I3C_REG_WRITE(I3C_INTR_STATUS, I3C_INTR_STATUS_MSK_EVENTS);

// push Write Transfer Command into command queue
	val =
		I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, CMD_ATTR, 0,
				  I3C_R_CMD_ATTR_TRANSFER_CMD);
	val = I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, CP, val, 0);
	val = I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, DEV_INDX, val, 0);      // always use the 0th slot
	val = I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, RNW, val, 0);           // write
	val = I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, TOC, val, 1);           // send STOP after the transmission
	val = I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, ROC, val, 1);           // require TX complete response
	val = I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, TID, val, I3C_TX_TID);  // TX magic number

	if (dev->tx_size <= 3) {
		val = I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, SDAP, val, 1);
	}
	I3C_REG_WRITE(I3C_COMMAND_QUEUE_PORT, val);

#ifdef __Xdmac

	if (dma_flag) { // only set for DMA transfers >= 4 bytes
		// set DMA burst size to data buffer threshold
		// val is the word count
		val =
			I3C_REG_READ_FIELD(I3C_DATA_BUFFER_THLD_CTRL,
					   TX_EMPTY_BUF_THLD);

		if (val == I3C_THLD_CTRL_THLD_1) {
			val = 1;
		} else {
			val = 2 << val;
		}

		if (val > dev->fifo_depth) {
			val = dev->fifo_depth;
		}

		i3c_start_dma_tx(dev, val);

	} else {
		// indicate that no DMA takes place
		dev->tx_dma_count = 0;
	}
#endif

	// unmask RESP_READY interrupt (and TX_THLD if there is data left to be sent)
	val = I3C_REG_READ(I3C_INTR_SIGNAL_EN);
	if (dev->tx_count != dev->tx_size) {
		val =
			I3C_REG_SET_FIELD(I3C_INTR_SIGNAL_EN, TX_THLD_SIGNAL_EN, val,
					  !dma_flag);
	} else {
		val =
			I3C_REG_SET_FIELD(I3C_INTR_SIGNAL_EN, TX_THLD_SIGNAL_EN, val,
					  0);
	}
	val =
		I3C_REG_SET_FIELD(I3C_INTR_SIGNAL_EN, RESP_READY_SIGNAL_EN, val,
				  1);
	I3C_REG_WRITE(I3C_INTR_SIGNAL_EN, val);
}

static void i3c_start_rx(uint32_t dev_id)
{
	i3c_info_pt dev = i3c_handles[dev_id];
	uint32_t val;
	uint32_t dma_flag = 0;

	dev->handling_rx = 1;

	// clear interrupts
	I3C_REG_WRITE(I3C_INTR_STATUS, I3C_INTR_STATUS_MSK_EVENTS);

	// clear RX fifo
	I3C_REG_WRITE(I3C_RESET_CTRL, I3C_RESET_CTRL_MSK_RX_FIFO_RST);
	val = I3C_REG_READ(I3C_RESET_CTRL); // dummy read

#ifdef __Xdmac
	if ((dev->dmarxchanid != DMA_NONE) && (dev->rx_size >= 4)) {
		dma_flag = 1;
	}
#endif

	// unmask RESP_READY and RX_THLD interrupts depending on the DMA flag
	val = I3C_REG_READ(I3C_INTR_SIGNAL_EN);
	val =
		I3C_REG_SET_FIELD(I3C_INTR_SIGNAL_EN, RESP_READY_SIGNAL_EN, val,
				  1);
	val =
		I3C_REG_SET_FIELD(I3C_INTR_SIGNAL_EN, RX_THLD_SIGNAL_EN, val,
				  !dma_flag);
	I3C_REG_WRITE(I3C_INTR_SIGNAL_EN, val);

	// push Transfer Argument command into the command queue
	val =
		I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, CMD_ATTR, 0,
				  I3C_R_CMD_ATTR_TRANSFER_ARG);
	val = I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, DMA, val, dma_flag);
	val =
		I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, DATA_LENGTH, val,
				  dev->rx_size);
	I3C_REG_WRITE(I3C_COMMAND_QUEUE_PORT, val);
	val = I3C_REG_READ(I3C_INTR_STATUS); // dummy read

	// push Private Read Transfer Command into command queue
	val =
		I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, CMD_ATTR, 0,
				  I3C_R_CMD_ATTR_TRANSFER_CMD);
	val = I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, CP, val, 0);
	val = I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, DEV_INDX, val, 0);      // always use 0th slot
	val = I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, RNW, val, 1);           // read
	val = I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, TOC, val, 1);           // send STOP after the transfer
	val = I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, ROC, val, 1);           // require RX complete response
	val = I3C_REG_SET_FIELD(I3C_COMMAND_QUEUE_PORT, TID, val, I3C_RX_TID);  // RX magic number
	I3C_REG_WRITE(I3C_COMMAND_QUEUE_PORT, val);
	val = I3C_REG_READ(I3C_INTR_STATUS);                                    // dummy read

#ifdef __Xdmac

	if (dma_flag) {
		// set DMA burst size to data buffer threshold
		// val is the word count
		val = I3C_REG_READ_FIELD(I3C_DATA_BUFFER_THLD_CTRL, RX_BUF_THLD);

		if (val == I3C_THLD_CTRL_THLD_1) {
			val = 1;
		} else {
			val = 2 << val;
		}

		if (val > dev->fifo_depth) {
			val = dev->fifo_depth;
		}

		i3c_start_dma_rx(dev, val);

	} else {
		// indicate that no DMA takes place
		dev->rx_dma_count = 0;
	}
#endif

}

static void i3c_reset_and_init(uint32_t dev_id)
{
	i3c_info_pt dev = i3c_handles[dev_id];
	uint32_t reg_val;

	// initialize data structure
	dev->handling_tx = dev->handling_rx = 0;
	dev->tx_data = dev->rx_data = NULL;

	// reset (and wait)
	I3C_REG_WRITE(I3C_RESET_CTRL, I3C_RESET_CTRL_MSK_RESET_ALL);
	while ((I3C_RESET_CTRL_MSK_RESET_ALL & I3C_REG_READ(I3C_RESET_CTRL)) !=
	       0) {
		;
	}

	// initialize RAM registers
	I3C_REG_WRITE(I3C_DEV_CHAR_TABLE1_LOC1, 0);
	I3C_REG_WRITE(I3C_DEV_CHAR_TABLE1_LOC2, 0);
	I3C_REG_WRITE(I3C_DEV_CHAR_TABLE1_LOC3, 0);
	I3C_REG_WRITE(I3C_DEV_CHAR_TABLE1_LOC4, 0);
	I3C_REG_WRITE(I3C_DEV_CHAR_TABLE2_LOC1, 0);
	I3C_REG_WRITE(I3C_DEV_CHAR_TABLE2_LOC2, 0);
	I3C_REG_WRITE(I3C_DEV_CHAR_TABLE2_LOC3, 0);
	I3C_REG_WRITE(I3C_DEV_CHAR_TABLE2_LOC4, 0);
	I3C_REG_WRITE(I3C_DEV_ADDR_TABLE_LOC1, 0);
	I3C_REG_WRITE(I3C_DEV_ADDR_TABLE_LOC2, 0);

	// configure queue and buffer thresholds
	reg_val = I3C_REG_READ(I3C_QUEUE_THLD_CTRL);
	I3C_REG_WRITE_FIELD(I3C_QUEUE_THLD_CTRL, RESP_BUF_THLD, reg_val,
			    I3C_THLD_CTRL_THLD_1);

	reg_val = I3C_REG_READ(I3C_DATA_BUFFER_THLD_CTRL);
	I3C_REG_WRITE_FIELD(I3C_DATA_BUFFER_THLD_CTRL, RX_BUF_THLD, reg_val,
			    I3C_THLD_CTRL_THLD_1);

	// Set the IBI (In-Band Interrupt) Related Registers
	// Nack's the Master Request and Auto-disables the Master Request
	// ACK the SIR (Slave Interrupt Request) Request
	I3C_REG_WRITE(I3C_IBI_MR_P2P_SEL, 0);
	I3C_REG_WRITE(I3C_IBI_MR_P2P_REQ_REJECT,
		      I3C_IBI_MR_P2P_REQ_REJECT_MSK_MR_REQ_REJECT);
	I3C_REG_WRITE(I3C_IBI_SIR_REQ_REJECT, 0);

	// clear all interrupts
	I3C_REG_WRITE(I3C_INTR_STATUS, I3C_INTR_STATUS_MSK_EVENTS);

	// Program the Interrupt related Registers - INTR_STATUS_EN and INTR_SIGNAL_EN
	// Limit to error interrupts
	// INTR_STATUS_EN may be allowed to reflect all interrupts
	I3C_REG_WRITE(I3C_INTR_STATUS_EN, I3C_INTR_STATUS_EN_MSK_ALL);

	I3C_REG_WRITE(I3C_INTR_SIGNAL_EN,
		      I3C_INTR_SIGNAL_EN_MSK_TRANSFER_ERR_SIGNAL_EN |
		      I3C_INTR_SIGNAL_EN_MSK_CMD_RETRY_FAILED_SIGNAL_EN |
		      I3C_INTR_SIGNAL_EN_MSK_DYN_ADDR_ASSGN_SIGNAL_EN |
		      I3C_INTR_SIGNAL_EN_MSK_TRANSFER_ABORT_SIGNAL_EN);
}

static void i3c_abort(uint32_t dev_id)
{
	i3c_info_pt dev = i3c_handles[dev_id];
	uint32_t reg_val, intr_en;

	// disable interrupt signals
	intr_en = I3C_REG_READ(I3C_INTR_SIGNAL_EN);
	I3C_REG_WRITE(I3C_INTR_SIGNAL_EN, 0);

	// clear interrupt status
	I3C_REG_WRITE(I3C_INTR_STATUS, I3C_INTR_STATUS_MSK_EVENTS);

	// flush all FIFO's
	I3C_REG_WRITE(I3C_RESET_CTRL, I3C_RESET_CTRL_MSK_CMD_QUEUE_RST |
		      I3C_RESET_CTRL_MSK_RESP_QUEUE_RST |
		      I3C_RESET_CTRL_MSK_TX_FIFO_RST |
		      I3C_RESET_CTRL_MSK_RX_FIFO_RST |
		      I3C_RESET_CTRL_MSK_IBI_QUEUE_RST);

	// abort (and wait)
	reg_val = I3C_REG_READ(I3C_DEVICE_CTRL);
	I3C_REG_WRITE_FIELD(I3C_DEVICE_CTRL, ABORT, reg_val, 1);
	// optionally may wait for ABORT flag to read as 0

	// wait fot halt state
	while (I3C_REG_READ_FIELD(I3C_PRESENT_STATE, CM_TFR_STATUS) !=
	       I3C_R_CM_TFR_STATUS_HALT) {
		;
	}

	// resume from halt
	I3C_REG_WRITE_FIELD(I3C_DEVICE_CTRL, RESUME, reg_val, 1);
	while (I3C_REG_READ_FIELD(I3C_PRESENT_STATE, CM_TFR_STATUS) !=
	       I3C_R_CM_TFR_STATUS_IDLE) {
		;
	}

	// restore interrupts to their previous value
	I3C_REG_WRITE(I3C_INTR_SIGNAL_EN, intr_en);

}

#if I3C_DEBUG_PRINT
static void print_all_registers(uint32_t dev_id)
{
	i3c_info_pt dev = i3c_handles[dev_id];
	uint32_t i;

	for (i = 0; i < reg_count; i++) {
		uint32_t reg = I3C_REG_READ(reg_offsets[i]);
		I3C_PRINT("%s = 0x%08X\n", reg_names[i], reg);
	}

	I3C_PRINT("-------------------------------------------\n");

}

static void print_status_registers(uint32_t dev_id)
{
	i3c_info_pt dev = i3c_handles[dev_id];
	uint32_t i;

	for (i = 0; i < reg_count_short; i++) {
		uint32_t reg = I3C_REG_READ(reg_offsets_short[i]);
		I3C_PRINT("%s = 0x%08X\n", reg_names_short[i], reg);
	}

	I3C_PRINT("-------------------------------------------\n");
}
#endif
/****************************************************************************
*
* API Functions (Definition)
*
****************************************************************************/

uint32_t io_i3c_master_open(uint32_t dev_id)
{
	uint32_t h = 0;
	uint32_t reg_val;
	i3c_info_pt dev;

	while ((i3c_master_devs[h].instID != dev_id)
	       && (i3c_master_devs[h].instID != I3C_MAX_CNT)) {
		h++;
	}
	if ((i3c_master_devs[h].instID == I3C_MAX_CNT)
	    || (0 != i3c_handles[dev_id])) {
		// dev_id is not part of design, or still open
		return 1;
	}
	i3c_handles[dev_id] = &i3c_master_devs[h];
	dev = i3c_handles[dev_id];

	// set interrupt vector
	_setvecti(dev->vector, dev->isr);

	// enable clock gate
	I3C_REG_WRITE(I3C_CLKEN, 1);
	reg_val = I3C_REG_READ(I3C_DEVICE_CTRL); // dummy read

	i3c_reset_and_init(dev_id);

#ifdef __Xdmac
	dev->dmarxdescriptor = &i3c_dmarx_descriptor[4 * h];
	dev->dmatxdescriptor = &i3c_dmatx_descriptor[4 * h];

	if (dev->dmarxchanid != DMA_NONE) {
		dev->dmarxdescriptor[1] = dev->reg_base + I3C_RX_DATA_PORT;
		dev->dmarxdescriptor[3] = 0;

		_setvecti(DMAC_INT_BASE + dev->dmarxchanid,
			  dev->isr_dma_rx_complete);
		_setvecti(DMAC_ERR_BASE + dev->dmarxchanid, dev->isr_dma_err);
	}

	if (dev->dmatxchanid != DMA_NONE) {
		dev->dmatxdescriptor[1] = dev->reg_base + I3C_TX_DATA_PORT;
		dev->dmatxdescriptor[3] = 0;

		_setvecti(DMAC_INT_BASE + dev->dmatxchanid,
			  dev->isr_dma_tx_complete);
		_setvecti(DMAC_ERR_BASE + dev->dmatxchanid, dev->isr_dma_err);
	}

	I3C_PRINT("io_i3c_master_open + DMA\n");
#endif

	// enable (and wait)
	reg_val = I3C_REG_READ(I3C_DEVICE_CTRL);
	reg_val = I3C_REG_SET_FIELD(I3C_DEVICE_CTRL, ENABLE, reg_val, 1);
	I3C_REG_WRITE(I3C_DEVICE_CTRL, reg_val);

	while (I3C_REG_READ_FIELD(I3C_DEVICE_CTRL, ENABLE) != 1) {
		;
	}

	// TODO timeout and return 1 ?

	return 0;
}

void io_i3c_master_close(uint32_t dev_id)
{
	i3c_info_pt dev = i3c_handles[dev_id];
	uint32_t device_ctrl;

	if (dev == NULL) {
		return;
	}
	// disable all interrupts
	I3C_REG_WRITE(I3C_INTR_SIGNAL_EN, 0);
	i3c_abort(dev_id);

	// remove ISR handlers
	_setvecti(dev->vector, NULL);

#ifdef __Xdmac
	if (dev->dmarxchanid != DMA_NONE) {
		_setvecti(DMAC_INT_BASE + dev->dmarxchanid, NULL);
		_setvecti(DMAC_ERR_BASE + dev->dmarxchanid, NULL);
		_dma_chan_reset(0x1 << dev->dmarxchanid);
	}

	if (dev->dmatxchanid != DMA_NONE) {
		_setvecti(DMAC_INT_BASE + dev->dmatxchanid, NULL);
		_setvecti(DMAC_ERR_BASE + dev->dmatxchanid, NULL);
		_dma_chan_reset(0x1 << dev->dmatxchanid);
	}
	I3C_PRINT("io_i3c_master_close + DMA\n");
#endif

	dev->rx_cb = NULL;
	dev->tx_cb = NULL;
	dev->err_cb = NULL;

	// disable (and wait)
	device_ctrl = I3C_REG_READ(I3C_DEVICE_CTRL);
	I3C_REG_WRITE_FIELD(I3C_DEVICE_CTRL, ENABLE, device_ctrl, 0);
	while (I3C_REG_READ_FIELD(I3C_DEVICE_CTRL, ENABLE) != 0) {
		;
	}

	// gate the I3C clock
	I3C_REG_WRITE(I3C_CLKEN, 0);

	i3c_handles[dev_id] = NULL;
}

void io_i3c_master_write(uint32_t dev_id, uint8_t *data, uint32_t *size)
{
	i3c_info_pt dev = i3c_handles[dev_id];

	if (dev->handling_tx == 1) {
		I3C_PRINT
			("io_i3c_master_write - return, write is already in progress!\n");
		return;
	}
	dev->tx_size = *size;
	dev->tx_count = 0;
	dev->tx_data = data;

	I3C_PRINT("io_i3c_master_write, size = %d\n", dev->tx_size);

	// check whether a master-rx is in progress; if so, postpone until RX completion */
	if (dev->handling_rx == 0) {
		i3c_start_tx(dev_id);
	}
}

void io_i3c_master_read(uint32_t dev_id, uint8_t *data, uint32_t *size)
{
	i3c_info_pt dev = i3c_handles[dev_id];

	if (dev->handling_rx == 1) {
		I3C_PRINT
			("io_i3c_master_read - return, read is already in progress!\n");
		return;
	}

	dev->p_rxsize = size;
	dev->rx_size = *size;
	dev->rx_count = 0;
	dev->rx_data = data;

	I3C_PRINT("io_i3c_master_read, size = %d\n", dev->rx_size);

	// check whether a master-TX is in progress; if so, postpone until TX completion */
	if (dev->handling_tx == 0) {
		i3c_start_rx(dev_id);
	}

}

void io_i3c_master_ioctl(uint32_t dev_id, uint32_t cmd, void *arg)
{
	uint32_t data = 0, device_ctrl = 0;
	i3c_info_pt dev = i3c_handles[dev_id];
	uint8_t par;

	switch (cmd) {
	case IO_SET_CB_RX:
		dev->rx_cb = ((io_cb_t *) arg)->cb;
		break;

	case IO_SET_CB_TX:
		dev->tx_cb = ((io_cb_t *) arg)->cb;
		break;

	case IO_SET_CB_ERR:
		dev->err_cb = ((io_cb_t *) arg)->cb;
		break;

	default:
	{
		// Software can disable DWC_mipi_i3c while it is active. However, the disable happens after all the initiated
		// commands are completed in the command queue and Master FSM is in IDLE state. Software can read back 1'b0
		// from this field once disabling of DWC_mipi_i3c is completed

		device_ctrl = I3C_REG_READ(I3C_DEVICE_CTRL);
		device_ctrl =
			I3C_REG_SET_FIELD(I3C_DEVICE_CTRL, ENABLE, device_ctrl, 0);
		I3C_REG_WRITE(I3C_DEVICE_CTRL, device_ctrl);

		while ((I3C_DEVICE_CTRL_MSK_ENABLE &
			I3C_REG_READ(I3C_DEVICE_CTRL)) != 0) {
			;
		}

		switch (cmd) {

		case IO_I3C_MASTER_SET_RX_START_THLD:
			data = I3C_REG_READ(I3C_DATA_BUFFER_THLD_CTRL);
			data =
				I3C_REG_SET_FIELD(I3C_DATA_BUFFER_THLD_CTRL,
						  RX_START_THLD, data,
						  (*((uint32_t *) arg)));
			I3C_REG_WRITE(I3C_DATA_BUFFER_THLD_CTRL, data);
			break;

		case IO_I3C_MASTER_SET_TX_START_THLD:
			data = I3C_REG_READ(I3C_DATA_BUFFER_THLD_CTRL);
			data =
				I3C_REG_SET_FIELD(I3C_DATA_BUFFER_THLD_CTRL,
						  TX_START_THLD, data,
						  (*((uint32_t *) arg)));
			I3C_REG_WRITE(I3C_DATA_BUFFER_THLD_CTRL, data);
			break;

		case IO_I3C_MASTER_SET_RX_BUF_THLD:
			data = I3C_REG_READ(I3C_DATA_BUFFER_THLD_CTRL);
			data =
				I3C_REG_SET_FIELD(I3C_DATA_BUFFER_THLD_CTRL,
						  RX_BUF_THLD, data,
						  (*((uint32_t *) arg)));
			I3C_REG_WRITE(I3C_DATA_BUFFER_THLD_CTRL, data);
			break;

		case IO_I3C_MASTER_SET_TX_EMPTY_THLD:
			data = I3C_REG_READ(I3C_DATA_BUFFER_THLD_CTRL);
			data =
				I3C_REG_SET_FIELD(I3C_DATA_BUFFER_THLD_CTRL,
						  TX_EMPTY_BUF_THLD, data,
						  (*((uint32_t *) arg)));
			I3C_REG_WRITE(I3C_DATA_BUFFER_THLD_CTRL, data);
			break;

		case IO_I3C_MASTER_SET_RESP_BUF_THLD:
			data = I3C_REG_READ(I3C_QUEUE_THLD_CTRL);
			data =
				I3C_REG_SET_FIELD(I3C_QUEUE_THLD_CTRL, RESP_BUF_THLD,
						  data, (*((uint32_t *) arg)));
			I3C_REG_WRITE(I3C_QUEUE_THLD_CTRL, data);
			break;

		case IO_I3C_MASTER_SET_CMD_EMPTY_BUF_THLD:
			data = I3C_REG_READ(I3C_QUEUE_THLD_CTRL);
			data =
				I3C_REG_SET_FIELD(I3C_QUEUE_THLD_CTRL,
						  CMD_EMPTY_BUF_THLD, data,
						  (*((uint32_t *) arg)));
			I3C_REG_WRITE(I3C_QUEUE_THLD_CTRL, data);
			break;

		case IO_I3C_MASTER_SET_DYNAMIC_ADDR:
			data = I3C_REG_READ(I3C_DEVICE_ADDR);
			data =
				I3C_REG_SET_FIELD(I3C_DEVICE_ADDR, DYNAMIC_ADDR_VALID,
						  data, (*((uint32_t *) arg) != 0x00));
			data =
				I3C_REG_SET_FIELD(I3C_DEVICE_ADDR, DYNAMIC_ADDR, data,
						  (*((uint32_t *) arg)));
			I3C_REG_WRITE(I3C_DEVICE_ADDR, data);
			break;

		case IO_I3C_MASTER_SET_TARGET_I2C_ADDR:
			data = I3C_REG_READ(I3C_DEVICE_CTRL);
			data =
				I3C_REG_SET_FIELD(I3C_DEVICE_CTRL, I2C_SLAVE_PRESENT,
						  data, 0x01);
			I3C_REG_WRITE(I3C_DEVICE_CTRL, data);
			data = I3C_REG_READ(I3C_DEV_ADDR_TABLE_LOC1);
			data =
				I3C_REG_SET_FIELD(I3C_DEV_ADDR_TABLE_LOC,
						  LEGACY_I2C_DEVICE, data, 0x01);
			data =
				I3C_REG_SET_FIELD(I3C_DEV_ADDR_TABLE_LOC,
						  DEV_NACK_RETRY_CNT, data, 0x01);
			data =
				I3C_REG_SET_FIELD(I3C_DEV_ADDR_TABLE_LOC,
						  DEV_STATIC_ADDR, data,
						  (*((uint32_t *) arg)));
			I3C_REG_WRITE(I3C_DEV_ADDR_TABLE_LOC1, data);
			//   I3C_REG_WRITE( I3C_DEV_ADDR_TABLE_LOC2, data );
			break;

		case IO_I3C_MASTER_SET_TARGET_DYNAMIC_ADDR:
			//  TODO: should we clear I2C_SLAVE_PRESENT ?
			//  data = I3C_REG_READ( I3C_DEVICE_CTRL );
			//  data = I3C_REG_SET_FIELD( I3C_DEVICE_CTRL, I2C_SLAVE_PRESENT, data, 0x01);
			//  I3C_REG_WRITE( I3C_DEVICE_CTRL, data );

			// calculate the odd parity bit of the 7 address bits (TODO: optimize)
			data = *((uint32_t *) arg);
			par = 1;
			for (uint8_t i = 0; i < 7; i++) {
				par ^= (data & 0x01);
				data >>= 1;
			}
			data = I3C_REG_READ(I3C_DEV_ADDR_TABLE_LOC1);
			data =
				I3C_REG_SET_FIELD(I3C_DEV_ADDR_TABLE_LOC,
						  LEGACY_I2C_DEVICE, data, 0x00);
			data =
				I3C_REG_SET_FIELD(I3C_DEV_ADDR_TABLE_LOC,
						  DEV_NACK_RETRY_CNT, data, 0x01);
			data =
				I3C_REG_SET_FIELD(I3C_DEV_ADDR_TABLE_LOC,
						  DEV_DYNAMIC_ADDR, data,
						  (((*((uint32_t *) arg)) & 0x7F) |
						   (par << 7)));
			I3C_REG_WRITE(I3C_DEV_ADDR_TABLE_LOC1, data);
			//    I3C_REG_WRITE( I3C_DEV_ADDR_TABLE_LOC2, data );
			break;

		case IO_I3C_MASTER_SET_TARGET_STATIC_ADDR:
			//  TODO: should we clear I2C_SLAVE_PRESENT ?
			//  data = I3C_REG_READ( I3C_DEVICE_CTRL );
			//  data = I3C_REG_SET_FIELD( I3C_DEVICE_CTRL, I2C_SLAVE_PRESENT, data, 0x01);
			//  I3C_REG_WRITE( I3C_DEVICE_CTRL, data );
			data = I3C_REG_READ(I3C_DEV_ADDR_TABLE_LOC1);
			data =
				I3C_REG_SET_FIELD(I3C_DEV_ADDR_TABLE_LOC,
						  LEGACY_I2C_DEVICE, data, 0x00);
			data =
				I3C_REG_SET_FIELD(I3C_DEV_ADDR_TABLE_LOC,
						  DEV_NACK_RETRY_CNT, data, 0x01);
			data =
				I3C_REG_SET_FIELD(I3C_DEV_ADDR_TABLE_LOC,
						  DEV_STATIC_ADDR, data,
						  (*((uint32_t *) arg)));
			I3C_REG_WRITE(I3C_DEV_ADDR_TABLE_LOC1, data);
			//   I3C_REG_WRITE( I3C_DEV_ADDR_TABLE_LOC2, data );
			break;

		case IO_I3C_MASTER_SET_I3C_PP_LCNT:
			data = I3C_REG_READ(I3C_SCL_LCNT_TIMING);
			data =
				I3C_REG_SET_FIELD(I3C_SCL_LCNT_TIMING, I3C_PP_LCNT,
						  data, (*((uint32_t *) arg)));
			I3C_REG_WRITE(I3C_SCL_LCNT_TIMING, data);
			break;

		case IO_I3C_MASTER_SET_I3C_OD_LCNT:
			data = I3C_REG_READ(I3C_SCL_LCNT_TIMING);
			data =
				I3C_REG_SET_FIELD(I3C_SCL_LCNT_TIMING, I3C_OD_LCNT,
						  data, (*((uint32_t *) arg)));
			I3C_REG_WRITE(I3C_SCL_LCNT_TIMING, data);
			break;

		case IO_I3C_MASTER_SET_I2C_OD_LCNT:
			data = I3C_REG_READ(I3C_SCL_LCNT_TIMING);
			data =
				I3C_REG_SET_FIELD(I3C_SCL_LCNT_TIMING, I2C_OD_LCNT,
						  data, (*((uint32_t *) arg)));
			I3C_REG_WRITE(I3C_SCL_LCNT_TIMING, data);
			break;

		case IO_I3C_MASTER_SET_I3C_PP_HCNT:
			data = I3C_REG_READ(I3C_SCL_HCNT_TIMING);
			data =
				I3C_REG_SET_FIELD(I3C_SCL_HCNT_TIMING, I3C_PP_HCNT,
						  data, (*((uint32_t *) arg)));
			I3C_REG_WRITE(I3C_SCL_HCNT_TIMING, data);
			break;

		case IO_I3C_MASTER_SET_I3C_OD_HCNT:
			data = I3C_REG_READ(I3C_SCL_HCNT_TIMING);
			data =
				I3C_REG_SET_FIELD(I3C_SCL_HCNT_TIMING, I3C_OD_HCNT,
						  data, (*((uint32_t *) arg)));
			I3C_REG_WRITE(I3C_SCL_HCNT_TIMING, data);
			break;

		case IO_I3C_MASTER_SET_I2C_OD_HCNT:
			data = I3C_REG_READ(I3C_SCL_HCNT_TIMING);
			data =
				I3C_REG_SET_FIELD(I3C_SCL_HCNT_TIMING, I2C_OD_HCNT,
						  data, (*((uint32_t *) arg)));
			I3C_REG_WRITE(I3C_SCL_HCNT_TIMING, data);
			break;

		case IO_I3C_MASTER_RESET:
			i3c_reset_and_init(dev_id);
			break;

		default:
			break;
		}

		device_ctrl = I3C_REG_READ(I3C_DEVICE_CTRL);
		I3C_REG_WRITE(I3C_DEVICE_CTRL,
			      (device_ctrl | I3C_DEVICE_CTRL_MSK_ENABLE));

		while (I3C_REG_READ_FIELD(I3C_DEVICE_CTRL, ENABLE) != 1) {
			;
		}

		break;
	}
	}
}

#endif              // I3C_DEV_PRESENT
