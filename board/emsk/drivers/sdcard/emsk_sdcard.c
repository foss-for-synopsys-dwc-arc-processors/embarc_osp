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
 * @defgroup	BOARD_EMSK_DRV_MID_FS_SDCARD	EMSK Fatfs Middleware SDCard Driver
 * @ingroup	BOARD_EMSK_DRIVER
 * @brief	EMSK Fatfs Middleware SDCard Interface Driver
 * @details
 *		Realize the sdcard driver for fatfs based on the middleware fatfs diskio abstract
 *	layer, sdcard interface can be spi or sdio and so on.
 */

/**
 * @file
 * @ingroup	BOARD_EMSK_DRV_MID_FS_SDCARD
 * @brief	sdcard driver for fatfs of emsk board
 */

/**
 * @addtogroup	BOARD_EMSK_DRV_MID_FS_SDCARD
 * @{
 */
#ifdef MID_FATFS /* only available when enable fatfs middleware */

#include "embARC_toolchain.h"
#include "ff_diskio.h"
#include "emsk_sdcard.h"
#include <time.h>

uint32_t diskio_get_fattime(void)
{
	struct tm *p_tm;
	time_t cur_time;
	uint32_t fattime;

	cur_time = time(NULL);
	p_tm = localtime(&cur_time);
	fattime = ((p_tm->tm_year + 1900 - 1980) & 0x7f) << 25;
	fattime |= ((p_tm->tm_mon + 1) & 0xf) << 21;
	fattime |= ((p_tm->tm_mday) & 0x1f) << 16;
	fattime |= ((p_tm->tm_hour) & 0x1f) << 11;
	fattime |= ((p_tm->tm_min) & 0x3f) << 5;
	fattime |= ((p_tm->tm_sec >> 1) & 0x1f);

	return fattime;
}

FATFS_DISKIO *get_fatfs_diskio(uint32_t drvid)
{
	switch (drvid) {
#if (USE_EMSK_SDCARD_SPI_0)
	case EMSK_SDCARD_0_DRVID:
		return &sdcard_spi_0_diskio;
		break;
#endif
	default:
		break;
	}
	return NULL;
}

/** update in 1ms period interrupt for disk status update */
void emsk_sdcard_1ms_update(void)
{
#if (USE_EMSK_SDCARD_SPI_0)
	sdcard_spi_0_diskio.diskio_timerproc();
#endif
}

#endif /* MID_FATFS */

/** @} end of group BOARD_EMSK_DRV_MID_FS_SDCARD */
