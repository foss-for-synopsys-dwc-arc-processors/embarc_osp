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

#ifndef I3C_PRIV_H_
#define I3C_PRIV_H_

#include "ioctl.h"

/****************************************************************************
 *
 * Internal header for I3C Master driver
 * Defines:
 * - Register offsets
 * - Register fields (both offsets and masks)
 * - Useful register values
 * - Driver internal data structure
 *
 ****************************************************************************/

 #define        DMA_NONE    (0xFF)
 

typedef _Interrupt void (*I3C_ISR) ();


// Private data structure maintained by the driver 
typedef struct i3c_info
{
    uint32_t        reg_base;       // base address of device register set
    uint8_t         instID;

    // TX & RX Buffer and lengths 
    uint8_t        *tx_data;
    uint32_t        tx_size;
    uint32_t        tx_count;

    uint8_t        *rx_data;
    uint32_t        rx_size;
    uint32_t        rx_count;
    uint32_t       *p_rxsize;

    uint8_t         handling_tx;
    uint8_t         handling_rx;

    uint16_t        fifo_depth;

    // Callbacks 
    IO_CB_FUNC      tx_cb;
    IO_CB_FUNC      rx_cb;
    IO_CB_FUNC      err_cb;

    // Interrupt numbers and handlers 
    uint8_t         vector;
    I3C_ISR         isr;
    
    uint16_t        dmarxchanid, dmatxchanid;
#ifdef __Xdmac
    _Uncached uint32_t *dmarxdescriptor;
    _Uncached uint32_t *dmatxdescriptor;
    I3C_ISR         isr_dma_err;
    I3C_ISR         isr_dma_rx_complete;
    I3C_ISR         isr_dma_tx_complete;
    
    uint16_t        rx_dma_count;   // the total number of bytes transferred after the ongoing DMA transaction is complete
    uint16_t        tx_dma_count;   // the total number of bytes transferred after the ongoing DMA transaction is complete

#endif
} i3c_info_t, *i3c_info_pt;



#ifdef __Xdmac

// single block
// Manual-request
// Auxiliary to Memory
// dw=word, inc=word
// Source Address not incremented, Destination Address incremented
//#define     I3C_MST_DMA_RX_CTRL            (0x800000B5)

#define DMACTRLx_SIZE_POS (8)
#define DMACTRLx_BSIZE_POS (21)

#define DMACTRLx_OP(x)    (x)           // 0,1
#define DMACTRLx_R(x)     ((x)<<2)      // 2
#define DMACTRLx_DTT(x)   ((x)<<3)      // 3,4
#define DMACTRLx_DWINC(x) ((x)<<5)      // 5,6,7
#define DMACTRLx_SIZE(x)  ((x)<<8)      // 8..20
#define DMACTRLx_BSIZE(x) ((x)<<21)     // 21..28
#define DMACTRLx_INT(x)   ((x)<<29)     // 29
#define DMACTRLx_AM(x)    ((x)<<30)     // 30,31

#define DMACTRLx_OP_MASK    DMACTRLx_OP(0x3)        // 0,1
#define DMACTRLx_R_MASK     DMACTRLx_R(0x1)         // 2
#define DMACTRLx_DTT_MASK   DMACTRLx_DTT(0x3)       // 3,4
#define DMACTRLx_DWINC_MASK DMACTRLx_DWINC(0x7)     // 5,6,7
#define DMACTRLx_SIZE_MASK  DMACTRLx_SIZE(0x1FFF)   // 8..20
#define DMACTRLx_BSIZE_MASK DMACTRLx_BSIZE(0xFF)    // 21..28
#define DMACTRLx_INT_MASK   DMACTRLx_INT(0x1)       // 29
#define DMACTRLx_AM_MASK    DMACTRLx_AM(0x3)        // 30,31


#define DMACTRLx_OP_SINGLE    (DMACTRLx_OP(1))
#define DMACTRLx_REQ_MANUAL   (DMACTRLx_R(1))
#define DMACTRLx_DTT_AUX_MEM  (DMACTRLx_DTT(2))
#define DMACTRLx_DTT_MEM_AUX  (DMACTRLx_DTT(1))
#define DMACTRLx_DW_WORD_WORD (DMACTRLx_DWINC(5))
#define DMACTRLx_INT_ENABLE   (DMACTRLx_INT(1))
#define DMACTRLx_AM_SRC_INC   (DMACTRLx_AM(1))
#define DMACTRLx_AM_DST_INC   (DMACTRLx_AM(2))

#define I3C_MST_DMA_RX_CTRL     (DMACTRLx_OP_SINGLE | DMACTRLx_REQ_MANUAL | DMACTRLx_DTT_AUX_MEM | DMACTRLx_DW_WORD_WORD | DMACTRLx_AM_DST_INC | DMACTRLx_INT_ENABLE)
#define I3C_MST_DMA_TX_CTRL     (DMACTRLx_OP_SINGLE | DMACTRLx_REQ_MANUAL | DMACTRLx_DTT_MEM_AUX | DMACTRLx_DW_WORD_WORD | DMACTRLx_AM_SRC_INC | DMACTRLx_INT_ENABLE)

    // DMACTRLx bit fields
    // 0..1 - Operation (0 = Invalid, 1 = Single Block, 2 = Linked-List (Auto-Request), 3 = Linked-List (Manual-Request))
    // 2    - Request Type (0 = Auto, 1 = Manual)
    // 3..4 - Data Transfer Type (0 = Mem->Mem, 1 = Mem->Aux, 2 = Aux->Mem, 3 = Aux->Aux)
    // 5..7 - Data Word / Increment
    //        000 = Byte      / Byte,
    //        001 = Byte      / Half-Word
    //        010 = Byte      / Word
    //        011 = Half-Word / Half-Word
    //        100 = Half-Word / Word
    //        101 = Word      / Word
    //        110 = Word      / Word    (Clear mode)
    // 8..20 - block size (13-bit, up to 8192, minus-1 encoded)
    // 21..28 - burst size (8-bit)
    // 29   - Interrupt Enable
    // 30..31 - Address Update Mode (bit 30 = Source Increment, bit 31 = Destination Increment)






#endif


// BIT_MASK macro - creates bit mask with <len> bits set starting from 0th bit. 0 <= len <= 31
#define BIT_MASK(len)       (((uint32_t)1 << (len)) - 1)
// UAUX_ADDR macro - converts byte address (from DesignWare databook) to 32-bit base UAUX address
#define UAUX_ADDR(offset)  ((offset) >> 2)



// Magic numbers for identifying transfer responses
#define     I3C_TX_TID  0x05
#define     I3C_RX_TID  0x03


// Configuration of variable-bit width fields.
// Should probably come from hwconfig
#define IC_RAM_DAA_ABW      4    // ??? TBD: IC_RAM_DAA_ABW =[function_of: IC_DEV_CHAR_TABLE_BUF_DEPTH]
#define IC_FREE_CNT_REG_WD  2    // ??? TBD: [function_of: IC_FREE_CNT_DIV_FACT], IC_FREE_CNT_DIV_FACT = (1000/IC_CLK_PERIOD)
#define IC_IDL_CNT_REG_WD   1    // ??? TBD: [function_of: IC_IDL_CNT_DIV_FACT],  IC_IDL_CNT_DIV_FACT = (1000000/IC_CLK_PERIOD)


// Just for the reference, the actual values are defined in hwconfig
//#define IO_I3C0_BASE_ADDR  (0x80000000 + 0x100000)
//#define IO_I3C1_BASE_ADDR  (0x80000000 + 0x100200)
//#define IO_I3C2_BASE_ADDR  (0x80000000 + 0x100400)
//#define IO_I3C3_BASE_ADDR  (0x80000000 + 0x100600)


