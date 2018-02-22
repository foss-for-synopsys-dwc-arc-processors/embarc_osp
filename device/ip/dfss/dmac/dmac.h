/*
* 
* CONFIDENTIAL AND PROPRIETARY INFORMATION
* 
* Copyright (c) 2015 Synopsys, Inc. All rights reserved.
* This software and documentation contain confidential and
* proprietary information that is the property of
* Synopsys, Inc. The software and documentation are
* furnished under a license agreement and may be used
* or copied only in accordance with the terms of the license
* agreement. No part of the software and documentation
* may be reproduced, transmitted, or translated, in any
* form or by any means, electronic, mechanical, manual,
* optical, or otherwise, without prior written permission
* of Synopsys, Inc., or as expressly provided by the license agreement.
* Reverse engineering is prohibited, and reproduction,
* disclosure or use without specific written authorization
* of Synopsys Inc. is strictly forbidden.
*/
/*==========================================================================
* Library DW_DFSS-1.1.1
* ========================================================================== */

#ifndef DMAC_H_
#define DMAC_H_

#ifdef __Xdmac

/* DMAC enable/disable; set DMAC Base address */
extern void _dma_enable(int enable);     /* 0=disable, 1=enable */
extern void _dma_set_base(unsigned int address);

/* DMA interrupts: clear */
extern void _dma_int_clear(unsigned int mask);

/* DMA channel */
extern void _dma_chan_reset(unsigned int mask);
extern void _dma_chan_enable(unsigned int mask, int enable);  /* 0=disable, 1=enable */
extern void _dma_chan_prio(unsigned int mask, int prio);      /* 0=low, 1=high */
extern void _dma_chan_desc(int chan, _Uncached unsigned int *descr);
extern void _dma_chan_desc_aux(int chan, _Uncached unsigned int *descr);
extern void _dma_chan_desc_get_ctrl(int chan, unsigned int *ctrl);
extern void _dma_chan_desc_aux_get_ctrl(int chan, unsigned int *ctrl);

#endif

#endif /* DMAC_H_ */

