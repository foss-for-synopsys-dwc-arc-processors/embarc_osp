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
 * \defgroup	BOARD_EMSK_DRV_MID_FS_SDCARD_SPI	EMSK SDCard SPI Interface Driver
 * \ingroup	BOARD_EMSK_DRV_MID_FS_SDCARD
 * \brief	EMSK Fatfs SDCard SPI Interface Driver
 * \details
 *		Realize the sdcard driver using spi interface for fatfs, under the abstract of
 *	fatfs diskio.
 */

/**
 * \file
 * \ingroup	BOARD_EMSK_DRV_MID_FS_SDCARD_SPI
 * \brief	sdcard driver using spi interface for fatfs
 */

/**
 * \addtogroup	BOARD_EMSK_DRV_MID_FS_SDCARD_SPI
 * @{
 */
#ifdef MID_FATFS /* only available when enable fatfs middleware */
#include <stddef.h>
#include <string.h>

#include "arc.h"
#include "arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "arc_exception.h"

#include "dev_spi.h"
#include "board.h"

#ifdef ENABLE_OS
#include "os_hal_inc.h"
#endif

#include "ff_diskio.h"
#include "ff_sdcard.h"

#include "emsk_sdcard_spi.h"

#if USE_EMSK_SDCARD_SPI_0

#define EMSK_SDCARD_SPI_0_ID		(DW_SPI_0_ID)
#define EMSK_SDCARD_SPI_0_LINE		BOARD_SDCARD_SPI_LINE

/** SDCARD SPI FREQ & CLK MODE SETTINGS */
#define EMSK_SDCARD_SPI_0_FREQ		BOARD_SPI_FREQ
#define EMSK_SDCARD_SPI_0_CLKMODE	SPI_CLK_MODE_0

#define EMSK_SDCARD_SPI_0_CPULOCK_ENABLE


static int32_t sdcard_spi_card_is_writeprotect(void)
{
	return 0;
}

static int32_t sdcard_spi_card_is_inserted(void)
{
	return 1;
}

static int32_t sdcard_spi_0_diskio_initialize(void)
{
	return sdcard_spi_diskio_initialize(&sdcard_spi_0_diskio);
}

static int32_t sdcard_spi_0_diskio_status(void)
{
	return sdcard_spi_diskio_status(&sdcard_spi_0_diskio);
}

static int32_t sdcard_spi_0_diskio_read(void *buf, uint32_t sector, uint32_t count)
{
	return sdcard_spi_diskio_read(&sdcard_spi_0_diskio, buf, sector, count);
}

static int32_t sdcard_spi_0_diskio_write(const void *buf, uint32_t sector, uint32_t count)
{
	return sdcard_spi_diskio_write(&sdcard_spi_0_diskio, buf, sector, count);
}

static int32_t sdcard_spi_0_diskio_ioctl(uint32_t cmd, void *buf)
{
	return sdcard_spi_diskio_ioctl(&sdcard_spi_0_diskio, cmd, buf);
}

static void sdcard_spi_0_diskio_timerproc(void)
{
	sdcard_spi_diskio_timerproc(&sdcard_spi_0_diskio);
}

static FS_SDCARD_SPI_CTRL sdcard_spi_0_ctrl = {
	0, 0, 0,
	SDCARD_SPI_FREQ_400K, EMSK_SDCARD_SPI_0_FREQ, EMSK_SDCARD_SPI_0_CLKMODE,
	DW_SPI_0_ID, BOARD_SDCARD_SPI_LINE,
	sdcard_spi_card_is_writeprotect,
	sdcard_spi_card_is_inserted
};

FATFS_DISKIO sdcard_spi_0_diskio = {
	(void *)&sdcard_spi_0_ctrl,
	sdcard_spi_0_diskio_initialize,
	sdcard_spi_0_diskio_status,
	sdcard_spi_0_diskio_read,
	sdcard_spi_0_diskio_write,
	sdcard_spi_0_diskio_ioctl,
	sdcard_spi_0_diskio_timerproc
};

#endif /* USE_EMSK_SDCARD_SPI_0 */

#endif /* MID_FATFS */
/** @} end of group BOARD_EMSK_DRV_MID_FS_SDCARD_SPI */