/*  I3C device registers */
#define I3C_DEVICE_CTRL                 UAUX_ADDR(0x0000)  //      Read/Write  <TBD>               Device Configuration Register
#define I3C_DEVICE_ADDR                 UAUX_ADDR(0x0004)  //      Read/Write  <TBD>               Device Address Register
#define I3C_HW_CAPABILITY               UAUX_ADDR(0x0008)
#define I3C_COMMAND_QUEUE_PORT          UAUX_ADDR(0x000C)  //      Write Only  N.A.                Command Queue Port Register
#define I3C_RESPONSE_QUEUE_PORT         UAUX_ADDR(0x0010)  //      Read Only   0x0                 Response Queue Port Register
#define I3C_TX_DATA_PORT                UAUX_ADDR(0x0014)  //      Read/Write  0x0                 Receive and Transmit Data Port Register
#define I3C_RX_DATA_PORT                UAUX_ADDR(0x0014)  //      Read/Write  0x0                 Receive and Transmit Data Port Register
#define I3C_IBI_QUEUE_DATA              UAUX_ADDR(0x0018)  //      Read Only   0x0                 In-Band Interrupt Queue Data and Status Register
#define I3C_IBI_QUEUE_STATUS            UAUX_ADDR(0x0018)  //      Read Only   0x0                 In-Band Interrupt Queue Data and Status Register
#define I3C_QUEUE_THLD_CTRL             UAUX_ADDR(0x001C)  //      Read/Write  <TBD>               Queue Threshold Register
#define I3C_DATA_BUFFER_THLD_CTRL       UAUX_ADDR(0x0020)  //      Read/Write  <TBD>               Data Buffer Threshold Register
#define I3C_IBI_QUEUE_CTRL              UAUX_ADDR(0x0024)  //      Read/Write  0x0                 IBI Queue Control Register
#define I3C_IBI_MR_P2P_SEL              UAUX_ADDR(0x0028)
#define I3C_IBI_MR_P2P_REQ_REJECT       UAUX_ADDR(0x002C)  //      Read/Write  0x0                 IBI Master or P2P Request Rejection Control Register
#define I3C_IBI_SIR_REQ_REJECT          UAUX_ADDR(0x0030)  //      Read/Write  0x0                 IBI SIR Request Rejection Control
#define I3C_RESET_CTRL                  UAUX_ADDR(0x0034)  //      Read/Write  0x0                 Reset Control Register
//#define I3C_SLV_EVENT_CTRL              UAUX_ADDR(0x0038)  //      Read/Write  0x0                 Slave Event Control Register
#define I3C_INTR_STATUS                 UAUX_ADDR(0x003C)  //      Read/Write  0x0                 Interrupt Status Register
#define I3C_INTR_STATUS_EN              UAUX_ADDR(0x0040)  //      Read/Write  0x0                 Interrupt Status Enable Register
#define I3C_INTR_SIGNAL_EN              UAUX_ADDR(0x0044)  //      Read/Write  0x0                 Interrupt Signal Enable Register
#define I3C_INTR_FORCE                  UAUX_ADDR(0x0048)  //      Read/Write  0x0                 Interrupt Force Enable Register
#define I3C_QUEUE_STATUS_LEVEL          UAUX_ADDR(0x004C)  //      Read Only   0x0                 Queue Status Level Register
#define I3C_DATA_BUFFER_STATUS_LEVEL    UAUX_ADDR(0x0050)  //      Read Only   0x0                 Data Buffer Status Level Register
#define I3C_PRESENT_STATE               UAUX_ADDR(0x0054)  //      Read Only   0x3                 Present State Register
//#define I3C_CCC_DEVICE_STATUS           UAUX_ADDR(0x0058)  //      Read Only   0x0 (volatile)
#define I3C_DEVICE_ADDR_TABLE_POINTER   UAUX_ADDR(0x005C)  //      Read/Write  Config. dependent   Pointer for Device Address Table Registers
#define I3C_DEV_CHAR_TABLE_POINTER      UAUX_ADDR(0x0060)  //      Read/Write  Config. dependent   Pointer for Device Charcteristic Table
//#define I3C_CCC_PARAM_TABLE_POINTER     UAUX_ADDR(0x0064)  //      Read/Write  Config. dependent   Pointer for CCC Parameter Table Registers
//#define I3C_DMA_SPECIFIC_REG_POINTER    UAUX_ADDR(0x0068)  //      Read/Write  0x100               Pointer for DMA specific Registers
#define I3C_VENDOR_SPECIFIC_REG_POINTER UAUX_ADDR(0x006C)  //      Read/Write  0xb0                Pointer for Vendor specific Registers
#define I3C_DEVICE_CTRL_EXTENDED        UAUX_ADDR(0x00B0)  //      Read/Write  Config. dependent   Device Role Register
#define I3C_SCL_HCNT_TIMING             UAUX_ADDR(0x00B4)  //      Read/Write  0x0                 SCL High Count Timing Register
#define I3C_SCL_LCNT_TIMING             UAUX_ADDR(0x00B8)  //      Read/Write  0x0                 SCL Low Count Timing Register
#define I3C_SCL_EXT_LCNT_TIMING         UAUX_ADDR(0x00BC)  //      Read/Write  0x0                 SCL Extended Low Count Timing Register
#define I3C_BUS_FREE_TIMING             UAUX_ADDR(0x00C0)  //      Read/Write  0x0                 Bus Free Timing Register
#define I3C_BUS_IDLE_TIMING             UAUX_ADDR(0x00C4)  //      Read/Write  0x0                 Bus IdleTiming Register
#define I3C_SCL_EXT_TERMN_LCNT_TIMING   UAUX_ADDR(0x00C8)
#define I3C_VER_ID                      UAUX_ADDR(0x00E0)
#define I3C_VER_TYPE                    UAUX_ADDR(0x00E4)
#define I3C_EXTENDED_CAPABILITY         UAUX_ADDR(0x00E8)
#define I3C_DEV_CHAR_TABLE1_LOC1        0x0080
#define I3C_DEV_CHAR_TABLE1_LOC2        0x0081
#define I3C_DEV_CHAR_TABLE1_LOC3        0x0082
#define I3C_DEV_CHAR_TABLE1_LOC4        0x0083
#define I3C_DEV_CHAR_TABLE2_LOC1        0x0084
#define I3C_DEV_CHAR_TABLE2_LOC2        0x0085
#define I3C_DEV_CHAR_TABLE2_LOC3        0x0086
#define I3C_DEV_CHAR_TABLE2_LOC4        0x0087
#define I3C_DEV_ADDR_TABLE_LOC1         0x0088
#define I3C_DEV_ADDR_TABLE_LOC2         0x0089
#define I3C_CLKEN                       0x01FF



/* I3C_DEVICE_CTRL register fields */

#define I3C_DEVICE_CTRL_POS_IBA_INCLUDE         0
#define I3C_DEVICE_CTRL_POS_IBA_ARB_BITS        1
#define I3C_DEVICE_CTRL_POS_ARB_RETRY_CNT       4
#define I3C_DEVICE_CTRL_POS_I2C_SLAVE_PRESENT   7
#define I3C_DEVICE_CTRL_POS_HOT_JOIN_CTRL       8
#define I3C_DEVICE_CTRL_POS_ABORT               29
#define I3C_DEVICE_CTRL_POS_RESUME              30
#define I3C_DEVICE_CTRL_POS_ENABLE              31

#define I3C_DEVICE_CTRL_MSK_IBA_INCLUDE        (BIT_MASK(1) << I3C_DEVICE_CTRL_POS_IBA_INCLUDE)       // This bit is used to include I3C broadcast address (0x7E) for private transfer
#define I3C_DEVICE_CTRL_MSK_IBA_ARB_BITS       (BIT_MASK(3) << I3C_DEVICE_CTRL_POS_IBA_ARB_BITS)      // This field is used to enable the I3C broadcast address (0x7E) optimization for CCC and private transfers
#define I3C_DEVICE_CTRL_MSK_ARB_RETRY_CNT      (BIT_MASK(3) << I3C_DEVICE_CTRL_POS_ARB_RETRY_CNT)     // 0 : Retry on Arbitration loss until transfer is successful,  1..7 : Maximum of 1..7 retries will be attempted
#define I3C_DEVICE_CTRL_MSK_I2C_SLAVE_PRESENT  (BIT_MASK(1) << I3C_DEVICE_CTRL_POS_I2C_SLAVE_PRESENT) // This Bit indicates whether any Legacy I2C Devices are present in the system
#define I3C_DEVICE_CTRL_MSK_HOT_JOIN_CTRL      (BIT_MASK(1) << I3C_DEVICE_CTRL_POS_HOT_JOIN_CTRL)     // 0x0 (DISABLED): Ack Hot-Join requests; 0x1 (ENABLED): Nack and auto-disable Hot-Join request
#define I3C_DEVICE_CTRL_MSK_ABORT              (BIT_MASK(1) << I3C_DEVICE_CTRL_POS_ABORT)             // In response to an ABORT request, the controller issues the STOP condition after the complete data byte is transferred or received
#define I3C_DEVICE_CTRL_MSK_RESUME             (BIT_MASK(1) << I3C_DEVICE_CTRL_POS_RESUME)            // This bit is used to resume the Controller after it goes to Halt state
#define I3C_DEVICE_CTRL_MSK_ENABLE             (BIT_MASK(1) << I3C_DEVICE_CTRL_POS_ENABLE)            // 1: Enables the DWC_mipi_i3c controller; 0: Disables the DWC_mipi_i3c controller

/* I3C_DEVICE_ADDR register fields */

#define I3C_DEVICE_ADDR_POS_STATIC_ADDR        0
#define I3C_DEVICE_ADDR_POS_STATIC_ADDR_VALID  15
#define I3C_DEVICE_ADDR_POS_DYNAMIC_ADDR       16
#define I3C_DEVICE_ADDR_POS_DYNAMIC_ADDR_VALID 31

#define I3C_DEVICE_ADDR_MSK_STATIC_ADDR        (BIT_MASK(7) << I3C_DEVICE_ADDR_POS_STATIC_ADDR)
#define I3C_DEVICE_ADDR_MSK_STATIC_ADDR_VALID  (BIT_MASK(1) << I3C_DEVICE_ADDR_POS_STATIC_ADDR_VALID)
#define I3C_DEVICE_ADDR_MSK_DYNAMIC_ADDR       (BIT_MASK(7) << I3C_DEVICE_ADDR_POS_DYNAMIC_ADDR)
#define I3C_DEVICE_ADDR_MSK_DYNAMIC_ADDR_VALID (BIT_MASK(1) << I3C_DEVICE_ADDR_POS_DYNAMIC_ADDR_VALID)

/* 5.1.4 COMMAND_QUEUE_PORT_ADDR_ASSGN_CMD - I3C_COMMAND_QUEUE_PORT register fields */

#define I3C_COMMAND_QUEUE_PORT_POS_TOC          30
#define I3C_COMMAND_QUEUE_PORT_POS_ROC          26
#define I3C_COMMAND_QUEUE_PORT_POS_DEV_COUNT    21  //21:25
#define I3C_COMMAND_QUEUE_PORT_POS_DEV_INDX     16  //16:20
#define I3C_COMMAND_QUEUE_PORT_POS_CMD          7   //7:14
#define I3C_COMMAND_QUEUE_PORT_POS_TID          3   //3:6
#define I3C_COMMAND_QUEUE_PORT_POS_CMD_ATTR     0   //0:2

