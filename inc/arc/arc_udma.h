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
 * \ingroup ARC_HAL_MISC_UDMA
 * \brief header file of ARC uDMA driver
 */

/**
 * \addtogroup ARC_HAL_MISC_UDMA
 * @{
 */
#ifndef _ARC_UDMA_H_
#define _ARC_UDMA_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "embARC_toolchain.h"
#include "arc.h"
#include "arc_builtin.h"
#include "arc_cache.h"
#include "arc_udma_config.h"

#ifdef OS_FREERTOS
#include "os_hal_inc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DMA_ALL_CHANNEL_NUM
/*!< Number of all DMA channels */
#define DMA_ALL_CHANNEL_NUM			CORE_DMAC_CHANNELS
#endif
#ifndef DMA_REGISTER_CHANNEL_NUM
/*!< Number of all aux-register based DMA channels */
#define DMA_REGISTER_CHANNEL_NUM		CORE_DMAC_REGISTERS
#endif

/*!< Mask of all DMA channels at the most */
#define DMA_ALL_CHANNEL_MASK			0xFFFF

#define DMACTRLx_OP_OFS		(0)		/*!< bit field offset: bit 0,1 */
#define DMACTRLx_R_OFS		(2)		/*!< bit field offset: bit 2 */
#define DMACTRLx_DTT_OFS	(3)		/*!< bit field offset: bit 3,4 */
#define DMACTRLx_DWINC_OFS	(5)		/*!< bit field offset: bit 5,6,7 */
#define DMACTRLx_SIZE_OFS	(8)		/*!< bit field offset: bit 8..20 */
#define DMACTRLx_ARB_OFS	(21)		/*!< bit field offset: bit 21..28 */
#define DMACTRLx_INT_OFS	(29)		/*!< bit field offset: bit 29 */
#define DMACTRLx_AM_OFS		(30)		/*!< bit field offset: bit 30,31 */

#define DMACTRLx_OP(x)		((x)<<DMACTRLx_OP_OFS)		/*!< bit field value: bit 0,1 */
#define DMACTRLx_R(x)		((x)<<DMACTRLx_R_OFS)		/*!< bit field value: bit 2 */
#define DMACTRLx_DTT(x)		((x)<<DMACTRLx_DTT_OFS)		/*!< bit field value: bit 3,4 */
#define DMACTRLx_DWINC(x)	((x)<<DMACTRLx_DWINC_OFS)	/*!< bit field value: bit 5,6,7 */
#define DMACTRLx_SIZE(x)	((x)<<DMACTRLx_SIZE_OFS)	/*!< bit field value: bit 8..20 */
#define DMACTRLx_ARB(x)		((x)<<DMACTRLx_ARB_OFS)		/*!< bit field value: bit 21..28 */
#define DMACTRLx_INT(x)		((x)<<DMACTRLx_INT_OFS)		/*!< bit field value: bit 29 */
#define DMACTRLx_AM(x)		((x)<<DMACTRLx_AM_OFS)		/*!< bit field value: bit 30,31 */

#define DMACTRLx_OP_MASK	DMACTRLx_OP(0x3)		/*!< bit field mask: bit 0,1 */
#define DMACTRLx_R_MASK		DMACTRLx_R(0x1)			/*!< bit field mask: bit 2 */
#define DMACTRLx_DTT_MASK	DMACTRLx_DTT(0x3)		/*!< bit field mask: bit 3,4 */
#define DMACTRLx_DWINC_MASK	DMACTRLx_DWINC(0x7)		/*!< bit field mask: bit 5,6,7 */
#define DMACTRLx_SIZE_MASK	DMACTRLx_SIZE(0x1FFF)		/*!< bit field mask: bit 8..20 */
#define DMACTRLx_ARB_MASK	DMACTRLx_ARB(0xFF)		/*!< bit field mask: bit 21..28 */
#define DMACTRLx_INT_MASK	DMACTRLx_INT(0x1)		/*!< bit field mask: bit 29 */
#define DMACTRLx_AM_MASK	DMACTRLx_AM(0x3)		/*!< bit field mask: bit 30,31 */

#define DMACHANNEL(x)		(0x1<<(x))			/*!< Channel number to Bit */

/** callback function for dma transfer, param should be current dma channel transfer structure dma_channel_t */
typedef void (*dma_callback_t) (void *param);

