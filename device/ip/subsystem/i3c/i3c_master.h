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

#ifndef I3C_MASTER_H_
#define I3C_MASTER_H_

#include "ioctl.h"

/****************************************************************************
 *
 * Public header for I3C Master driver
 * Defines:
 * - Public API functions
 * - I/O Control commands
 * - Useful constants for I/O Control parameters
 *
 ****************************************************************************/


// Public API functions

extern uint32_t io_i3c_master_open ( uint32_t dev_id );
extern void     io_i3c_master_close( uint32_t dev_id );
extern void     io_i3c_master_ioctl( uint32_t dev_id, uint32_t cmd,   void * arg );
extern void     io_i3c_master_read ( uint32_t dev_id, uint8_t * data, uint32_t * size );
extern void     io_i3c_master_write( uint32_t dev_id, uint8_t * data, uint32_t * size );




// I3C Master I/O Control command values

// Threshold configuration                              // supported values are I3C_THLD_CTRL_THLD_*
#define IO_I3C_MASTER_SET_RX_START_THLD         0x04    // Default: I3C_THLD_CTRL_THLD_4 - Receive Start Threshold Value
#define IO_I3C_MASTER_SET_TX_START_THLD         0x05    // Default: I3C_THLD_CTRL_THLD_4 - Transfer Start Threshold Value
#define IO_I3C_MASTER_SET_RX_BUF_THLD           0x06    // Default: I3C_THLD_CTRL_THLD_4 - Receive Buffer Threshold Value (to trigger the RX_THLD_STAT interrupt)
#define IO_I3C_MASTER_SET_TX_EMPTY_THLD         0x07    // Default: I3C_THLD_CTRL_THLD_4 - Transmit Buffer Threshold Value (to trigger the TX_THLD_STAT interrupt)
#define IO_I3C_MASTER_SET_RESP_BUF_THLD         0x08    // Default: I3C_THLD_CTRL_THLD_4 - Response Buffer Threshold Value (to trigger the RESP_READY_STAT_INTR interrupt)
#define IO_I3C_MASTER_SET_CMD_EMPTY_BUF_THLD    0x09    // Default: I3C_THLD_CTRL_THLD_4 - Command Buffer Empty Threshold Value (to trigger CMD_QUEUE_READY_STAT interrupt)

// Device address configuration
#define IO_I3C_MASTER_SET_DYNAMIC_ADDR          0x0A    // if address is 0, the "valid" flag is cleared

// Target address configuration
#define IO_I3C_MASTER_SET_TARGET_I2C_ADDR       0x0C    // automatically sets the LEGACY_I2C_DEVICE flag and I2C_SLAVE_PRESENT flag
#define IO_I3C_MASTER_SET_TARGET_DYNAMIC_ADDR   0x0D    // automatically calulates CRC and clears the LEGACY_I2C_DEVICE flag
#define IO_I3C_MASTER_SET_TARGET_STATIC_ADDR    0x0E    // automatically clears the LEGACY_I2C_DEVICE flag

// Timing configuration
#define IO_I3C_MASTER_SET_I3C_PP_LCNT           0x0F    // Default: 0x0A - I3C Push Pull Low Count
#define IO_I3C_MASTER_SET_I3C_OD_LCNT           0x10    // Default: 0x10 - I3C Open Drain Low Count
#define IO_I3C_MASTER_SET_I2C_OD_LCNT           0x11    // Default: 0x10 - I2C Open Drain Low Count
#define IO_I3C_MASTER_SET_I3C_PP_HCNT           0x12    // Default: 0x0A - I3C Push Pull High Count
#define IO_I3C_MASTER_SET_I3C_OD_HCNT           0x13    // Default: 0x0A - I3C Open Drain High Count
#define IO_I3C_MASTER_SET_I2C_OD_HCNT           0x14    // Default: 0x10 - I2C Open Drain High Count

// Peripheral Control
#define IO_I3C_MASTER_RESET                     0x15    // Soft-Reset the I3C peripheral



// Threshold field values for IO_I3C_MASTER_SET_*_THLD

#define I3C_THLD_CTRL_THLD_1    0x00
#define I3C_THLD_CTRL_THLD_4    0x01
#define I3C_THLD_CTRL_THLD_8    0x02
#define I3C_THLD_CTRL_THLD_16   0x03
#define I3C_THLD_CTRL_THLD_32   0x04
#define I3C_THLD_CTRL_THLD_64   0x05
#define I3C_THLD_CTRL_THLD_128  0x06
#define I3C_THLD_CTRL_THLD_256  0x07




#endif // I3C_MASTER_H_ 