#define I3C_COMMAND_QUEUE_PORT_MSK_TOC         (BIT_MASK(1) << I3C_COMMAND_QUEUE_PORT_POS_TOC)
#define I3C_COMMAND_QUEUE_PORT_MSK_ROC         (BIT_MASK(1) << I3C_COMMAND_QUEUE_PORT_POS_ROC)
#define I3C_COMMAND_QUEUE_PORT_MSK_DEV_COUNT   (BIT_MASK(5) << I3C_COMMAND_QUEUE_PORT_POS_DEV_COUNT)
#define I3C_COMMAND_QUEUE_PORT_MSK_DEV_INDX    (BIT_MASK(5) << I3C_COMMAND_QUEUE_PORT_POS_DEV_INDX)
#define I3C_COMMAND_QUEUE_PORT_MSK_CMD         (BIT_MASK(8) << I3C_COMMAND_QUEUE_PORT_POS_CMD)
#define I3C_COMMAND_QUEUE_PORT_MSK_TID         (BIT_MASK(4) << I3C_COMMAND_QUEUE_PORT_POS_TID)
#define I3C_COMMAND_QUEUE_PORT_MSK_CMD_ATTR    (BIT_MASK(3) << I3C_COMMAND_QUEUE_PORT_POS_CMD_ATTR)

/* 5.1.5 COMMAND_QUEUE_PORT_TRANSFER_COMMAND - I3C_COMMAND_QUEUE_PORT register fields */

//      I3C_COMMAND_QUEUE_PORT_POS_TOC      30
#define I3C_COMMAND_QUEUE_PORT_POS_RNW      28
#define I3C_COMMAND_QUEUE_PORT_POS_SDAP     27
//      I3C_COMMAND_QUEUE_PORT_POS_ROC      26
#define I3C_COMMAND_QUEUE_PORT_POS_SPEED    21 //23:21
//      I3C_COMMAND_QUEUE_PORT_POS_DEV_INDX 16  //16:20
#define I3C_COMMAND_QUEUE_PORT_POS_CP       15
//      I3C_COMMAND_QUEUE_PORT_POS_CMD      7   //7:14
//      I3C_COMMAND_QUEUE_PORT_POS_TID      3   //3:6
//      I3C_COMMAND_QUEUE_PORT_POS_CMD_ATTR 0   //0:2

#define I3C_COMMAND_QUEUE_PORT_MSK_RNW     (BIT_MASK(1) << I3C_COMMAND_QUEUE_PORT_POS_RNW)
#define I3C_COMMAND_QUEUE_PORT_MSK_SDAP    (BIT_MASK(1) << I3C_COMMAND_QUEUE_PORT_POS_SDAP)
#define I3C_COMMAND_QUEUE_PORT_MSK_SPEED   (BIT_MASK(3) << I3C_COMMAND_QUEUE_PORT_POS_SPEED)
#define I3C_COMMAND_QUEUE_PORT_MSK_CP      (BIT_MASK(1) << I3C_COMMAND_QUEUE_PORT_POS_CP)

/* 5.1.6 COMMAND_QUEUE_PORT_SHORT_DATA_ARG - I3C_COMMAND_QUEUE_PORT register fields */

#define I3C_COMMAND_QUEUE_PORT_POS_DATA_BYTE_3 24  //31:24
#define I3C_COMMAND_QUEUE_PORT_POS_DATA_BYTE_2 16  //23:16
#define I3C_COMMAND_QUEUE_PORT_POS_DATA_BYTE_1 8   //15:8
#define I3C_COMMAND_QUEUE_PORT_POS_BYTE_STRB   3   //3:5
#define I3C_COMMAND_QUEUE_PORT_POS_BYTE_STRB_3 5
#define I3C_COMMAND_QUEUE_PORT_POS_BYTE_STRB_2 4
#define I3C_COMMAND_QUEUE_PORT_POS_BYTE_STRB_1 3
//      I3C_COMMAND_QUEUE_PORT_POS_CMD_ATTR    0   //0:2

#define I3C_COMMAND_QUEUE_PORT_MSK_BYTE_STRB    (BIT_MASK(3) << I3C_COMMAND_QUEUE_PORT_POS_BYTE_STRB)
#define I3C_COMMAND_QUEUE_PORT_MSK_BYTE_STRB_1  (BIT_MASK(1) << I3C_COMMAND_QUEUE_PORT_POS_BYTE_STRB_1)
#define I3C_COMMAND_QUEUE_PORT_MSK_BYTE_STRB_2  (BIT_MASK(1) << I3C_COMMAND_QUEUE_PORT_POS_BYTE_STRB_2)
#define I3C_COMMAND_QUEUE_PORT_MSK_BYTE_STRB_3  (BIT_MASK(1) << I3C_COMMAND_QUEUE_PORT_POS_BYTE_STRB_3)
#define I3C_COMMAND_QUEUE_PORT_MSK_DATA_BYTE_1  (BIT_MASK(8) << I3C_COMMAND_QUEUE_PORT_POS_DATA_BYTE_1)
#define I3C_COMMAND_QUEUE_PORT_MSK_DATA_BYTE_2  (BIT_MASK(8) << I3C_COMMAND_QUEUE_PORT_POS_DATA_BYTE_2)
#define I3C_COMMAND_QUEUE_PORT_MSK_DATA_BYTE_3  (BIT_MASK(8) << I3C_COMMAND_QUEUE_PORT_POS_DATA_BYTE_3)

#define I3C_COMMAND_QUEUE_PORT_MSK_DATA_ALLBYTES  (I3C_COMMAND_QUEUE_PORT_MSK_DATA_BYTE_1 | \
                                                   I3C_COMMAND_QUEUE_PORT_MSK_DATA_BYTE_2 | \
                                                   I3C_COMMAND_QUEUE_PORT_MSK_DATA_BYTE_3)


/* 5.1.7 COMMAND_QUEUE_PORT_TRANSFER_ARGUMENT - I3C_COMMAND_QUEUE_PORT register fields */

#define I3C_COMMAND_QUEUE_PORT_POS_DATA_LENGTH 16   //31:16
#define I3C_COMMAND_QUEUE_PORT_POS_DMA         15   //15
//      I3C_COMMAND_QUEUE_PORT_POS_CMD_ATTR    0    //0:2

#define I3C_COMMAND_QUEUE_PORT_MSK_DATA_LENGTH    (BIT_MASK(16) << I3C_COMMAND_QUEUE_PORT_POS_DATA_LENGTH)
#define I3C_COMMAND_QUEUE_PORT_MSK_DMA            (BIT_MASK(1)  << I3C_COMMAND_QUEUE_PORT_POS_DMA)

/* 5.1.8 RESPONSE_QUEUE_PORT register fields */

#define I3C_RESPONSE_QUEUE_PORT_POS_ERR_STATUS                  28 //  31:28
#define I3C_RESPONSE_QUEUE_PORT_POS_TID                         24 //  27:24
#define I3C_RESPONSE_QUEUE_PORT_POS_CCCT                        16 //  23:16
#define I3C_RESPONSE_QUEUE_PORT_POS_DATA_LENGTH_OR_DEV_COUNT    0  //  15:0

#define I3C_RESPONSE_QUEUE_PORT_MSK_ERR_STATUS                  (BIT_MASK(4)  << I3C_RESPONSE_QUEUE_PORT_POS_ERR_STATUS)
#define I3C_RESPONSE_QUEUE_PORT_MSK_TID                         (BIT_MASK(4)  << I3C_RESPONSE_QUEUE_PORT_POS_TID)
#define I3C_RESPONSE_QUEUE_PORT_MSK_CCCT                        (BIT_MASK(8)  << I3C_RESPONSE_QUEUE_PORT_POS_CCCT)
#define I3C_RESPONSE_QUEUE_PORT_MSK_DATA_LENGTH_OR_DEV_COUNT    (BIT_MASK(16) << I3C_RESPONSE_QUEUE_PORT_POS_DATA_LENGTH_OR_DEV_COUNT)

/* 5.1.12 IBI_QUEUE_STATUS register fields */

#define I3C_IBI_QUEUE_STATUS_POS_IBI_STS     28 //31:28
#define I3C_IBI_QUEUE_STATUS_POS_IBI_ID      8  //15:8
#define I3C_IBI_QUEUE_STATUS_POS_DATA_LENGTH 0  //7:0

#define I3C_IBI_QUEUE_STATUS_MSK_IBI_STS        (BIT_MASK(4)  << I3C_IBI_QUEUE_STATUS_POS_IBI_STS)
#define I3C_IBI_QUEUE_STATUS_MSK_IBI_ID         (BIT_MASK(8)  << I3C_IBI_QUEUE_STATUS_POS_IBI_ID)
#define I3C_IBI_QUEUE_STATUS_MSK_DATA_LENGTH    (BIT_MASK(8)  << I3C_IBI_QUEUE_STATUS_POS_DATA_LENGTH)



/* 5.1.13 QUEUE_THLD_CTRL register fields */


#define I3C_QUEUE_THLD_CTRL_POS_IBI_STATUS_THLD     24  //28:24
#define I3C_QUEUE_THLD_CTRL_POS_RESP_BUF_THLD       8   //15:8
#define I3C_QUEUE_THLD_CTRL_POS_CMD_EMPTY_BUF_THLD  0   //7:0

#define I3C_QUEUE_THLD_CTRL_MSK_IBI_STATUS_THLD       (BIT_MASK(5)  << I3C_QUEUE_THLD_CTRL_POS_IBI_STATUS_THLD)
#define I3C_QUEUE_THLD_CTRL_MSK_RESP_BUF_THLD         (BIT_MASK(8)  << I3C_QUEUE_THLD_CTRL_POS_RESP_BUF_THLD)
#define I3C_QUEUE_THLD_CTRL_MSK_CMD_EMPTY_BUF_THLD    (BIT_MASK(8)  << I3C_QUEUE_THLD_CTRL_POS_CMD_EMPTY_BUF_THLD)


