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
/* embARC HAL */
#include "embARC.h"
#include "embARC_debug.h"

#include "stdlib.h"

#define _MEMORY_FENCE()				_arc_sync()
#define _DCACHE_FLUSH_MLINES(addr, size)	dcache_flush_mlines((uint32_t)(addr), (uint32_t)(size))
#define _DCACHE_INVALIDATE_MLINES(addr, size)	dcache_invalidate_mlines((uint32_t)(addr), (uint32_t)(size))
#define _ICACHE_INVALIDATE_MLINES(addr, size)	icache_invalidate_mlines((uint32_t)(addr), (uint32_t)(size))

static volatile unsigned int start = 0;
static unsigned int perf_id = 0xFF;
/** performance timer initialization */
static void perf_init(unsigned int id)
{
	if (timer_start(id, TIMER_CTRL_NH, 0xFFFFFFFF) < 0) {
		EMBARC_PRINTF("perf timer init failed\r\n");
		while(1);
	}
	perf_id = id;
}

/** performance timer start */
static void perf_start(void)
{
	if (timer_current(perf_id, (void *)(&start)) < 0) {
		start = 0;
	}
}

/** performance timer end, and return the time passed */
unsigned int perf_end(void)
{
	unsigned int end = 0;

	if (timer_current(perf_id, (void *)(&end)) < 0) {
		return 0;
	}

	if (start < end) {
		return (end - start);
	} else {
		return (0xFFFFFFFF - start + end);
	}
}

static dma_state_t udma;
static dma_channel_t dma_chn;
static dma_desc_t dma_desc;
static dma_ctrl_t dma_ctrl;
static int32_t dma_channel = DMA_CHN_ANY; /* Notice: here must use int32_t type */

int32_t dma_prepare(void)
{
	DMA_CTRL_SET_OP(&dma_ctrl, DMA_SINGLE_TRANSFER);
	DMA_CTRL_SET_RT(&dma_ctrl, DMA_AUTO_REQUEST);
	DMA_CTRL_SET_DTT(&dma_ctrl, DMA_MEM2MEM);
	DMA_CTRL_SET_DWINC(&dma_ctrl, DMA_DW1INC1);
	DMA_CTRL_SET_AM(&dma_ctrl, DMA_AM_SRCINC_DSTINC);
	DMA_CTRL_SET_ARB(&dma_ctrl, 255);
	DMA_CTRL_SET_INT(&dma_ctrl, DMA_INT_DISABLE);

	dmac_config_desc(&dma_desc, NULL, NULL, 0, &dma_ctrl);

	dmac_desc_add_linked(&dma_desc, NULL);
	/* Init and configure dma channel transfer with transfer descriptor */
	dmac_init_channel(&dma_chn);
	dmac_config_channel(&dma_chn, &dma_desc);

	/* Reserve a channel for use */
	dma_channel = dmac_reserve_channel(DMA_CHN_ANY, &dma_chn, DMA_REQ_SOFT);
	return dma_channel;
}

void dma_finish(void)
{
	/* Release channel resource */
	dmac_release_channel(&dma_chn);
}

#define TEST_ENABLE_DMA_INTERRUPT
static void dma_xfer_callback(void *param)
{

}

uint32_t cycles_cnt[15];
uint32_t cycles_idx = 0;
int32_t dma_copy(void *dst, void *src, uint32_t size)
{
	int32_t ercd = -1;
	cycles_idx = 0;
	dma_callback_t xfer_cb = NULL;

#ifdef TEST_ENABLE_DMA_INTERRUPT
	xfer_cb = dma_xfer_callback;
#endif
	cycles_cnt[cycles_idx++] = perf_end();
	dmac_config_desc(&dma_desc, src, dst, size, &dma_ctrl);
	cycles_cnt[cycles_idx++] = perf_end();

	_MEMORY_FENCE();
	_DCACHE_FLUSH_MLINES((void *)src, size);
	// _ICACHE_INVALIDATE_MLINES((void *)src, size);
	_DCACHE_FLUSH_MLINES((void *)dst, size);
	// _ICACHE_INVALIDATE_MLINES((void *)dst, size);
	cycles_cnt[cycles_idx++] = perf_end();
	/* Start channel transfer with priority, without callback  */
	dmac_start_channel(&dma_chn, xfer_cb, DMA_CHN_HIGH_PRIO);
	cycles_cnt[cycles_idx++] = perf_end();

	/* Wait until transfer is done */
	if (dmac_wait_channel(&dma_chn) != DMA_IDLE) {
		ercd = -1;
	} else {
		ercd = 0;
	}
	cycles_cnt[cycles_idx++] = perf_end();

	_MEMORY_FENCE();
	_DCACHE_INVALIDATE_MLINES((void *)src, size);
	// _ICACHE_INVALIDATE_MLINES((void *)src, size);
	_DCACHE_INVALIDATE_MLINES((void *)dst, size);
	// _ICACHE_INVALIDATE_MLINES((void *)dst, size);
	cycles_cnt[cycles_idx++] = perf_end();

	return ercd;
}

