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

#ifdef BOARD_EMSDP
#define UDMA_TIMER_ID TIMER_0
#else
#define UDMA_TIMER_ID TIMER_1
#endif

//Waring: remember to increase heap size in makefile in order to use multiple channels.
//Warning: With multiple channels in use, the memory usage is increase and it could exceed cache's capability. A significent redution of cycles for cache operations will be observed.
#define DMA_USE_CHANNEL_NUM 	4		//DMA_ALL_CHANNEL_NUM
#define TEST_INCREMENT			1024	//MEMSZ_GAP
#if DMA_USE_CHANNEL_NUM	> DMA_ALL_CHANNEL_NUM
#warning Use more channels than the board has!
#undef DMA_USE_CHANNEL_NUM
#define DMA_USE_CHANNEL_NUM		DMA_ALL_CHANNEL_NUM
#endif

//define USE_EXTRA_TASK will stop reading of comparison result, assuming that all memory copy / dma copy are succeed
#define USE_EXTRA_TASK
#ifdef USE_EXTRA_TASK
#define EXTRA_TASK_NUM 1
#else
#define EXTRA_TASK_NUM 0
#endif

#define _MEMORY_FENCE()				_arc_sync()
#define _DCACHE_FLUSH_MLINES(addr, size)	dcache_flush_mlines((uint32_t)(addr), (uint32_t)(size))
#define _DCACHE_INVALIDATE_MLINES(addr, size)	dcache_invalidate_mlines((uint32_t)(addr), (uint32_t)(size))
#define _ICACHE_INVALIDATE_MLINES(addr, size)	icache_invalidate_mlines((uint32_t)(addr), (uint32_t)(size))

