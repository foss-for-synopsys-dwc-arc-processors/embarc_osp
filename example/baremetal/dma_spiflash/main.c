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
#if defined(BOARD_EMSK)
#include "spi_flash.h"
#include "spi_flash_w25qxx.h"


#define BUFFER_SIZE 0x4000

uint8_t test_buffer[BUFFER_SIZE];
uint8_t test_buffer_rd[BUFFER_SIZE];
uint32_t flash_dma_test(uint32_t freq);
uint32_t flash_poll_test(uint32_t freq);

static volatile unsigned int start = 0;
static unsigned int perf_id = 0xFF;
static void perf_init(unsigned int id);
static void perf_start(void);
static unsigned int perf_end(void);

#define BOOTIMAGE_START		(0xD80000)

#define DMA_FLASH_TEST		1
#define POLL_FLASH_TEST		2

//// Uncomment the following test modes, just leave one TEST_MODE defined
//#define TEST_MODE		(DMA_FLASH_TEST|POLL_FLASH_TEST)
#define TEST_MODE		(DMA_FLASH_TEST)
//#define TEST_MODE		(POLL_FLASH_TEST)

/**
 * \brief	call cache related functions
 */
int main(void)
{
	uint32_t i = 0, freq;
	while(1) {
		i ++;
		freq = 1000000 * i;
#if (TEST_MODE & DMA_FLASH_TEST) == DMA_FLASH_TEST
		EMBARC_PRINTF("Do SPIFlash DMA Test @%dHz\r\n", freq);
		flash_dma_test(freq);
#endif
#if (TEST_MODE & POLL_FLASH_TEST) == POLL_FLASH_TEST
		EMBARC_PRINTF("Do SPIFlash Poll Test @%dHz\r\n", freq>>2);
		flash_poll_test(freq>>2);
#endif
	}
	return E_SYS;
}

uint32_t mem_cmp(uint8_t *src, uint8_t *dst, uint32_t size)
{
	uint32_t err_cnt = 0;
	for (int i = 0; i < size; i++) {
		if (src[i] != dst[i]) {
			err_cnt ++;
		}
	}
	return err_cnt;
}

uint32_t flash_dma_test(uint32_t freq)
{
	uint32_t start_addr = BOOTIMAGE_START & ~(FLASH_SECTOR_SIZE - 1);
	uint32_t i = 0, wr_err_cnt;
	uint8_t initval;
	uint32_t cost_cyc;

	initval = (uint8_t)OSP_GET_CUR_MS();
	EMBARC_PRINTF("Random value:%d\r\n", initval);

	for (i = 0; i < BUFFER_SIZE; i++) {
		test_buffer[i] = (initval+i) & 0xff;
	}
	spiflash_init(freq);
	perf_init(TIMER_1);
	EMBARC_PRINTF("SPI FLASH ID: 0x%x\r\n", spiflash_read_id());
	perf_start();
	spiflash_erase(start_addr, BUFFER_SIZE);
	cost_cyc = perf_end();
	EMBARC_PRINTF("SPI Flash Erase Finished in %d cycles\r\n", cost_cyc);
	perf_start();
	spiflash_read(start_addr, BUFFER_SIZE, test_buffer_rd);
	cost_cyc = perf_end();
	for (i = 0; i < BUFFER_SIZE; i++) {
		if (test_buffer_rd[i] != 0xFF) {
			EMBARC_PRINTF("SPI Erase failed:%d\r\n", i);
			break;
		}
	}
	EMBARC_PRINTF("SPI Flash Read Finished in %d cycles\r\n", cost_cyc);

	perf_start();
	spiflash_write(start_addr, BUFFER_SIZE, test_buffer);
	cost_cyc = perf_end();
	EMBARC_PRINTF("SPI Flash Write Finished in %d cycles\r\n", cost_cyc);
	perf_start();
	spiflash_read(start_addr, BUFFER_SIZE, test_buffer_rd);
	cost_cyc = perf_end();
	EMBARC_PRINTF("SPI Flash Read Finished in %d cycles\r\n", cost_cyc);
	/* verify read and write */
	if ((wr_err_cnt = mem_cmp(test_buffer_rd, test_buffer, BUFFER_SIZE)) > 0) {
		EMBARC_PRINTF("SPI FLASH Write Read Verification Error @%uHz, error count:%u\r\n", \
		              freq, wr_err_cnt);
	} else {
		EMBARC_PRINTF("SPI FLASH Write Read Verification successfully\r\n");
	}
	return 0;
}

uint32_t flash_poll_test(uint32_t freq)
{
	uint32_t start_addr = BOOTIMAGE_START & ~(FLASH_SECTOR_SIZE - 1);
	uint32_t i = 0, wr_err_cnt;
	uint8_t initval;
	uint32_t cost_cyc;
	W25QXX_DEF(flash, BOARD_SFLASH_SPI_ID, BOARD_SFLASH_SPI_LIN, FLASH_PAGE_SIZE, FLASH_SECTOR_SIZE);

	initval = (uint8_t)OSP_GET_CUR_MS();
	EMBARC_PRINTF("Random value:%d\r\n", initval);

	for (i = 0; i < BUFFER_SIZE; i++) {
		test_buffer[i] = (initval+i) & 0xff;
	}
	w25qxx_init(flash, freq);
	perf_init(TIMER_1);
	EMBARC_PRINTF("SPI FLASH ID: 0x%x\r\n", w25qxx_read_id(flash));
	perf_start();
	w25qxx_erase(flash, start_addr, BUFFER_SIZE);
	cost_cyc = perf_end();
	EMBARC_PRINTF("SPI Flash Erase Finished in %d cycles\r\n", cost_cyc);
	perf_start();
	w25qxx_read(flash, start_addr, BUFFER_SIZE, test_buffer_rd);
	cost_cyc = perf_end();
	for (i = 0; i < BUFFER_SIZE; i++) {
		if (test_buffer_rd[i] != 0xFF) {
			EMBARC_PRINTF("SPI Erase failed:%d\r\n", i);
			break;
		}
	}
	EMBARC_PRINTF("SPI Flash Read Finished in %d cycles\r\n", cost_cyc);

	perf_start();
	w25qxx_write(flash, start_addr, BUFFER_SIZE, test_buffer);
	cost_cyc = perf_end();
	EMBARC_PRINTF("SPI Flash Write Finished in %d cycles\r\n", cost_cyc);
	perf_start();
	w25qxx_read(flash, start_addr, BUFFER_SIZE, test_buffer_rd);
	cost_cyc = perf_end();
	EMBARC_PRINTF("SPI Flash Read Finished in %d cycles\r\n", cost_cyc);
	/* verify read and write */
	if ((wr_err_cnt = mem_cmp(test_buffer_rd, test_buffer, BUFFER_SIZE)) > 0) {
		EMBARC_PRINTF("SPI FLASH Write Read Verification Error @%uHz, error count:%u\r\n", \
		              freq, wr_err_cnt);
	} else {
		EMBARC_PRINTF("SPI FLASH Write Read Verification successfully\r\n");
	}
	return 0;
}

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
static unsigned int perf_end(void)
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
#else
int main(void)
{
	EMBARC_PRINTF("\r\n\r\ndma_spiflash example only support EMSK board!\r\n");
	while(1);
}
#endif //BOARD_EMSK