/* 5.1.14 DATA_BUFFER_THLD_CTRL register fields */

#define I3C_DATA_BUFFER_THLD_CTRL_POS_RX_START_THLD     24  //  26:24
#define I3C_DATA_BUFFER_THLD_CTRL_POS_TX_START_THLD     16  //  18:16
#define I3C_DATA_BUFFER_THLD_CTRL_POS_RX_BUF_THLD       8   //  10:8
#define I3C_DATA_BUFFER_THLD_CTRL_POS_TX_EMPTY_BUF_THLD 0   //  2:0

#define I3C_DATA_BUFFER_THLD_CTRL_MSK_RX_START_THLD     (BIT_MASK(3) << I3C_DATA_BUFFER_THLD_CTRL_POS_RX_START_THLD)
#define I3C_DATA_BUFFER_THLD_CTRL_MSK_TX_START_THLD     (BIT_MASK(3) << I3C_DATA_BUFFER_THLD_CTRL_POS_TX_START_THLD)
#define I3C_DATA_BUFFER_THLD_CTRL_MSK_RX_BUF_THLD       (BIT_MASK(3) << I3C_DATA_BUFFER_THLD_CTRL_POS_RX_BUF_THLD)
#define I3C_DATA_BUFFER_THLD_CTRL_MSK_TX_EMPTY_BUF_THLD (BIT_MASK(3) << I3C_DATA_BUFFER_THLD_CTRL_POS_TX_EMPTY_BUF_THLD)

/* 5.1.15 IBI_QUEUE_CTRL register fields */

#define I3C_IBI_QUEUE_CTRL_POS_NOTIFY_SIR_REJECTED  3
#define I3C_IBI_QUEUE_CTRL_POS_NOTIFY_MR_REJECTED   1
#define I3C_IBI_QUEUE_CTRL_POS_NOTIFY_HJ_REJECTED   0

#define I3C_IBI_QUEUE_CTRL_MSK_NOTIFY_SIR_REJECTED  (BIT_MASK(1) << I3C_IBI_QUEUE_CTRL_POS_NOTIFY_SIR_REJECTED)
#define I3C_IBI_QUEUE_CTRL_MSK_NOTIFY_MR_REJECTED   (BIT_MASK(1) << I3C_IBI_QUEUE_CTRL_POS_NOTIFY_MR_REJECTED)
#define I3C_IBI_QUEUE_CTRL_MSK_NOTIFY_HJ_REJECTED   (BIT_MASK(1) << I3C_IBI_QUEUE_CTRL_POS_NOTIFY_HJ_REJECTED)


/* 5.1.16 I3C_IBI_MR_P2P_REQ_REJECT register fields */

#define I3C_IBI_MR_P2P_REQ_REJECT_POS_MR_REQ_REJECT 0

#define I3C_IBI_MR_P2P_REQ_REJECT_MSK_MR_REQ_REJECT (BIT_MASK(1) << I3C_IBI_MR_P2P_REQ_REJECT_POS_MR_REQ_REJECT)




/* 5.1.18 RESET_CTRL register fields */

#define I3C_RESET_CTRL_POS_IBI_QUEUE_RST    5
#define I3C_RESET_CTRL_POS_RX_FIFO_RST      4
#define I3C_RESET_CTRL_POS_TX_FIFO_RST      3
#define I3C_RESET_CTRL_POS_RESP_QUEUE_RST   2
#define I3C_RESET_CTRL_POS_CMD_QUEUE_RST    1
#define I3C_RESET_CTRL_POS_SOFT_RST         0

#define I3C_RESET_CTRL_MSK_IBI_QUEUE_RST    (BIT_MASK(1) << I3C_RESET_CTRL_POS_IBI_QUEUE_RST)
#define I3C_RESET_CTRL_MSK_RX_FIFO_RST      (BIT_MASK(1) << I3C_RESET_CTRL_POS_RX_FIFO_RST)
#define I3C_RESET_CTRL_MSK_TX_FIFO_RST      (BIT_MASK(1) << I3C_RESET_CTRL_POS_TX_FIFO_RST)
#define I3C_RESET_CTRL_MSK_RESP_QUEUE_RST   (BIT_MASK(1) << I3C_RESET_CTRL_POS_RESP_QUEUE_RST)
#define I3C_RESET_CTRL_MSK_CMD_QUEUE_RST    (BIT_MASK(1) << I3C_RESET_CTRL_POS_CMD_QUEUE_RST)
#define I3C_RESET_CTRL_MSK_SOFT_RST         (BIT_MASK(1) << I3C_RESET_CTRL_POS_SOFT_RST)
#define I3C_RESET_CTRL_MSK_RESET_ALL        ( I3C_RESET_CTRL_MSK_SOFT_RST       |    \
                                              I3C_RESET_CTRL_MSK_CMD_QUEUE_RST  |    \
                                              I3C_RESET_CTRL_MSK_RESP_QUEUE_RST |    \
                                              I3C_RESET_CTRL_MSK_TX_FIFO_RST    |    \
                                              I3C_RESET_CTRL_MSK_RX_FIFO_RST    |    \
                                              I3C_RESET_CTRL_MSK_IBI_QUEUE_RST )
/* 5.1.19 SLV_EVENT_CTRL register fields */

#define I3C_SLV_EVENT_CTRL_POS_ENTAS3          7
#define I3C_SLV_EVENT_CTRL_POS_ENTAS2          6
#define I3C_SLV_EVENT_CTRL_POS_ENTAS1          5
#define I3C_SLV_EVENT_CTRL_POS_ENTAS0          4
#define I3C_SLV_EVENT_CTRL_POS_HJ_INTR_REQ     3
#define I3C_SLV_EVENT_CTRL_POS_P2P_INTR_REQ    2
#define I3C_SLV_EVENT_CTRL_POS_MST_INTR_REQ    1
#define I3C_SLV_EVENT_CTRL_POS_SLAVE_INTR_REQ  0

#define I3C_SLV_EVENT_CTRL_MSK_ENTAS3          (BIT_MASK(1) << I3C_SLV_EVENT_CTRL_POS_ENTAS3)
#define I3C_SLV_EVENT_CTRL_MSK_ENTAS2          (BIT_MASK(1) << I3C_SLV_EVENT_CTRL_POS_ENTAS2)
#define I3C_SLV_EVENT_CTRL_MSK_ENTAS1          (BIT_MASK(1) << I3C_SLV_EVENT_CTRL_POS_ENTAS1)
#define I3C_SLV_EVENT_CTRL_MSK_ENTAS0          (BIT_MASK(1) << I3C_SLV_EVENT_CTRL_POS_ENTAS0)
#define I3C_SLV_EVENT_CTRL_MSK_HJ_INTR_REQ     (BIT_MASK(1) << I3C_SLV_EVENT_CTRL_POS_HJ_INTR_REQ)
#define I3C_SLV_EVENT_CTRL_MSK_P2P_INTR_REQ    (BIT_MASK(1) << I3C_SLV_EVENT_CTRL_POS_P2P_INTR_REQ)
#define I3C_SLV_EVENT_CTRL_MSK_MST_INTR_REQ    (BIT_MASK(1) << I3C_SLV_EVENT_CTRL_POS_MST_INTR_REQ)
#define I3C_SLV_EVENT_CTRL_MSK_SLAVE_INTR_REQ  (BIT_MASK(1) << I3C_SLV_EVENT_CTRL_POS_SLAVE_INTR_REQ)

/* 5.1.20 INT_STATUS register fields */

#define I3C_INTR_STATUS_POS_DEFSLV_P2PTRGT_STAT     10
#define I3C_INTR_STATUS_POS_TRANSFER_ERR_STAT       9
#define I3C_INTR_STATUS_POS_DYN_ADDR_ASSGN_STAT     8
#define I3C_INTR_STATUS_POS_CMD_RETRY_FAILED_STAT   7
#define I3C_INTR_STATUS_POS_CCC_TABLE_UPDT_STAT     6
#define I3C_INTR_STATUS_POS_TRANSFER_ABORT_STAT     5
#define I3C_INTR_STATUS_POS_RESP_READY_STAT_INTR    4
#define I3C_INTR_STATUS_POS_CMD_QUEUE_READY_STAT    3
#define I3C_INTR_STATUS_POS_IBI_THLD_STAT           2
#define I3C_INTR_STATUS_POS_RX_THLD_STAT            1
#define I3C_INTR_STATUS_POS_TX_THLD_STAT            0

#define I3C_INTR_STATUS_MSK_DEFSLV_P2PTRGT_STAT     (BIT_MASK(1) << I3C_INTR_STATUS_POS_DEFSLV_P2PTRGT_STAT)
#define I3C_INTR_STATUS_MSK_TRANSFER_ERR_STAT       (BIT_MASK(1) << I3C_INTR_STATUS_POS_TRANSFER_ERR_STAT)
#define I3C_INTR_STATUS_MSK_DYN_ADDR_ASSGN_STAT     (BIT_MASK(1) << I3C_INTR_STATUS_POS_DYN_ADDR_ASSGN_STAT)
#define I3C_INTR_STATUS_MSK_CMD_RETRY_FAILED_STAT   (BIT_MASK(1) << I3C_INTR_STATUS_POS_CMD_RETRY_FAILED_STAT)
#define I3C_INTR_STATUS_MSK_CCC_TABLE_UPDT_STAT     (BIT_MASK(1) << I3C_INTR_STATUS_POS_CCC_TABLE_UPDT_STAT)
#define I3C_INTR_STATUS_MSK_TRANSFER_ABORT_STAT     (BIT_MASK(1) << I3C_INTR_STATUS_POS_TRANSFER_ABORT_STAT)
#define I3C_INTR_STATUS_MSK_RESP_READY_STAT_INTR    (BIT_MASK(1) << I3C_INTR_STATUS_POS_RESP_READY_STAT_INTR)
#define I3C_INTR_STATUS_MSK_CMD_QUEUE_READY_STAT    (BIT_MASK(1) << I3C_INTR_STATUS_POS_CMD_QUEUE_READY_STAT)
#define I3C_INTR_STATUS_MSK_IBI_THLD_STAT           (BIT_MASK(1) << I3C_INTR_STATUS_POS_IBI_THLD_STAT)
#define I3C_INTR_STATUS_MSK_RX_THLD_STAT            (BIT_MASK(1) << I3C_INTR_STATUS_POS_RX_THLD_STAT)
#define I3C_INTR_STATUS_MSK_TX_THLD_STAT            (BIT_MASK(1) << I3C_INTR_STATUS_POS_TX_THLD_STAT)