static volatile uint32_t start = 0;
static uint32_t perf_id = 0xFF;
/** performance timer initialization */
static void perf_init(uint32_t id)
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
uint32_t perf_end(void)
{
	uint32_t end = 0;

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
static dma_channel_t dma_chn[DMA_USE_CHANNEL_NUM];
static dma_desc_t dma_desc[DMA_USE_CHANNEL_NUM];
static dma_ctrl_t dma_ctrl;
int32_t extra_cpy_task(uint32_t size);
int32_t dma_prepare(void)
{
	int32_t dma_channel = DMA_CHN_ANY;
	DMA_CTRL_SET_OP(&dma_ctrl, DMA_SINGLE_TRANSFER);
	DMA_CTRL_SET_RT(&dma_ctrl, DMA_AUTO_REQUEST);
	DMA_CTRL_SET_DTT(&dma_ctrl, DMA_MEM2MEM);
	DMA_CTRL_SET_DWINC(&dma_ctrl, DMA_DW4INC4);
	DMA_CTRL_SET_AM(&dma_ctrl, DMA_AM_SRCINC_DSTINC);
	DMA_CTRL_SET_ARB(&dma_ctrl, 255);
	DMA_CTRL_SET_INT(&dma_ctrl, DMA_INT_ENABLE);

	for(uint8_t i = 0; i < DMA_USE_CHANNEL_NUM; i++){
		dmac_config_desc(dma_desc + i, NULL, NULL, 0, &dma_ctrl);
		dmac_desc_add_linked(dma_desc + i, NULL);

		/* Init and configure dma channel transfer with transfer descriptor */
		dmac_init_channel(dma_chn + i);
		dmac_config_channel(dma_chn + i, dma_desc + i);
		/* Reserve a channel for use */
		dma_channel = dmac_reserve_channel(DMA_CHN_ANY, dma_chn + i, DMA_REQ_SOFT);
		if(dma_channel == DMA_CHN_INVALID){
			EMBARC_PRINTF("dmac_reserve_channel No.%d failed, ret %d\r\n", i, dma_channel);
		}
	}
	return dma_channel;
}

void dma_finish(void)
{
	/* Release channel resource */
	for(uint8_t i = 0; i < DMA_USE_CHANNEL_NUM; i++){
		dmac_release_channel(dma_chn+i);
	}
}

#define TEST_ENABLE_DMA_INTERRUPT
static volatile uint32_t task_done_count = 0;
static void dma_xfer_callback(void *param)
{
	// dma_channel_t *dma_chn = (dma_channel_t *)param;
	arc_lock();
	task_done_count++;
	arc_unlock();
	// EMBARC_PRINTF("i%d ", dma_chn->channel);
}

uint32_t cycles_cnt[15];
uint32_t cycles_idx = 0;
int32_t dma_copy(void *dst, void *src, uint32_t size)
{
	int32_t ercd = -1;
	cycles_idx = 0;
	dma_callback_t xfer_cb = NULL;
	uint8_t ch_idx;
	uint32_t ch_dst, ch_src, ch_size;

#ifdef TEST_ENABLE_DMA_INTERRUPT
	xfer_cb = dma_xfer_callback;
#endif
	ch_dst = (uint32_t)dst;
	ch_src = (uint32_t)src;
	ch_size = size / DMA_USE_CHANNEL_NUM;
	cycles_cnt[cycles_idx++] = perf_end();
	for(ch_idx = 0; ch_idx < DMA_USE_CHANNEL_NUM; ch_idx++){
		//EMBARC_PRINTF("dma_copy:%d src%x, dst%x, size%d \r\n", ch_idx, ch_src, ch_dst, ch_size);
		dmac_config_desc(dma_desc + ch_idx, (void *)ch_src, (void *)ch_dst, ch_size, &dma_ctrl);
		ch_src += ch_size;
		ch_dst += ch_size;
	}
	cycles_cnt[cycles_idx++] = perf_end();

	_MEMORY_FENCE();
	_DCACHE_FLUSH_MLINES((void *)src, size);
	// _ICACHE_INVALIDATE_MLINES((void *)src, size);
	_DCACHE_FLUSH_MLINES((void *)dst, size);
	// _ICACHE_INVALIDATE_MLINES((void *)dst, size);
	cycles_cnt[cycles_idx++] = perf_end();

	/* Start channel transfer with priority, without callback  */
	for(ch_idx = 0; ch_idx < DMA_USE_CHANNEL_NUM; ch_idx++){
		ercd = dmac_start_channel(dma_chn + ch_idx, xfer_cb, DMA_CHN_HIGH_PRIO);
		if(ercd != 0){
			EMBARC_PRINTF("dma_copy: start channel failed, index %d ret %d\r\n", ch_idx, ercd);
		}
	}
	cycles_cnt[cycles_idx++] = perf_end();
#ifdef USE_EXTRA_TASK
	ercd = extra_cpy_task(size);
	// EMBARC_PRINTF("dma_copy: extra_cpy_task ret %d\r\n", ercd);
#endif
	/* Wait until transfer is done */
	while(task_done_count < DMA_USE_CHANNEL_NUM + EXTRA_TASK_NUM);
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
	for (uint32_t i = 0; i < size; i ++) {
		src[i] = i;
		dst[i] = 0xFF;
	}
}

int32_t cmp_data(uint8_t *src, uint8_t *dst, uint32_t size)
{
	for (uint32_t i = 0; i < size; i ++) {
		if (src[i] != dst[i]) {
			//EMBARC_PRINTF("%u:%u %u\r\n", i, src[i], dst[i]);
			#ifndef USE_EXTRA_TASK
				return -1;
			#endif
		}
	}
	return 0;
}

int32_t perf_memcpy(uint8_t *src, uint8_t *dst, uint32_t size)
{
	uint32_t cycles;
	task_done_count = 0;
	init_data(src, dst, size);
	perf_init(UDMA_TIMER_ID);
	perf_start();
	memcpy(dst, src, size);
#ifdef USE_EXTRA_TASK
	if(extra_cpy_task(size)!=0){
		return -1;
	}
#endif
	while(task_done_count < EXTRA_TASK_NUM);
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
	task_done_count = 0;
	init_data(src, dst, size);
	perf_init(UDMA_TIMER_ID);
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
	perf_init(UDMA_TIMER_ID);
	perf_start();
	cycles = perf_end();
	return cycles;
}

#ifdef USE_EXTRA_TASK
int32_t extra_cpy_task(uint32_t size){
	uint8_t *src_ptr = NULL;
	uint8_t *dst_ptr = NULL;

	src_ptr = (uint8_t *)malloc(size);
	dst_ptr = (uint8_t *)malloc(size);
	if ((src_ptr == NULL) || (dst_ptr == NULL)) {
		EMBARC_PRINTF("extra_cpy_task: Not enough memory for testing, exit this example!\r\n");
		return -1;
	}
	init_data(src_ptr, dst_ptr, size);
	memcpy(dst_ptr, src_ptr, size);
	if (cmp_data(src_ptr, dst_ptr, size) == 0) {
		arc_lock();
		task_done_count++;
		arc_unlock();
		free(src_ptr);
		free(dst_ptr);
		return 0;
	} else {
		EMBARC_PRINTF("extra_cpy_task: data error!\r\n");
		free(src_ptr);
		free(dst_ptr);
		return -1;
	}
}
#endif //USE_EXTRA_TASK

int main( void )
{
	uint32_t test_sz = TEST_INCREMENT;
	uint8_t *src_ptr = NULL;
	uint8_t *dst_ptr = NULL;
	int32_t dmacpy_cycles, memcpy_cycles;
	int32_t ret;

	/** Must init uDMA before use it */
	EMBARC_PRINTF("DMA test: DMA has %d channels, now uses %d channels\r\n", DMA_ALL_CHANNEL_NUM, DMA_USE_CHANNEL_NUM);
	dmac_init(&udma);
	ret = dma_prepare();
	if (ret == DMA_CHN_INVALID) {
		EMBARC_PRINTF("dma channel failed, exit.\r\n");
		return -1;
	}
	EMBARC_PRINTF("Performance benchmark timer overhead is %u cycles\r\n", perf_overhead());
	EMBARC_PRINTF("Performance benchmark for both dmacpy and memcpy including overhead\r\n");
	EMBARC_PRINTF("XFER_SIZE DESC_CONF CACHE_FLUSH XFER_START XFER_WAIT CACHE_INV DMACPY MEMCPY\r\n");
	do {
		if(test_sz % DMA_USE_CHANNEL_NUM != 0){
			EMBARC_PRINTF("dma_copy: test_sz unable to be divided by %d channels, skip\r\n", DMA_USE_CHANNEL_NUM);
		} else {
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
			for (uint32_t i = 1; i < cycles_idx; i++) {
				EMBARC_PRINTF("%u ", cycles_cnt[i]-cycles_cnt[i-1]);
			}
			EMBARC_PRINTF("%u %u\r\n", (uint32_t)dmacpy_cycles, (uint32_t)memcpy_cycles);

			// EMBARC_PRINTF("Copy %u bytes, dmacpy cost %u cycles, memcpy cost %u cycles\r\n", \
			// 	test_sz, (uint32_t)dmacpy_cycles, (uint32_t)memcpy_cycles);
			free(src_ptr);
			free(dst_ptr);
		}
		test_sz += TEST_INCREMENT;
		if (test_sz > 0x2000 * DMA_USE_CHANNEL_NUM) {
			EMBARC_PRINTF("DMA only support 8K cell transfer\r\n");
			break;
		}
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