typedef struct _dma_ctrl_field {
	uint32_t op    : 2;	/*!< operation field */
	uint32_t rt    : 1;	/*!< request type field */
	uint32_t dtt   : 2;	/*!< source and destination target types field */
	uint32_t dwinc : 3;	/*!< data width and address increment field */
	uint32_t blksz : 13;	/*!< data transfer in bytes field */
	uint32_t arb   : 8;	/*!< arbitration size */
	uint32_t intm  : 1;	/*!< interrupt mode bit */
	uint32_t am    : 2;	/*!< address mode field */
} dma_ctrl_field_t;

/** DMA channel control register union */
typedef union _dma_ctrl {
	dma_ctrl_field_t bits;	/*!< control register in bits format */
	uint32_t value;		/*!< control register in word format */
} dma_ctrl_t;

/** DMA working status enumeration */
typedef enum _dma_status {
	DMA_IDLE = 0,	/*!< Current DMA status is IDLE */
	DMA_BUSY = 1,	/*!< Current DMA status is busy, in transfer */
	DMA_ERROR = 2,	/*!< Current DMA status is error, in transfer error */
	DMA_STOP = 3	/*!< Current DMA is stop by user */
} dma_status_t;

/** DMA channel type enumeration */
typedef enum _dma_channel_type {
	DMA_CHN_ANY = -1,	/*!< Any channel, request one */
	DMA_CHN_INVALID = -2	/*!< Invalid channel */
} dma_channel_type_t;

/** DMA channel request or trigger source enumeration */
typedef enum _dma_request_source {
	DMA_REQ_SOFT = 0,	/*!< Software trigger */
	DMA_REQ_PERIPHERAL = 1 	/*!< Peripheral trigger */
} dma_request_source_t;


/** DMA descriptor (DMA channel registers) */
typedef struct _dma_desc {
	uint32_t DMACTRLx;	/*!< DMA channel control register */
	uint32_t DMASARx;	/*!< DMA channel end source address register */
	uint32_t DMADARx;	/*!< DMA channel end destination address register */
	uint32_t DMALLPx;	/*!< DMA channel link-list pointer (for chaining/linking separate DMA transfers only) */
} dma_desc_t;

/** DMA Channel transfer structure */
typedef struct _dma_channel {
	int32_t channel;	/*!< Channel ID binded to this transfer */
	int16_t priority;	/*!< Transfer priority */
	int16_t int_enable;	/*!< Interrupt enable status */
	dma_request_source_t source;	/*!< DMA request or trigger source for this transfer */
	dma_desc_t *desc;	/*!< DMA transfer descriptor for this transfer */
	volatile dma_status_t status;	/*!< DMA transfer status of this transfer */
	dma_callback_t callback;	/*!< DMA transfer callback */
} dma_channel_t;

/** DMA channel control bit field enumeration - DMA Operation (OP) */
typedef enum _dma_ctrl_op {
	DMA_INVALID_TRANSFER = 0,	/*!< Invalid channel */
	DMA_SINGLE_TRANSFER = 1,	/*!< Single Block */
	DMA_AUTO_LINKED_TRANSFER = 2,	/*!< Link-List (Auto-Request) */
	DMA_MANUAL_LINKED_TRANSFER = 3	/*!< Link-List (Manual-Request) */
} dma_ctrl_op_t;

/** DMA channel control bit field enumeration - Request Type (RT) */
typedef enum _dma_ctrl_rt {
	DMA_AUTO_REQUEST = 0,	/*!< Auto-request following channel arbitration */
	DMA_MANUAL_REQUEST = 1	/*!< Manual-request following channel arbitration */
} dma_ctrl_rt_t;

/** DMA channel control bit field enumeration - Data Transfer Type (DTT) */
typedef enum _dma_ctrl_dtt {
	DMA_MEM2MEM = 0,	/*!< Memory to Memory */
	DMA_MEM2AUX = 1,	/*!< Memory to Auxiliary */
	DMA_AUX2MEM = 2,	/*!< Auxiliary to Memory */
	DMA_AUX2AUX = 3		/*!< Auxiliary to Auxiliary */
} dma_ctrl_dtt_t;