// The following mask clears all event interrupts. Other bits (status interrupts) are auto-cleared
#define I3C_INTR_STATUS_MSK_EVENTS                  (I3C_INTR_STATUS_MSK_DEFSLV_P2PTRGT_STAT    |   \
                                                     I3C_INTR_STATUS_MSK_TRANSFER_ERR_STAT      |   \
                                                     I3C_INTR_STATUS_MSK_DYN_ADDR_ASSGN_STAT    |   \
                                                     I3C_INTR_STATUS_MSK_CMD_RETRY_FAILED_STAT  |   \
                                                     I3C_INTR_STATUS_MSK_CCC_TABLE_UPDT_STAT    |   \
                                                     I3C_INTR_STATUS_MSK_TRANSFER_ABORT_STAT)

/* 5.1.21 INT_STATUS_EN register fields */

#define I3C_INTR_STATUS_EN_POS_DEFSLV_P2PTRGT_STAT_EN    10
#define I3C_INTR_STATUS_EN_POS_TRANSFER_ERR_STAT_EN      9
#define I3C_INTR_STATUS_EN_POS_DYN_ADDR_ASSGN_STAT_EN    8
#define I3C_INTR_STATUS_EN_POS_CMD_RETRY_FAILED_STAT_EN  7
#define I3C_INTR_STATUS_EN_POS_CCC_TABLE_UPDT_STAT_EN    6
#define I3C_INTR_STATUS_EN_POS_TRANSFER_ABORT_STAT_EN    5
#define I3C_INTR_STATUS_EN_POS_RESP_READY_STAT_INTR_EN   4
#define I3C_INTR_STATUS_EN_POS_CMD_QUEUE_READY_STAT_EN   3
#define I3C_INTR_STATUS_EN_POS_IBI_THLD_STAT_EN          2
#define I3C_INTR_STATUS_EN_POS_RX_THLD_STAT_EN           1
#define I3C_INTR_STATUS_EN_POS_TX_THLD_STAT_EN           0

#define I3C_INTR_STATUS_EN_MSK_DEFSLV_P2PTRGT_STAT_EN    (BIT_MASK(1) << I3C_INTR_STATUS_EN_POS_DEFSLV_P2PTRGT_STAT_EN)
#define I3C_INTR_STATUS_EN_MSK_TRANSFER_ERR_STAT_EN      (BIT_MASK(1) << I3C_INTR_STATUS_EN_POS_TRANSFER_ERR_STAT_EN)
#define I3C_INTR_STATUS_EN_MSK_DYN_ADDR_ASSGN_STAT_EN    (BIT_MASK(1) << I3C_INTR_STATUS_EN_POS_DYN_ADDR_ASSGN_STAT_EN)
#define I3C_INTR_STATUS_EN_MSK_CMD_RETRY_FAILED_STAT_EN  (BIT_MASK(1) << I3C_INTR_STATUS_EN_POS_CMD_RETRY_FAILED_STAT_EN)
#define I3C_INTR_STATUS_EN_MSK_CCC_TABLE_UPDT_STAT_EN    (BIT_MASK(1) << I3C_INTR_STATUS_EN_POS_CCC_TABLE_UPDT_STAT_EN)
#define I3C_INTR_STATUS_EN_MSK_TRANSFER_ABORT_STAT_EN    (BIT_MASK(1) << I3C_INTR_STATUS_EN_POS_TRANSFER_ABORT_STAT_EN)
#define I3C_INTR_STATUS_EN_MSK_RESP_READY_STAT_INTR_EN   (BIT_MASK(1) << I3C_INTR_STATUS_EN_POS_RESP_READY_STAT_INTR_EN)
#define I3C_INTR_STATUS_EN_MSK_CMD_QUEUE_READY_STAT_EN   (BIT_MASK(1) << I3C_INTR_STATUS_EN_POS_CMD_QUEUE_READY_STAT_EN)
#define I3C_INTR_STATUS_EN_MSK_IBI_THLD_STAT_EN          (BIT_MASK(1) << I3C_INTR_STATUS_EN_POS_IBI_THLD_STAT_EN)
#define I3C_INTR_STATUS_EN_MSK_RX_THLD_STAT_EN           (BIT_MASK(1) << I3C_INTR_STATUS_EN_POS_RX_THLD_STAT_EN)
#define I3C_INTR_STATUS_EN_MSK_TX_THLD_STAT_EN           (BIT_MASK(1) << I3C_INTR_STATUS_EN_POS_TX_THLD_STAT_EN)

#define I3C_INTR_STATUS_EN_MSK_ALL                       (BIT_MASK(11))


/* 5.1.22 INTR_SIGNAL_EN register fields */

#define I3C_INTR_SIGNAL_EN_POS_DEFSLV_P2PTRGT_SIGNAL_EN    10
#define I3C_INTR_SIGNAL_EN_POS_TRANSFER_ERR_SIGNAL_EN      9
#define I3C_INTR_SIGNAL_EN_POS_DYN_ADDR_ASSGN_SIGNAL_EN    8
#define I3C_INTR_SIGNAL_EN_POS_CMD_RETRY_FAILED_SIGNAL_EN  7
#define I3C_INTR_SIGNAL_EN_POS_CCC_TABLE_UPDT_SIGNAL_EN    6
#define I3C_INTR_SIGNAL_EN_POS_TRANSFER_ABORT_SIGNAL_EN    5
#define I3C_INTR_SIGNAL_EN_POS_RESP_READY_SIGNAL_EN        4
#define I3C_INTR_SIGNAL_EN_POS_CMD_QUEUE_READY_SIGNAL_EN   3
#define I3C_INTR_SIGNAL_EN_POS_IBI_THLD_SIGNAL_EN          2
#define I3C_INTR_SIGNAL_EN_POS_RX_THLD_SIGNAL_EN           1
#define I3C_INTR_SIGNAL_EN_POS_TX_THLD_SIGNAL_EN           0

#define I3C_INTR_SIGNAL_EN_MSK_DEFSLV_P2PTRGT_SIGNAL_EN    (BIT_MASK(1) << I3C_INTR_SIGNAL_EN_POS_DEFSLV_P2PTRGT_SIGNAL_EN)
#define I3C_INTR_SIGNAL_EN_MSK_TRANSFER_ERR_SIGNAL_EN      (BIT_MASK(1) << I3C_INTR_SIGNAL_EN_POS_TRANSFER_ERR_SIGNAL_EN)
#define I3C_INTR_SIGNAL_EN_MSK_DYN_ADDR_ASSGN_SIGNAL_EN    (BIT_MASK(1) << I3C_INTR_SIGNAL_EN_POS_DYN_ADDR_ASSGN_SIGNAL_EN)
#define I3C_INTR_SIGNAL_EN_MSK_CMD_RETRY_FAILED_SIGNAL_EN  (BIT_MASK(1) << I3C_INTR_SIGNAL_EN_POS_CMD_RETRY_FAILED_SIGNAL_EN)
#define I3C_INTR_SIGNAL_EN_MSK_CCC_TABLE_UPDT_SIGNAL_EN    (BIT_MASK(1) << I3C_INTR_SIGNAL_EN_POS_CCC_TABLE_UPDT_SIGNAL_EN)
#define I3C_INTR_SIGNAL_EN_MSK_TRANSFER_ABORT_SIGNAL_EN    (BIT_MASK(1) << I3C_INTR_SIGNAL_EN_POS_TRANSFER_ABORT_SIGNAL_EN)
#define I3C_INTR_SIGNAL_EN_MSK_RESP_READY_SIGNAL_EN        (BIT_MASK(1) << I3C_INTR_SIGNAL_EN_POS_RESP_READY_SIGNAL_EN)
#define I3C_INTR_SIGNAL_EN_MSK_CMD_QUEUE_READY_SIGNAL_EN   (BIT_MASK(1) << I3C_INTR_SIGNAL_EN_POS_CMD_QUEUE_READY_SIGNAL_EN)
#define I3C_INTR_SIGNAL_EN_MSK_IBI_THLD_SIGNAL_EN          (BIT_MASK(1) << I3C_INTR_SIGNAL_EN_POS_IBI_THLD_SIGNAL_EN)
#define I3C_INTR_SIGNAL_EN_MSK_RX_THLD_SIGNAL_EN           (BIT_MASK(1) << I3C_INTR_SIGNAL_EN_POS_RX_THLD_SIGNAL_EN)
#define I3C_INTR_SIGNAL_EN_MSK_TX_THLD_SIGNAL_EN           (BIT_MASK(1) << I3C_INTR_SIGNAL_EN_POS_TX_THLD_SIGNAL_EN)


/* 5.1.23 INTR_FORCE register fields */

