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
 * @file
 * @ingroup ARC_HAL_MISC_CACHE
 * @brief Cache manipulation
 *
 * This module contains functions for manipulation caches.
 */

#include "arc/arc_cache.h"

struct cache_config {
	uint8_t ver;            /* Version */
	uint8_t assoc;          /* Cache Associativity */
	uint16_t line;          /* Cache line/block size */
	uint32_t capacity;      /* Capacity */
};

static struct cache_config icache_config, dcache_config;
#ifdef ARC_FEATURE_SL2CACHE_PRESENT
static struct cache_config slc_config;
#endif

/**
 * @brief Invalidate instruction cache lines
 *
 * @param start_addr Start address in instruction cache
 * @param size Bytes to be invalidated
 * @return 0, succeeded, -1, failed
 */
int32_t icache_invalidate_mlines(uint32_t start_addr, uint32_t size)
{
	uint32_t end_addr;
	uint32_t line_size;
	uint32_t status;

	if ((size == 0) || (size > icache_config.capacity)) {
		return -1;
	}

	line_size = (uint32_t)(icache_config.line);
	end_addr = start_addr + size - 1;
	start_addr &= (uint32_t)(~(line_size - 1));

	status = cpu_lock_save();
	do {
		arc_aux_write(AUX_IC_IVIL, start_addr);
		arc_nop();
		arc_nop();
		arc_nop();
		start_addr += line_size;
	} while (start_addr <= end_addr);
	cpu_unlock_restore(status);

	return 0;
}

/**
 * @brief Lock instruction cache lines
 *
 * @param start_addr Start address in instruction cache
 * @param size Bytes to be locked
 * @return 0, succeeded, -1, failed (cache already locked or other reasons)
 */
int32_t icache_lock_mlines(uint32_t start_addr, uint32_t size)
{
	uint32_t end_addr;
	uint32_t line_size;
	uint32_t status;
	int32_t ercd = 0;

	if ((size == 0) || (size > icache_config.capacity)) {
		return -1;
	}

	line_size = (uint32_t)(icache_config.line);
	end_addr = start_addr + size - 1;
	start_addr &= (uint32_t)(~(line_size - 1));

	status = cpu_lock_save();
	do {
		arc_aux_write(AUX_IC_LIL, start_addr);
		if (arc_aux_read(AUX_IC_CTRL) & IC_CTRL_OP_SUCCEEDED) {
			start_addr += line_size;
		} else {
			ercd = -1;  /* the operation failed */
			break;
		}
	} while (start_addr <= end_addr);
	cpu_unlock_restore(status);

	return ercd;
}

#if ARC_FEATURE_ICACHE_FEATURE == 2
/**
 * @brief Directly write icache internal ram
 *
 * @param cache_addr Icache internal address(way+index+offset)
 * @param tag Cache tag to write (tag+lock bit+valid bit)
 * @param data Cache data to write
 * @return 0, succeeded, -1, failed
 */
int32_t icache_direct_write(uint32_t cache_addr, uint32_t tag, uint32_t data)
{
	if (arc_aux_read(AUX_IC_CTRL) & IC_CTRL_INDIRECT_ACCESS) {
		return -1;
	}
	arc_aux_write(AUX_IC_RAM_ADDR, cache_addr);
	arc_aux_write(AUX_IC_TAG, tag);
	arc_aux_write(AUX_IC_DATA, data);

	return 0;
}

/**
 * @brief Directly read icache internal ram
 *
 * @param cache_addr Icache internal address(way+index+offset)
 * @param tag Cache tag to read (tag+index+lock bit+valid bit)
 * @param data Cache data to read
 * @return 0, succeeded, -1, failed
 */
int32_t icache_direct_read(uint32_t cache_addr, uint32_t *tag, uint32_t *data)
{
	if (arc_aux_read(AUX_IC_CTRL) & IC_CTRL_INDIRECT_ACCESS) {
		return -1;
	}
	arc_aux_write(AUX_IC_RAM_ADDR, cache_addr);
	*tag = arc_aux_read(AUX_IC_TAG);
	*data = arc_aux_read(AUX_IC_DATA);

	return 0;
}

/**
 * @brief Indirectly read icache internal ram
 *
 * @param mem_addr Memory address
 * @param tag Cache tag to read
 * @param data Cache data to read
 * @return 0, succeeded, -1, failed
 */
int32_t icache_indirect_read(uint32_t mem_addr, uint32_t *tag, uint32_t *data)
{
	if (!(arc_aux_read(AUX_IC_CTRL) & IC_CTRL_INDIRECT_ACCESS)) {
		return -1;
	}
	arc_aux_write(AUX_IC_RAM_ADDR, mem_addr);
	if (arc_aux_read(AUX_IC_CTRL) & IC_CTRL_OP_SUCCEEDED) {
		*tag = arc_aux_read(AUX_IC_TAG);
		*data = arc_aux_read(AUX_IC_DATA);
	} else {
		return -1;  /* the specified memory is not in icache */
	}
	return 0;
}
#endif