/** DMA channel control bit field enumeration - Data Width/Increment (DW/INC) */
typedef enum _dma_ctrl_dwinc {
	DMA_DW1INC1 = 0,	/*!< dw=byte, inc=byte */
	DMA_DW1INC2 = 1,	/*!< dw=byte, inc=half-word */
	DMA_DW1INC4 = 2,	/*!< dw=byte, inc=word */
	DMA_DW2INC2 = 3,	/*!< dw=half-word, inc=half-word */
	DMA_DW2INC4 = 4,	/*!< dw=half-word, inc=word */
	DMA_DW4INC4 = 5,	/*!< dw=word, inc=word */
	DMA_DWINC_CLR = 6,	/*!< clear mode (dw=word, inc=word) */
	DMA_DW8INC8 = 7		/*!< dw=double-word, inc=double-word(Only supported in HS) */
} dma_ctrl_dwinc_t;

/** DMA channel control bit field enumeration - Internal/External Interrupt enable (INT) */
typedef enum _dma_ctrl_int {
	DMA_INT_DISABLE = 0,	/*!< Interrupt disabled, no interrupt raised is on completion of a data transfer */
	DMA_INT_ENABLE = 1	/*!< Interrupt enabled, a level interrupt raised on completion of a data transfer */
} dma_ctrl_int_t;

/** DMA channel control bit field enumeration - Address update Mode (AM) */
typedef enum _dma_ctrl_am {
	DMA_AM_SRCNOT_DSTNOT = 0,	/*!< No Source or Destination Address increment */
	DMA_AM_SRCINC_DSTNOT = 1,	/*!< Source Address incremented, Destination Address not incremented */
	DMA_AM_SRCNOT_DSTINC = 2,	/*!< Source Address not incremented, Destination Address incremented */
	DMA_AM_SRCINC_DSTINC = 3	/*!< Source Address and Destination Address incremented */
} dma_ctrl_am_t;

/** DMA channel priority enumeration */
typedef enum _dma_chn_prio {
	DMA_CHN_NORM_PRIO = 0,	/*!< Normal priority */
	DMA_CHN_HIGH_PRIO = 1	/*!< High priority */
} dma_chn_prio_t;


/** Number of Memory based DMA Channel */
#define DMA_MEMORY_CHANNEL_NUM		((DMA_ALL_CHANNEL_NUM) - (DMA_REGISTER_CHANNEL_NUM))

#if CORE_DMAC_INTERNAL_VERSION == 1
#if DMA_MEMORY_CHANNEL_NUM > 0
/** Memory based DMA channels existed */
#define DMA_MEMORY_HEADER
#endif
#endif

/** Check whether channel have register interface */
#define DMA_CHECK_REGISTER(channel)	(channel >= (DMA_MEMORY_CHANNEL_NUM))

/** DMA state to maintain uDMA resources */
typedef struct _dma_state {
#ifdef DMA_MEMORY_HEADER
	/*!< Memory based DMA descriptors */
	EMBARC_ALIGNED(256) volatile dma_desc_t mem_dma_descs[DMA_MEMORY_CHANNEL_NUM];
#endif
#if CORE_DMAC_INTERNAL_VERSION > 1
	EMBARC_ALIGNED(32) volatile uint32_t mem_dma_ptrs[DMA_ALL_CHANNEL_NUM];
#endif
	/*!< All uDMA channel resources */
	volatile dma_channel_t *dma_chns[DMA_ALL_CHANNEL_NUM];
#ifdef OS_FREERTOS
	/*!< Lock to protect these dma resources */
	xSemaphoreHandle lock;
#endif
} dma_state_t;

// **********************************
// ********   High level API   ******
// **********************************

/**
 * \brief       Init uDMA controller with a valid dma_state_t structure
 *
 * If you want to use this uDMA driver, you need to call the dmac_init function
 * with a valid state, dmac will init the valid state. If initialized successfully,
 * you can use the other uDMA APIs.
 *
 * \param       state  DMA state structure, this should not be NULL
 *
 * \retval      -1     state is NULL
 * \retval      0      Init successfully
 */
extern int32_t dmac_init(dma_state_t *state);
/**
 * \brief       De-init uDMA controller
 */
extern void dmac_close(void);

/** Set dma_ctrl_t structure ctrl bit field - OP
 *
 * \param       ctrl  This should be dma_ctrl_t structure, and not NULL
 * \param       val   set value
 */