#define I3C_INTR_FORCE_POS_DEFSLV_P2PTRGT_FORCE_EN     10
#define I3C_INTR_FORCE_POS_TRANSFER_ERR_FORCE_EN       9
#define I3C_INTR_FORCE_POS_DYN_ADDR_ASSGN_FORCE_EN     8
#define I3C_INTR_FORCE_POS_CMD_RETRY_FAILED_FORCE_EN   7
#define I3C_INTR_FORCE_POS_CCC_TABLE_UPDT_FORCE_EN     6
#define I3C_INTR_FORCE_POS_TRANSFER_ABORT_FORCE_EN     5
#define I3C_INTR_FORCE_POS_RESP_READY_FORCE_EN         4
#define I3C_INTR_FORCE_POS_CMD_QUEUE_READY_FORCE_EN    3
#define I3C_INTR_FORCE_POS_IBI_THLD_FORCE_EN           2
#define I3C_INTR_FORCE_POS_RX_THLD_FORCE_EN            1
#define I3C_INTR_FORCE_POS_TX_THLD_FORCE_EN            0

#define I3C_INTR_FORCE_MSK_DEFSLV_P2PTRGT_FORCE_EN      (BIT_MASK(1) << I3C_INTR_FORCE_POS_DEFSLV_P2PTRGT_FORCE_EN)
#define I3C_INTR_FORCE_MSK_TRANSFER_ERR_FORCE_EN        (BIT_MASK(1) << I3C_INTR_FORCE_POS_TRANSFER_ERR_FORCE_EN)
#define I3C_INTR_FORCE_MSK_DYN_ADDR_ASSGN_FORCE_EN      (BIT_MASK(1) << I3C_INTR_FORCE_POS_DYN_ADDR_ASSGN_FORCE_EN)
#define I3C_INTR_FORCE_MSK_CMD_RETRY_FAILED_FORCE_EN    (BIT_MASK(1) << I3C_INTR_FORCE_POS_CMD_RETRY_FAILED_FORCE_EN)
#define I3C_INTR_FORCE_MSK_CCC_TABLE_UPDT_FORCE_EN      (BIT_MASK(1) << I3C_INTR_FORCE_POS_CCC_TABLE_UPDT_FORCE_EN)
#define I3C_INTR_FORCE_MSK_TRANSFER_ABORT_FORCE_EN      (BIT_MASK(1) << I3C_INTR_FORCE_POS_TRANSFER_ABORT_FORCE_EN)
#define I3C_INTR_FORCE_MSK_RESP_READY_FORCE_EN          (BIT_MASK(1) << I3C_INTR_FORCE_POS_RESP_READY_FORCE_EN)
#define I3C_INTR_FORCE_MSK_CMD_QUEUE_READY_FORCE_EN     (BIT_MASK(1) << I3C_INTR_FORCE_POS_CMD_QUEUE_READY_FORCE_EN)
#define I3C_INTR_FORCE_MSK_IBI_THLD_FORCE_EN            (BIT_MASK(1) << I3C_INTR_FORCE_POS_IBI_THLD_FORCE_EN)
#define I3C_INTR_FORCE_MSK_RX_THLD_FORCE_EN             (BIT_MASK(1) << I3C_INTR_FORCE_POS_RX_THLD_FORCE_EN)
#define I3C_INTR_FORCE_MSK_TX_THLD_FORCE_EN             (BIT_MASK(1) << I3C_INTR_FORCE_POS_TX_THLD_FORCE_EN)

/* 5.1.24 QUEUE_STATUS_LEVEL register fields */

#define I3C_QUEUE_STATUS_LEVEL_POS_IBI_STATUS_CNT      24 //28:24
#define I3C_QUEUE_STATUS_LEVEL_POS_IBI_BUF_BLR         16 //23:16
#define I3C_QUEUE_STATUS_LEVEL_POS_RESP_BUF_BLR        8  //15:8
#define I3C_QUEUE_STATUS_LEVEL_POS_CMD_QUEUE_EMPTY_LOC 0  //7:0

#define I3C_QUEUE_STATUS_LEVEL_MSK_IBI_STATUS_CNT       (BIT_MASK(4) << I3C_QUEUE_STATUS_LEVEL_POS_IBI_STATUS_CNT)
#define I3C_QUEUE_STATUS_LEVEL_MSK_IBI_BUF_BLR          (BIT_MASK(8) << I3C_QUEUE_STATUS_LEVEL_POS_IBI_BUF_BLR)
#define I3C_QUEUE_STATUS_LEVEL_MSK_RESP_BUF_BLR         (BIT_MASK(8) << I3C_QUEUE_STATUS_LEVEL_POS_RESP_BUF_BLR)
#define I3C_QUEUE_STATUS_LEVEL_MSK_CMD_QUEUE_EMPTY_LOC  (BIT_MASK(8) << I3C_QUEUE_STATUS_LEVEL_POS_CMD_QUEUE_EMPTY_LOC)


/* 5.1.25 DATA_BUFFER_STATUS_LEVEL register fields */

#define I3C_DATA_BUFFER_STATUS_LEVEL_POS_RX_BUF_BLR         8 //15:8
#define I3C_DATA_BUFFER_STATUS_LEVEL_POS_TX_BUF_EMPTY_LOC   0 //7:0

#define I3C_DATA_BUFFER_STATUS_LEVEL_MSK_RX_BUF_BLR         (BIT_MASK(8) << I3C_DATA_BUFFER_STATUS_LEVEL_POS_RX_BUF_BLR)
#define I3C_DATA_BUFFER_STATUS_LEVEL_MSK_TX_BUF_EMPTY_LOC   (BIT_MASK(8) << I3C_DATA_BUFFER_STATUS_LEVEL_POS_TX_BUF_EMPTY_LOC)

/* 5.1.26 PRESENT_STATE register fields */

#define I3C_PRESENT_STATE_POS_CMD_TID                 24 //27:24
#define I3C_PRESENT_STATE_POS_CM_TFR_ST_STATUS        16 //21:16
#define I3C_PRESENT_STATE_POS_CM_TFR_STATUS           8  //13:8
#define I3C_PRESENT_STATE_POS_CURRENT_MASTER          2
#define I3C_PRESENT_STATE_POS_SDA_LINE_SIGNAL_LEVEL   1
#define I3C_PRESENT_STATE_POS_SCL_LINE_SIGNAL_LEVEL   0

#define I3C_PRESENT_STATE_MSK_CMD_TID                 (BIT_MASK(4) << I3C_PRESENT_STATE_POS_CMD_TID)
#define I3C_PRESENT_STATE_MSK_CM_TFR_ST_STATUS        (BIT_MASK(6) << I3C_PRESENT_STATE_POS_CM_TFR_ST_STATUS)
#define I3C_PRESENT_STATE_MSK_CM_TFR_STATUS           (BIT_MASK(6) << I3C_PRESENT_STATE_POS_CM_TFR_STATUS)
#define I3C_PRESENT_STATE_MSK_CURRENT_MASTER          (BIT_MASK(1) << I3C_PRESENT_STATE_POS_CURRENT_MASTER)
#define I3C_PRESENT_STATE_MSK_SDA_LINE_SIGNAL_LEVEL   (BIT_MASK(1) << I3C_PRESENT_STATE_POS_SDA_LINE_SIGNAL_LEVEL)
#define I3C_PRESENT_STATE_MSK_SCL_LINE_SIGNAL_LEVEL   (BIT_MASK(1) << I3C_PRESENT_STATE_POS_SCL_LINE_SIGNAL_LEVEL)


/* 5.1.27 CCC_DEVICE_STATUS register fields */

#define I3C_CCC_DEVICE_STATUS_POS_USER_DEFINED_STATUS 8 //15:8
#define I3C_CCC_DEVICE_STATUS_POS_ACTIVITY_MODE       6 //7:6
#define I3C_CCC_DEVICE_STATUS_POS_WRITE_ERR           5
#define I3C_CCC_DEVICE_STATUS_POS_P2P_REQ             4
#define I3C_CCC_DEVICE_STATUS_POS_PENDING_INTR        0 //3:0

#define I3C_CCC_DEVICE_STATUS_MSK_USER_DEFINED_STATUS   (BIT_MASK(8) << I3C_CCC_DEVICE_STATUS_POS_USER_DEFINED_STATUS)
#define I3C_CCC_DEVICE_STATUS_MSK_ACTIVITY_MODE         (BIT_MASK(2) << I3C_CCC_DEVICE_STATUS_POS_ACTIVITY_MODE)
#define I3C_CCC_DEVICE_STATUS_MSK_WRITE_ERR             (BIT_MASK(1) << I3C_CCC_DEVICE_STATUS_POS_WRITE_ERR)
#define I3C_CCC_DEVICE_STATUS_MSK_P2P_REQ               (BIT_MASK(1) << I3C_CCC_DEVICE_STATUS_POS_P2P_REQ)
#define I3C_CCC_DEVICE_STATUS_MSK_PENDING_INTR          (BIT_MASK(4) << I3C_CCC_DEVICE_STATUS_POS_PENDING_INTR)

/* 5.1.28 DEVICE_ADDR_TABLE_POINTER register fields */

#define I3C_DEVICE_ADDR_TABLE_POINTER_POS_DEV_ADDR_TABLE_DEPTH        16 //31:16
#define I3C_DEVICE_ADDR_TABLE_POINTER_POS_P_DEV_ADDR_TABLE_START_ADDR 0  //15:0

