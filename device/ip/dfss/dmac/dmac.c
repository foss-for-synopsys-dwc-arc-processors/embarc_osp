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

#include "dmac.h"

#ifdef __Xdmac

/* enable/disable the DMAC */
void _dma_enable(int enable)
{
	_sr(enable, 0x680);
}

/* set DMAC Base address */
void _dma_set_base(unsigned int address)
{
	_sr(address, 0x689);
}

void _dma_int_clear(unsigned int mask)
{
	_sr(mask, 0x688);
}

/* DMA channel */
void _dma_chan_reset(unsigned int mask)
{
	_sr(mask, 0x68a);
}

void _dma_chan_enable(unsigned int mask, int enable)
{
	/* 0=disable, 1=enable */
	_sr(mask, (0x682-enable));
}

void _dma_chan_prio(unsigned int mask, int prio)
{
	/* 0=low, 1=high */
	_sr(mask, (0x684-prio));
}

void _dma_chan_desc(int chan, _Uncached unsigned int *descr)
{
	int *ptr;
	ptr = (int *) (_lr(0x689)+(4*chan));
    *ptr = (int)descr;
}

void _dma_chan_desc_aux(int chan, _Uncached unsigned int *descr)
{
	_sr( descr[0], 0x690 + (3*chan));
	_sr( descr[1], 0x691 + (3*chan));
	_sr( descr[2], 0x692 + (3*chan));
	int *ptr;
	ptr = (int *) (_lr(0x689)+(4*chan));
    *ptr = (int)descr[3];
}

extern void _dma_chan_desc_get_ctrl(int chan, unsigned int *ctrl)
{
	int *ptr;
	ptr = (int *) (_lr(0x689)+(4*chan));
	ptr = (int *) *ptr;
    *ctrl = *ptr;
}

extern void _dma_chan_desc_aux_get_ctrl(int chan, unsigned int *ctrl)
{
    *ctrl = _lr(0x690 + (3*chan));
}

#endif
