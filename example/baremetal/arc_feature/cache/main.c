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
#include "embARC.h"
#include "embARC_debug.h"

#ifdef BOARD_EMSDP
//EMDK only has 2 way cache, it might have some chances that the cache get switched out before locking,
//   causing cached data being wrote to memory in advance
//   If that is the case, please test other lines of cache_data
#define DCACHE_LINE_NUM		256
#define DCACHE_LINE_LENGTH 	32
#else
#define DCACHE_LINE_NUM		64
#define DCACHE_LINE_LENGTH 	128
#endif

#define FILLED_DATA			0xcc

__attribute__ ((aligned(DCACHE_LINE_LENGTH*DCACHE_LINE_NUM)))
static unsigned char cache_data[DCACHE_LINE_NUM][DCACHE_LINE_LENGTH] = {0xFF};

/**
 * \brief	call cache related functions
 */
int main(void)
{
	uint32_t cache_ver;
	uint32_t cache_size;
	uint32_t cache_llen;
	uint32_t cache_ways;
	uint32_t build_cfg;
	volatile register int i = 0;
	volatile register int j = 0;

	build_cfg = _arc_aux_read(AUX_BCR_D_CACHE);

	cache_ver = build_cfg & 0xff;

	if (cache_ver) {
		cache_ways = 1 << ((build_cfg >> 8) & 0xf);
		cache_size = 512 << ((build_cfg >> 12) & 0xf);
		cache_llen = 16 << ((build_cfg >> 16) & 0xf);
		/* show data cache info */
		EMBARC_PRINTF("Data cache, ver:%d, capacity:%d, ways:%d, line len:%d\n",
				cache_ver, cache_size, cache_ways, cache_llen);
		build_cfg = _arc_aux_read(AUX_BCR_I_CACHE);
	} else {
		EMBARC_PRINTF("no data cache, error!\n");
		while(1);
	}

	cache_ver = build_cfg & 0xff;

	cache_ways = 1 << ((build_cfg >> 8) & 0xf);
	cache_size = 512 << ((build_cfg >> 12) & 0xf);
	cache_llen = 8 << ((build_cfg >> 16) & 0xf);

	if (cache_ver) {
	/* show instruction cache info */
		EMBARC_PRINTF("Instruction cache, ver:%d, capacity:%d, ways:%d, line len:%d\n",
			cache_ver, cache_size, cache_ways, cache_llen);
	} else {
		EMBARC_PRINTF("no instruction cache\n");
	}

	EMBARC_PRINTF("fill the data cache with data\n");
	for (i = 0; i < DCACHE_LINE_NUM; i++) {
		for (j = 0; j < DCACHE_LINE_LENGTH ; j++) {
			cache_data[i][j] = i;
		}
	}
	EMBARC_PRINTF("cache data:%x,  mem data:%x\n", _arc_read_cached_32(cache_data), _arc_read_uncached_32(cache_data));
	EMBARC_PRINTF("lock the data in data cache\n");
	dcache_lock_mlines((unsigned int)cache_data, 8192);
	EMBARC_PRINTF("invalidate data cache, even it's locked\n");
	dcache_invalidate_mlines((unsigned int)(cache_data)+4096, 4096);
	EMBARC_PRINTF("cache data:%x,  mem data:%x\n", _arc_read_cached_32(cache_data[DCACHE_LINE_NUM-1]), _arc_read_uncached_32(cache_data[DCACHE_LINE_NUM-1]));

	EMBARC_PRINTF("invalidate one cache line\n");
	dcache_invalidate_line((unsigned int)cache_data);
	EMBARC_PRINTF("cache the data into one cache line with 0x%x\n", FILLED_DATA);
	for (j = 0 ; j < DCACHE_LINE_LENGTH ; j++) {
		cache_data[0][j] = FILLED_DATA;
	}
	EMBARC_PRINTF("cache data:%x,  mem data:%x\n", _arc_read_cached_32(cache_data), _arc_read_uncached_32(cache_data));
	EMBARC_PRINTF("flush the changed cache line\n");
	dcache_flush_line((unsigned int)cache_data);
	EMBARC_PRINTF("cache data:%x,  mem data:%x\n", _arc_read_cached_32(cache_data), _arc_read_uncached_32(cache_data));
	EMBARC_PRINTF("cached the data into one cache line again with 0x55\n");
	for (j = 0 ; j < DCACHE_LINE_LENGTH ; j++) {
		cache_data[0][j] = 0x55;
	}
	EMBARC_PRINTF("cache data:%x,  mem data:%x\n", _arc_read_cached_32(cache_data), _arc_read_uncached_32(cache_data));
	EMBARC_PRINTF("lock the changed cache line\n");
	dcache_lock_line((unsigned int)cache_data);
	EMBARC_PRINTF("flush the whole data chache, the locked line won't be flushed\n");
	dcache_flush();
	EMBARC_PRINTF("cache data:%x,  mem data:%x\n", _arc_read_cached_32(cache_data), _arc_read_uncached_32(cache_data));
	EMBARC_PRINTF("refill the cache with data\n");
	for (i = DCACHE_LINE_NUM/2; i < DCACHE_LINE_NUM; i++) {
		for (j = 0; j < DCACHE_LINE_LENGTH ; j++) {
			cache_data[i][j] = j;
		}
	}
	EMBARC_PRINTF("cache data:%x,  mem data:%x\n", _arc_read_cached_32(cache_data[DCACHE_LINE_NUM-1]), _arc_read_uncached_32(cache_data[DCACHE_LINE_NUM-1]));
	EMBARC_PRINTF("flush multiple cache lines\n");
	dcache_flush_mlines((unsigned int)(cache_data)+5000, 4096);
	EMBARC_PRINTF("cache data:%x,  mem data:%x\n", _arc_read_cached_32(cache_data[DCACHE_LINE_NUM-1]), _arc_read_uncached_32(cache_data[DCACHE_LINE_NUM-1]));
	EMBARC_PRINTF("Cache Test Done\n");
	while(1);

	return E_SYS;
}