#ifdef ARC_FEATURE_SL2CACHE_PRESENT
// SLC_OP_PREFETCH is not implemented
void _slc_rgn_op(uint32_t start_addr, uint32_t size, uint32_t op)
{
	uint32_t ctrl;
	uint32_t end_addr;

	if (!slc_enabled()) {
		return;
	}
	ctrl = arc_aux_read(AUX_SLC_CTRL);
	// /* Don't rely on default value of IM bit */
	// if (!(op & SLC_OP_FLUSH))		/* i.e. SLC_OP_INV */
	// {
	// 	ctrl &= ~SLC_CTRL_IM;	/* clear IM: Disable flush before Inv */
	// }
	// else
	// {
	// 	ctrl |= SLC_CTRL_IM;
	// }

	if (op & SLC_OP_INV) {
		ctrl |= SLC_CTRL_RGN_OP_INV;    /* Inv or flush-n-inv */
	} else {
		ctrl &= ~SLC_CTRL_RGN_OP_INV;
	}
	arc_aux_write(AUX_SLC_CTRL, ctrl);
	/*
	 * Lower bits are ignored, no need to clip
	 * END needs to be setup before START (latter triggers the operation)
	 * END can't be same as START, so add (l2_line_sz - 1) to sz
	 */
	end_addr = start_addr + size + slc_config.line - 1;
	/*
	 * Upper addresses (ARC_AUX_SLC_RGN_END1 and ARC_AUX_SLC_RGN_START1)
	 * are always == 0 as we don't use PAE40, so we only setup lower ones
	 * (ARC_AUX_SLC_RGN_END and ARC_AUX_SLC_RGN_START)
	 */
	arc_aux_write(AUX_SLC_RGN_END, end_addr);
	arc_aux_write(AUX_SLC_RGN_START, start_addr);
	/* Make sure "busy" bit reports correct stataus, see STAR 9001165532 */
	arc_aux_read(AUX_SLC_CTRL);
	while (arc_aux_read(AUX_SLC_CTRL) & SLC_CTRL_BUSY) {
		;
	}
}

#endif

/**
 * @brief Invalidate data cache lines
 *
 * @param start_addr Start address in data cache
 * @param size Bytes to be invalidated
 * @return 0, succeeded, -1, failed
 */
int32_t dcache_invalidate_mlines(uint32_t start_addr, uint32_t size)
{
	uint32_t end_addr;
	uint32_t line_size;
	uint32_t status;
#ifdef ARC_FEATURE_SL2CACHE_PRESENT
	uint32_t start = start_addr;
#endif

	if ((size == 0) || (size > dcache_config.capacity)) {
		return -1;
	}

	line_size = (uint32_t)(dcache_config.line);
	end_addr = start_addr + size - 1;
	start_addr &= (uint32_t)(~(line_size - 1));

	status = cpu_lock_save();
	do {
		arc_aux_write(AUX_DC_IVDL, start_addr);
		arc_nop();
		arc_nop();
		arc_nop();
		/* wait for flush completion */
		while (arc_aux_read(AUX_DC_CTRL) & DC_CTRL_FLUSH_STATUS) {
			;
		}
		start_addr += line_size;
	} while (start_addr <= end_addr);

#ifdef ARC_FEATURE_SL2CACHE_PRESENT
	_slc_rgn_op(start, size, SLC_OP_INV);
#endif
	cpu_unlock_restore(status);

	return 0;

}

/**
 * @brief Flush data cache lines to memory
 *
 * @param start_addr Start address
 * @param size Bytes to be flushed
 * @return 0, succeeded, -1, failed
 */
int32_t dcache_flush_mlines(uint32_t start_addr, uint32_t size)
{
	uint32_t end_addr;
	uint32_t line_size;
	uint32_t status;
#ifdef ARC_FEATURE_SL2CACHE_PRESENT
	uint32_t start = start_addr;
#endif

	if ((size == 0) || (size > dcache_config.capacity)) {
		return -1;
	}

	line_size = (uint32_t)(dcache_config.line);
	end_addr = start_addr + size - 1;
	start_addr &= (uint32_t)(~(line_size - 1));

	status = cpu_lock_save();
	do {
		arc_aux_write(AUX_DC_FLDL, start_addr);
		arc_nop();
		arc_nop();
		arc_nop();
		/* wait for flush completion */
		while (arc_aux_read(AUX_DC_CTRL) & DC_CTRL_FLUSH_STATUS) {
			;
		}
		start_addr += line_size;
	} while (start_addr <= end_addr);

#ifdef ARC_FEATURE_SL2CACHE_PRESENT
	_slc_rgn_op(start, size, SLC_OP_FLUSH);
#endif
	cpu_unlock_restore(status);

	return 0;
}

/**
 * @brief Lock data cache lines
 *
 * @param start_addr Start address in data cache
 * @param size Bytes to be locked
 * @return 0, succeeded, -1, failed
 */