#define DMA_CTRL_SET_OP(ctrl, val)	(ctrl)->bits.op = val;
/** Set dma_ctrl_t structure ctrl bit field - RT
 *
 * \param       ctrl  This should be dma_ctrl_t structure, and not NULL
 * \param       val   set value
 */
#define DMA_CTRL_SET_RT(ctrl, val)	(ctrl)->bits.rt = val;
/** Set dma_ctrl_t structure ctrl bit field - DTT
 *
 * \param       ctrl  This should be dma_ctrl_t structure, and not NULL
 * \param       val   set value
 */
#define DMA_CTRL_SET_DTT(ctrl, val)	(ctrl)->bits.dtt = val;
/** Set dma_ctrl_t structure ctrl bit field - DW/INC
 *
 * \param       ctrl  This should be dma_ctrl_t structure, and not NULL
 * \param       val   set value
 */
#define DMA_CTRL_SET_DWINC(ctrl, val)	(ctrl)->bits.dwinc = val;
/** Calculate right size
 *
 * \param       x     transfer size
 * \retval	(x-1)	value after calculation
 */
#define DMA_CTRL_BLKSZ(x)		(x-1)
/** Set dma_ctrl_t structure ctrl bit field - BLOCKSIZE
 *
 * \param       ctrl  This should be dma_ctrl_t structure, and not NULL
 * \param       val   set value
 */
#define DMA_CTRL_SET_BLKSZ(ctrl, val)	(ctrl)->bits.blksz = val;
/** Set dma_ctrl_t structure ctrl bit field - ARB
 *
 * \param       ctrl  This should be dma_ctrl_t structure, and not NULL
 * \param       val   set value
 */
#define DMA_CTRL_SET_ARB(ctrl, val)	(ctrl)->bits.arb = val;
/** Set dma_ctrl_t structure ctrl bit field - I
 *
 * \param       ctrl  This should be dma_ctrl_t structure, and not NULL
 * \param       val   set value
 */
#define DMA_CTRL_SET_INT(ctrl, val)	(ctrl)->bits.intm = val;
/** Set dma_ctrl_t structure ctrl bit field - AM
 *
 * \param       ctrl  This should be dma_ctrl_t structure, and not NULL
 * \param       val   set value
 */
#define DMA_CTRL_SET_AM(ctrl, val)	(ctrl)->bits.am = val;

/** Set dma_ctrl_t structure ctrl value
 *
 * \param       ctrl  This should be dma_ctrl_t structure, and not NULL
 * \param       val   set value
 */
#define DMA_CTRL_SET_VALUE(ctrl, val)	(ctrl)->value = val;

/**
 * \brief       Configure uDMA descriptor with source/destination address,
 *              transfer size in bytes and ctrl mode.
 *
 * Note: The DMALLPx of desc will be set to NULL in this function, and DMACTRLx
 * of desc will be set with right transfer size
 *
 * \param       desc  uDMA descriptor, this should not be NULL
 * \param       src   source address
 * \param       dst   destination address
 * \param[in]   size  actual transfer size in bytes
 * \param       ctrl  DMA channel control value
 *
 * \retval      -1    desc is NULL
 * \retval      0     OK
 */
extern int32_t dmac_config_desc(dma_desc_t *desc, void *src, void *dst, uint32_t size, dma_ctrl_t *ctrl);
/**
 * \brief       Add linked descriptor for uDMA, head -> next
 *
 * \param       head  head uDMA descriptor, should not be NULL
 * \param       next  next uDMA descriptor, could be NULL
 *
 * \retval      -1    head is NULL
 * \retval      0     OK
 */
extern int32_t dmac_desc_add_linked(dma_desc_t *head, dma_desc_t *next);
/**
 * \brief       Init a dma_channel_t structure - dma_chn with initial value
 *
 * Note: Channel will be set to DMA_CHN_INVALID
 *
 * \param       dma_chn  uDMA channel structure, should not be NULL
 *
 * \retval      -1       dma_chn is NULL
 * \retval      0        OK
 */
extern int32_t dmac_init_channel(dma_channel_t *dma_chn);
/**
 * \brief       Configure uDMA channel with uDMA descriptor
 *
 * \param       dma_chn  uDMA channel structure, should not be NULL
 * \param       desc     uDMA descriptor
 *
 * \retval      -1       dma_chn is NULL
 * \retval      0        OK
 */