#define I3C_DEVICE_ADDR_TABLE_POINTER_MSK_DEV_ADDR_TABLE_DEPTH        (BIT_MASK(16) << I3C_DEVICE_ADDR_TABLE_POINTER_POS_DEV_ADDR_TABLE_DEPTH)
#define I3C_DEVICE_ADDR_TABLE_POINTER_MSK_P_DEV_ADDR_TABLE_START_ADDR (BIT_MASK(16) << I3C_DEVICE_ADDR_TABLE_POINTER_POS_P_DEV_ADDR_TABLE_START_ADDR)

/* 5.1.29 DEV_CHAR_TABLE_POINTER register fields    */

#define I3C_DEV_CHAR_TABLE_POINTER_POS_PRESENT_DEV_CHAR_TABLE_INDX 19 // (IC_RAM_DAA_ABW + 16):19
#define I3C_DEV_CHAR_TABLE_POINTER_POS_DEV_CHAR_TABLE_DEPTH        12 // 18:12
#define I3C_DEV_CHAR_TABLE_POINTER_POS_P_DEV_CHAR_TABLE_START_ADDR 0  // 11:0


#define I3C_DEV_CHAR_TABLE_POINTER_MSK_PRESENT_DEV_CHAR_TABLE_INDX  (BIT_MASK(IC_RAM_DAA_ABW - 2) << I3C_DEV_CHAR_TABLE_POINTER_POS_PRESENT_DEV_CHAR_TABLE_INDX)
#define I3C_DEV_CHAR_TABLE_POINTER_MSK_DEV_CHAR_TABLE_DEPTH         (BIT_MASK(7) << I3C_DEV_CHAR_TABLE_POINTER_POS_DEV_CHAR_TABLE_DEPTH)
#define I3C_DEV_CHAR_TABLE_POINTER_MSK_P_DEV_CHAR_TABLE_START_ADDR  (BIT_MASK(12) << I3C_DEV_CHAR_TABLE_POINTER_POS_P_DEV_CHAR_TABLE_START_ADDR)


/* 5.1.30 CCC_PARAM_TABLE_POINTER register fields */

#define I3C_CCC_PARAM_TABLE_POINTER_POS_P_CCC_PARAM_START_ADDR 0 // 15:0
#define I3C_CCC_PARAM_TABLE_POINTER_MSK_P_CCC_PARAM_START_ADDR (BIT_MASK(16) << I3C_CCC_PARAM_TABLE_POINTER_POS_P_CCC_PARAM_START_ADDR)


/* 5.1.31 VENDOR_SPECIFIC_REG_POINTER register fields */

#define I3C_VENDOR_SPECIFIC_REG_POINTER_POS_P_VENDOR_REG_START_ADDR 0 // 15:0
#define I3C_VENDOR_SPECIFIC_REG_POINTER_MSK_P_VENDOR_REG_START_ADDR (BIT_MASK(16) << I3C_VENDOR_SPECIFIC_REG_POINTER_POS_P_VENDOR_REG_START_ADDR)


/* 5.1.32 DEVICE_CTRL_EXTENDED register fields */

#define I3C_DEVICE_CTRL_EXTENDED_POS_REQMST_ACK_CTRL    3
#define I3C_DEVICE_CTRL_EXTENDED_POS_DEV_INTERFACE_TYPE 2
#define I3C_DEVICE_CTRL_EXTENDED_POS_DEV_OPERATION_MODE 0 //1:0

#define I3C_DEVICE_CTRL_EXTENDED_MSK_REQMST_ACK_CTRL    (BIT_MASK(1) << I3C_DEVICE_CTRL_EXTENDED_POS_REQMST_ACK_CTRL)
#define I3C_DEVICE_CTRL_EXTENDED_MSK_DEV_INTERFACE_TYPE (BIT_MASK(1) << I3C_DEVICE_CTRL_EXTENDED_POS_DEV_INTERFACE_TYPE)
#define I3C_DEVICE_CTRL_EXTENDED_MSK_DEV_OPERATION_MODE (BIT_MASK(2) << I3C_DEVICE_CTRL_EXTENDED_POS_DEV_OPERATION_MODE)



/* 5.1.33 SCL_HCNT_TIMING register fields */


#define I3C_SCL_HCNT_TIMING_POS_I3C_PP_HCNT 16 //23:16
#define I3C_SCL_HCNT_TIMING_POS_I3C_OD_HCNT 8  //15:8
#define I3C_SCL_HCNT_TIMING_POS_I2C_OD_HCNT 0  //7:0


#define I3C_SCL_HCNT_TIMING_MSK_I3C_PP_HCNT (BIT_MASK(8) << I3C_SCL_HCNT_TIMING_POS_I3C_PP_HCNT)
#define I3C_SCL_HCNT_TIMING_MSK_I3C_OD_HCNT (BIT_MASK(8) << I3C_SCL_HCNT_TIMING_POS_I3C_OD_HCNT)
#define I3C_SCL_HCNT_TIMING_MSK_I2C_OD_HCNT (BIT_MASK(8) << I3C_SCL_HCNT_TIMING_POS_I2C_OD_HCNT)



/* 5.1.34 SCL_LCNT_TIMING register fields */

#define I3C_SCL_LCNT_TIMING_POS_I3C_PP_LCNT 24 //31:24
#define I3C_SCL_LCNT_TIMING_POS_I3C_OD_LCNT 16 //23:16
#define I3C_SCL_LCNT_TIMING_POS_I2C_OD_LCNT 0  //15:0

#define I3C_SCL_LCNT_TIMING_MSK_I3C_PP_LCNT     (BIT_MASK(8) << I3C_SCL_LCNT_TIMING_POS_I3C_PP_LCNT)
#define I3C_SCL_LCNT_TIMING_MSK_I3C_OD_LCNT     (BIT_MASK(8) << I3C_SCL_LCNT_TIMING_POS_I3C_OD_LCNT)
#define I3C_SCL_LCNT_TIMING_MSK_I2C_OD_LCNT     (BIT_MASK(16) << I3C_SCL_LCNT_TIMING_POS_I2C_OD_LCNT)

/* 5.1.35 SCL_EXT_LCNT_TIMING register fields */

#define I3C_SCL_EXT_LCNT_TIMING_POS_I3C_EXT_LCNT_4  24 //   31:24
#define I3C_SCL_EXT_LCNT_TIMING_POS_I3C_EXT_LCNT_3  16 //   23:16
#define I3C_SCL_EXT_LCNT_TIMING_POS_I3C_EXT_LCNT_2  8  //   15:8
#define I3C_SCL_EXT_LCNT_TIMING_POS_I3C_EXT_LCNT_1  0  //   7:0

#define I3C_SCL_EXT_LCNT_TIMING_MSK_I3C_EXT_LCNT_4  (BIT_MASK(8) << I3C_SCL_EXT_LCNT_TIMING_POS_I3C_EXT_LCNT_4)
#define I3C_SCL_EXT_LCNT_TIMING_MSK_I3C_EXT_LCNT_3  (BIT_MASK(8) << I3C_SCL_EXT_LCNT_TIMING_POS_I3C_EXT_LCNT_3)
#define I3C_SCL_EXT_LCNT_TIMING_MSK_I3C_EXT_LCNT_2  (BIT_MASK(8) << I3C_SCL_EXT_LCNT_TIMING_POS_I3C_EXT_LCNT_2)
#define I3C_SCL_EXT_LCNT_TIMING_MSK_I3C_EXT_LCNT_1  (BIT_MASK(8) << I3C_SCL_EXT_LCNT_TIMING_POS_I3C_EXT_LCNT_1)

/* 5.1.36 BUS_FREE_TIMING register fields */

#define I3C_BUS_FREE_TIMING_POS_I3C_IBI_FREE    16 //IC_FREE_CNT_REG_WD + 15:16
#define I3C_BUS_FREE_TIMING_POS_I3C_MST_FREE    0  //7:0

#define I3C_BUS_FREE_TIMING_MSK_I3C_IBI_FREE    (BIT_MASK(IC_FREE_CNT_REG_WD) << I3C_BUS_FREE_TIMING_POS_I3C_IBI_FREE)
#define I3C_BUS_FREE_TIMING_MSK_I3C_MST_FREE    (BIT_MASK(8) << I3C_BUS_FREE_TIMING_POS_I3C_MST_FREE)

/* 5.1.37 BUS_IDLE_TIMING register fields */

#define I3C_BUS_IDLE_TIMING_POS_BUS_IDLE        0 // IC_IDL_CNT_REG_WD - 1:0
#define I3C_BUS_IDLE_TIMING_MSK_BUS_IDLE        (BIT_MASK(IC_IDL_CNT_REG_WD) << I3C_BUS_IDLE_TIMING_POS_BUS_IDLE)

/* 5.1.41 EXTENDED_CAPABILITY register fields */

#define I3C_EXTENDED_CAPABILITY_POS_CLK_PERIOD        6 //11:6
#define I3C_EXTENDED_CAPABILITY_POS_OPERATION_MODE    4 //5:4
#define I3C_EXTENDED_CAPABILITY_POS_APP_IF_DATA_WIDTH 2 //3:2
#define I3C_EXTENDED_CAPABILITY_POS_APP_IF_MODE       0 //1:0

#define I3C_EXTENDED_CAPABILITY_MSK_CLK_PERIOD          (BIT_MASK(6) << I3C_EXTENDED_CAPABILITY_POS_CLK_PERIOD)
#define I3C_EXTENDED_CAPABILITY_MSK_OPERATION_MODE      (BIT_MASK(2) << I3C_EXTENDED_CAPABILITY_POS_OPERATION_MODE)
#define I3C_EXTENDED_CAPABILITY_MSK_APP_IF_DATA_WIDTH   (BIT_MASK(2) << I3C_EXTENDED_CAPABILITY_POS_APP_IF_DATA_WIDTH)
#define I3C_EXTENDED_CAPABILITY_MSK_APP_IF_MODE         (BIT_MASK(2) << I3C_EXTENDED_CAPABILITY_POS_APP_IF_MODE)