int32_t dcache_lock_mlines(uint32_t start_addr, uint32_t size)
{
	uint32_t end_addr;
	uint32_t line_size;
	uint32_t status;
	int32_t ercd = 0;

	if ((size == 0) || (size > dcache_config.capacity)) {
		return -1;
	}

	line_size = (uint32_t)(dcache_config.line);
	end_addr = start_addr + size - 1;
	start_addr &= (uint32_t)(~(line_size - 1));

	status = cpu_lock_save();
	do {
		arc_aux_write(AUX_DC_LDL, start_addr);
		arc_nop();
		if (arc_aux_read(AUX_DC_CTRL) & DC_CTRL_OP_SUCCEEDED) {
			start_addr += line_size;
		} else {
			ercd = -1;  /* the operation failed */
			break;
		}
	} while (start_addr <= end_addr);
	cpu_unlock_restore(status);

	return ercd;
}

/**
 * @brief Directly write dcache internal ram
 *
 * @param cache_addr Dcache internal address(way+index+offset)
 * @param tag Cache tag to write
 * @param data Cache data to write
 * @return 0, succeeded, -1, failed
 */
int32_t dcache_direct_write(uint32_t cache_addr, uint32_t tag, uint32_t data)
{
	if (arc_aux_read(AUX_DC_CTRL) & DC_CTRL_INDIRECT_ACCESS) {
		return -1;
	}

	arc_aux_write(AUX_DC_RAM_ADDR, cache_addr);
	arc_aux_write(AUX_DC_TAG, tag);
	arc_aux_write(AUX_DC_DATA, data);

	return 0;
}

/**
 * @brief Directly read dcache internal ram
 *
 * @param cache_addr Dcache internal address(way+index+offset)
 * @param tag Cache tag to read
 * @param data Cache data to read
 * @return 0, succeeded, -1, failed
 */
int32_t dcache_direct_read(uint32_t cache_addr, uint32_t *tag, uint32_t *data)
{
	if (arc_aux_read(AUX_DC_CTRL) & DC_CTRL_INDIRECT_ACCESS) {
		return -1;
	}

	arc_aux_write(AUX_DC_RAM_ADDR, cache_addr);
	*tag = arc_aux_read(AUX_DC_TAG);
	*data = arc_aux_read(AUX_DC_DATA);

	return 0;
}

/**
 * @brief Indirectly read dcache internal ram
 *
 * @param mem_addr Memory address(tag+index+offset)
 * @param tag Cache tag to read
 * @param data Cache data to read
 * @return 0, succeeded, -1, failed
 */
int32_t dcache_indirect_read(uint32_t mem_addr, uint32_t *tag, uint32_t *data)
{
	if (!(arc_aux_read(AUX_DC_CTRL) & DC_CTRL_INDIRECT_ACCESS)) {
		return -1;
	}

	arc_aux_write(AUX_DC_RAM_ADDR, mem_addr);
	if (arc_aux_read(AUX_DC_CTRL) & DC_CTRL_OP_SUCCEEDED) {
		*tag = arc_aux_read(AUX_DC_TAG);
		*data = arc_aux_read(AUX_DC_DATA);
	} else {
		return -1;  /* the specified memory is not in dcache */
	}

	return 0;
}

/**
 * @brief  Initialize cache
 * 1. invalidate icache and dcache
 * 2. Only support ARCv2 cache
 */
void arc_cache_init(void)
{
	uint32_t build_cfg;

	build_cfg = arc_aux_read(AUX_BCR_D_CACHE);

	dcache_config.ver = build_cfg & 0xff;

	if (dcache_config.ver >= 0x04) { /* ARCv2 */
		dcache_enable(DC_CTRL_DISABLE_FLUSH_LOCKED |
			      DC_CTRL_INDIRECT_ACCESS | DC_CTRL_INVALID_FLUSH);
		dcache_invalidate();
		dcache_config.assoc = 1 << ((build_cfg >> 8) & 0xf);
		dcache_config.capacity = 512 << ((build_cfg >> 12) & 0xf);
		dcache_config.line = 16 << ((build_cfg >> 16) & 0xf);
	}

	build_cfg = arc_aux_read(AUX_BCR_I_CACHE);

	icache_config.ver = build_cfg & 0xff;

	if (icache_config.ver >= 0x04) { /* ARCv2 */
		icache_config.assoc = 1 << ((build_cfg >> 8) & 0xf);
		icache_config.capacity = 512 << ((build_cfg >> 12) & 0xf);
		icache_config.line = 8 << ((build_cfg >> 16) & 0xf);

		icache_enable(IC_CTRL_IC_ENABLE);
		icache_invalidate();
	}

#ifdef ARC_FEATURE_SL2CACHE_PRESENT
	build_cfg = arc_aux_read(AUX_BCR_SLC);
	slc_config.ver = build_cfg & 0xff;

	build_cfg = arc_aux_read(AUX_SLC_CACHE_CONFIG);
	slc_enable(SLC_CTRL_DISABLE_FLUSH_LOCKED | SLC_CTRL_IM);
	slc_invalidate();
	slc_config.assoc = 4 << ((build_cfg >> 6) & 0x3);
	// default 128KB = 0x20000
	slc_config.capacity = 0x20000 << ((build_cfg) & 0xf);
	slc_config.line = 128 >> ((build_cfg >> 4) & 0x3);
#endif

}