void init_data(uint8_t *src, uint8_t *dst, uint32_t size)
{
	for (int i = 0; i < size; i ++) {
		src[i] = i;
		dst[i] = 0xFF;
	}
}

int32_t cmp_data(uint8_t *src, uint8_t *dst, uint32_t size)
{
	uint32_t err_cnt = 0;
	for (int i = 0; i < size; i ++) {
		if (src[i] != dst[i]) {
			err_cnt ++;
			//EMBARC_PRINTF("%u:%u %u\r\n", i, src[i], dst[i]);
			//return -1;
		}
	}
	if (err_cnt == 0) {
		return 0;
	} else {
		return -1;
	}
}

int32_t perf_memcpy(uint8_t *src, uint8_t *dst, uint32_t size)
{
	uint32_t cycles;
	init_data(src, dst, size);
	perf_init(TIMER_1);
	perf_start();
	memcpy(dst, src, size);
	cycles = perf_end();
	if (cmp_data(src, dst, size) == 0) {
		return cycles;
	} else {
		return -1;
	}
}

int32_t perf_dmacpy(uint8_t *src, uint8_t *dst, uint32_t size)
{
	uint32_t cycles;
	init_data(src, dst, size);
	perf_init(TIMER_1);
	perf_start();
	dma_copy(dst, src, size);
	cycles = perf_end();
	if (cmp_data(src, dst, size) == 0) {
		return cycles;
	} else {
		return -1;
	}
}

int32_t perf_overhead(void)
{
	uint32_t cycles;
	perf_init(TIMER_1);
	perf_start();
	cycles = perf_end();
	return cycles;
}

#define MEMSZ_GAP		256
int main( void )
{
	uint32_t test_sz = 256;
	uint8_t *src_ptr = NULL;
	uint8_t *dst_ptr = NULL;
	int32_t dmacpy_cycles, memcpy_cycles;

	/** Must init uDMA before use it */
	dmac_init(&udma);

	if (dma_prepare() == DMA_CHN_INVALID) {
		EMBARC_PRINTF("No valid dma channel for testing, failed this example.\r\n");
		return -1;
	}
	EMBARC_PRINTF("Performance benchmark timer overhead is %u cycles\r\n", perf_overhead());
	EMBARC_PRINTF("Performance benchmark for both dmacpy and memcpy including overhead\r\n");
	EMBARC_PRINTF("XFER_SIZE DESC_CONF CACHE_FLUSH XFER_START XFER_WAIT CACHE_INV DMACPY MEMCPY\r\n");
	do {
		src_ptr = (uint8_t *)malloc(test_sz);
		dst_ptr = (uint8_t *)malloc(test_sz);
		if ((src_ptr == NULL) || (dst_ptr == NULL)) {
			EMBARC_PRINTF("Not enough memory for testing, exit this example!\r\n");
			break;
		}
		memcpy_cycles = perf_memcpy(src_ptr, dst_ptr, test_sz);
		dmacpy_cycles = perf_dmacpy(src_ptr, dst_ptr, test_sz);
		if (memcpy_cycles == -1) {
			EMBARC_PRINTF("MEMCPY API Test failed!\r\n");
			break;
		}
		if (dmacpy_cycles == -1) {
			EMBARC_PRINTF("DMACPY API Test failed!\r\n");
			break;
		}
		EMBARC_PRINTF("%u ", test_sz);
		for (int i = 1; i < cycles_idx; i++) {
			EMBARC_PRINTF("%u ", cycles_cnt[i]-cycles_cnt[i-1]);
		}
		EMBARC_PRINTF("%u %u\r\n", (uint32_t)dmacpy_cycles, (uint32_t)memcpy_cycles);

		// EMBARC_PRINTF("Copy %u bytes, dmacpy cost %u cycles, memcpy cost %u cycles\r\n", \
		// 	test_sz, (uint32_t)dmacpy_cycles, (uint32_t)memcpy_cycles);

		test_sz += MEMSZ_GAP;
		if (test_sz > 8192) {
			EMBARC_PRINTF("DMA only support 8K cell transfer\r\n");
			break;
		}
		free(src_ptr);
		free(dst_ptr);
	} while(1);

	if (src_ptr != NULL) {
		free(src_ptr);
	}
	if (dst_ptr != NULL) {
		free(dst_ptr);
	}
	dma_finish();
	dmac_close();
	return 0;
}