/* 5.1.47 DEV_ADDR_TABLE_LOCx register fields */

#define I3C_DEV_ADDR_TABLE_LOC_POS_LEGACY_I2C_DEVICE   31
#define I3C_DEV_ADDR_TABLE_LOC_POS_DEV_NACK_RETRY_CNT  29 //30:29
#define I3C_DEV_ADDR_TABLE_LOC_POS_DEV_DYNAMIC_ADDR    16 //23:16
#define I3C_DEV_ADDR_TABLE_LOC_POS_DEV_STATIC_ADDR     0  //6:0

#define I3C_DEV_ADDR_TABLE_LOC_MSK_LEGACY_I2C_DEVICE    (BIT_MASK(1) << I3C_DEV_ADDR_TABLE_LOC_POS_LEGACY_I2C_DEVICE)
#define I3C_DEV_ADDR_TABLE_LOC_MSK_DEV_NACK_RETRY_CNT   (BIT_MASK(2) << I3C_DEV_ADDR_TABLE_LOC_POS_DEV_NACK_RETRY_CNT)
#define I3C_DEV_ADDR_TABLE_LOC_MSK_DEV_DYNAMIC_ADDR     (BIT_MASK(8) << I3C_DEV_ADDR_TABLE_LOC_POS_DEV_DYNAMIC_ADDR)
#define I3C_DEV_ADDR_TABLE_LOC_MSK_DEV_STATIC_ADDR      (BIT_MASK(7) << I3C_DEV_ADDR_TABLE_LOC_POS_DEV_STATIC_ADDR)







/* I3C_RESPONSE_QUEUE_PORT - ERR_STATUS field values */
#define I3C_RESPONSE_QUEUE_PORT_ERR_STATUS_NO_ERROR                         0x0 // No Error: This bit indicates that the transfer is successfully completed and there is no error.
#define I3C_RESPONSE_QUEUE_PORT_ERR_STATUS_CRC_ERROR                        0x1 // CRC Error: This bit indicates that CRC error has occured in HDR-DDR receive transfer.
#define I3C_RESPONSE_QUEUE_PORT_ERR_STATUS_PARITY_ERROR                     0x2 // Parity Error: This bit indicates that Parity error has occured in SDR (Slave mode) and HDR receive transfers.
#define I3C_RESPONSE_QUEUE_PORT_ERR_STATUS_FRAME_ERROR                      0x3 // Frame Error: This bit indicates that Framing error has occured in HDR-DDR receive transfers.
#define I3C_RESPONSE_QUEUE_PORT_ERR_STATUS_IBA_NACK_ERROR                   0x4 // IBA Nack Error: This bit indicates that NACK is received for the I3C Broadcast Write/Read address.
#define I3C_RESPONSE_QUEUE_PORT_ERR_STATUS_SLAVE_ADDR_NACK_ERROR            0x5 // Slave Address Nack Error: This bit indicates that NACK is received for the Slave address of the transfers, or if the Slave NACKs for dynamic address assignment during ENTDAA process after the number of retry count is over.
#define I3C_RESPONSE_QUEUE_PORT_ERR_STATUS_BUFFER_OVERFLOW                  0x6 // Receive Buffer Overflow/Transmit Buffer Underflow: This bit indicates that either Rx-FIFO Overflow or Tx-FIFO Underflow happened during the middle of HDR transfers. In SDR transfers, this bit is not applicable because the Master issues clock stalling in such situations.
#define I3C_RESPONSE_QUEUE_PORT_ERR_STATUS_TX_ABORTED                       0x8 // Transfer Aborted: This bit indicates that the transfer is aborted as indicated in the Device Control register.
#define I3C_RESPONSE_QUEUE_PORT_ERR_STATUS_I2C_SLAVE_WRITE_DATA_NACK_ERROR  0x9 // I2C Slave Write Data NACK Error: This bit indicates that NACK is received for the I2C Write Data transfer.


/* COMMAND_QUEUE_PORT_ADDR_ASSGN_CMD - CMD_ATTR field values */

#define I3C_R_CMD_ATTR_TRANSFER_CMD   0x0  //(TRANSFER_CMD): Transfer Command
#define I3C_R_CMD_ATTR_TRANSFER_ARG   0x1  //(TRANSFER_ARG): Transfer Argument
#define I3C_R_CMD_ATTR_SHORT_DATA_ARG 0x2  //(SHORT_DATA_ARG): Short Data Argument
#define I3C_R_CMD_ATTR_ADDR_ASSGN_CMD 0x3  //(ADDR_ASSGN_CMD): Address Assignment Command


/* PRESENT_STATE - CM_TFR_STATUS field values */

#define I3C_R_CM_TFR_STATUS_IDLE        0x0 // 6'h0: IDLE (Controller is in Idle state, waiting for commands from application or Slave initated In-band Interrupt)
#define I3C_R_CM_TFR_STATUS_B_CCC_W     0x1 // 6'h1: Broadcast CCC Write Transfer.
#define I3C_R_CM_TFR_STATUS_D_CCC_W     0x2 // 6'h2: Directed CCC Write Transfer.
#define I3C_R_CM_TFR_STATUS_D_CCC_R     0x3 // 6'h3: Directed CCC Read Transfer.
#define I3C_R_CM_TFR_STATUS_ENTDAA      0x4 // 6'h4: ENTDAA Address Assignment Transfer.
#define I3C_R_CM_TFR_STATUS_SETDASA     0x5 // 6'h5: SETDASA Address Assignment Transfer.
#define I3C_R_CM_TFR_STATUS_I3C_SDR_W   0x6 // 6'h6: Private I3C SDR Write Transfer.
#define I3C_R_CM_TFR_STATUS_I3C_SDR_R   0x7 // 6'h7: Private I3C SDR Read Transfer.
#define I3C_R_CM_TFR_STATUS_I2C_SDR_W   0x8 // 6'h8: Private I2C SDR Write Transfer.
#define I3C_R_CM_TFR_STATUS_I2C_SDR_R   0x9 // 6'h9: Private I2C SDR Read Transfer.
#define I3C_R_CM_TFR_STATUS_HDR_TS_W    0xA // 6'hA: Private HDR Ternary Symbol (TS) Write Transfer.
#define I3C_R_CM_TFR_STATUS_HDR_TS_R    0xB // 6'hB: Private HDR Ternary Symbol (TS) Read Transfer.
#define I3C_R_CM_TFR_STATUS_HDR_DDR_W   0xC // 6'hC: Private HDR Double-Data Rate (DDR) Write Transfer.
#define I3C_R_CM_TFR_STATUS_HDR_DDR_R   0xD // 6'hD: Private HDR Double-Data Rate (DDR) Read Transfer.
#define I3C_R_CM_TFR_STATUS_IBI         0xE // 6'hE: Servicing In-Band Interrupt Transfer.
#define I3C_R_CM_TFR_STATUS_HALT        0xF // 6'hF: Halt state (Controller is in Halt State, waiting for the application to resume through DEVICE_CTRL Register)


/* PRESENT_STATE - CM_TFR_ST_STATUS field values */

// Current Master Transfer State Status. Indicates the state of current transfer currently executing by
// the DWC_mipi_i3c controller.
#define I3C_R_CM_TFR_ST_STATUS_IDLE         0x00 // 6'h0: IDLE (Controller is Idle state, waiting for commands from application or Slave initated In-band Interrupt)
#define I3C_R_CM_TFR_ST_STATUS_START        0x01 // 6'h1: START Generation State.
#define I3C_R_CM_TFR_ST_STATUS_RESTART      0x02 // 6'h2: RESTART Generation State.
#define I3C_R_CM_TFR_ST_STATUS_STOP         0x03 // 6'h3: STOP Generation State.
#define I3C_R_CM_TFR_ST_STATUS_START_HOLD   0x04 // 6'h4: START Hold Generation for the Slave Initiated START State.
#define I3C_R_CM_TFR_ST_STATUS_B_ADDR_W     0x05 // 6'h5: Broadcast Write Address Header(7'h7E,W) Generation State.
#define I3C_R_CM_TFR_ST_STATUS_B_ADDR_R     0x06 // 6'h6: Broadcast Read Address Header(7'h7E,R) Generation State.
#define I3C_R_CM_TFR_ST_STATUS_DAA          0x07 // 6'h7: Dynamic Address Assignment State.
#define I3C_R_CM_TFR_ST_STATUS_SLAVE_ADDR   0x08 // 6'h8: Slave Address Generation State.
#define I3C_R_CM_TFR_ST_STATUS_CCC_BYTE     0x0B // 6'hB: CCC Byte Generation State.
#define I3C_R_CM_TFR_ST_STATUS_HDR_CMD      0x0C // 6'hC: HDR Command Generation State.
#define I3C_R_CM_TFR_ST_STATUS_WR           0x0D // 6'hD: Write Data Transfer State.
#define I3C_R_CM_TFR_ST_STATUS_RD           0x0E // 6'hE: Read Data Transfer State.
#define I3C_R_CM_TFR_ST_STATUS_IBI          0x0F // 6'hF: In-Band Interrupt(SIR) Read Data State.
#define I3C_R_CM_TFR_ST_STATUS_CRC          0x10 // 6'h10: HDR-DDR CRC Data Generation/Receive State.
#define I3C_R_CM_TFR_ST_STATUS_CEXT         0x11 // 6'h11: Clock Extension State.
#define I3C_R_CM_TFR_ST_STATUS_HALT         0x12 // 6'h12: Halt State.
//          Warning: current implementation seems to have HALT state indicated by 0x13 instead of 0x12!




#endif