extern int32_t dmac_config_channel(dma_channel_t *dma_chn, dma_desc_t *desc);
/**
 * \brief       Reserve a DMA channel, and bind it with dma_chn, and set the dma
 *              trigger source
 *
 * \param[in]   channel                This can be DMA_CHN_ANY or any valid
 *                                     channel id. For DMA_CHN_ANY, it will try
 *                                     to peek an available channel. For any
 *                                     valid channel id, it will try to reserve
 *                                     that channel.
 * \param       dma_chn                uDMA channel structure, should not be
 *                                     NULL
 * \param[in]   source                 DMA trigger source, this can be any value
 *                                     in dma_request_source_t enum
 *
 * \retval      DMA_CHN_INVALID        dma_chn is NULL, or channel is not a
 *                                     valid one, or there is no channel
 *                                     available now
 * \retval      0-DMA_ALL_CHANNEL_NUM  the channel id that reserved
 */
extern int32_t dmac_reserve_channel(int32_t channel, dma_channel_t *dma_chn, dma_request_source_t source);
/**
 * \brief       Start uDMA transfer for dma_chn, and set callback and transfer
 *              priority
 *
 * Note: When callback is NULL, then all INT field in DMA descriptor will be set
 * to interrupt disable, otherwise it will be set to interrupt enable.
 *
 * \param       dma_chn   uDMA channel structure, should not be NULL
 * \param[in]   callback  callback function, when DMA transfer is done, it will
 *                        be called with parameter which value is dma_chn
 * \param[in]   priority  uDMA transfer priority
 *
 * \retval      -1        dma_chn is NULL or dma_chn->desc is NULL or channel of
 *                        dma_chn is not a valid one or dma_chn is still in
 *                        transfer
 * \retval      -2        When channel is a aux-based channel, the dma
 *                        descriptor should not be a linked list.
 * \retval      0         OK
 */
extern int32_t dmac_start_channel(dma_channel_t *dma_chn, dma_callback_t callback, uint32_t priority);
/**
 * \brief       Stop uDMA transfer for dma_chn
 *
 * \param       dma_chn  uDMA channel structure, should not be NULL
 *
 * \retval      -1       dma_chn is NULL or dma_chn->desc is NULL or channel of
 *                       dma_chn is not a valid one
 *
 * \retval      0        OK
 */
extern int32_t dmac_stop_channel(dma_channel_t *dma_chn);
/**
 * \brief       Release the channel used by dma_chn
 *
 * \param       dma_chn  uDMA channel structure, should not be NULL
 *
 * \retval      -1       dma_chn is NULL or dma_chn->desc is NULL or channel of
 *                       dma_chn is not a valid one
 * \retval      0        OK
 */
extern int32_t dmac_release_channel(dma_channel_t *dma_chn);
/**
 * \brief       Wait until channel transfer is done
 *
 * \param       dma_chn    uDMA channel structure, should not be NULL
 *
 * \retval      -1         dma_chn is NULL or dma_chn->desc is NULL or channel
 *                         of dma_chn is not a valid one
 * \retval      DMA_IDLE   Transfer is done without error
 * \retval      DMA_ERROR  Transfer is done with error
 */
extern int32_t dmac_wait_channel(dma_channel_t *dma_chn);
/**
 * \brief       Check channel transfer status
 *
 * \param       dma_chn    uDMA channel structure, should not be NULL
 *
 * \retval      -1         dma_chn is NULL or dma_chn->desc is NULL or channel
 *                         of dma_chn is not a valid one
 * \retval      DMA_BUSY   Still in transfer state
 * \retval      DMA_IDLE   Transfer is done without error
 * \retval      DMA_ERROR  Transfer is done with error
 */
extern int32_t dmac_check_channel(dma_channel_t *dma_chn);
/**
 * \brief       Clear channel transfer status and set it to DMA_IDLE
 *
 * \param       dma_chn  uDMA channel structure, should not be NULL
 *
 * \retval      -1       dma_chn is NULL or dma_chn->desc is NULL or channel of
 *                       dma_chn is not a valid one
 * \retval      0        OK
 */
extern int32_t dmac_clear_channel(dma_channel_t *dma_chn);

#ifdef __cplusplus
}
#endif

#endif /* _ARC_UDMA_H_ */
/** }@*/