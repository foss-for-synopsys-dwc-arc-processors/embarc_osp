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
#ifdef MID_FATFS /* only available when enable fatfs middleware */
#include <stddef.h>
#include <string.h>

#include "arc.h"
#include "arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "arc_exception.h"

#include "board.h"

#ifdef ENABLE_OS
#include "os_hal_inc.h"
#endif

#include "ff_diskio.h"
#include "ff_sdcard.h"

#include "sdcard_sdio.h"

#define SDCARD_SDIO_ID		0

#if (USE_HSDK_SDCARD_SDIO_0)

static FS_SDCARD_SDIO_CTRL sdcard_sdio0 = {
	.id = 0,
	.drv_status = STA_NOINIT
};

static int32_t sdcard_sdio_0_diskio_initialize(void)
{
	sdcard_sdio0.host = (void *)sdio_get_dev(SDCARD_SDIO_ID);
	return sdcard_sdio_diskio_initialize(&sdcard_sdio_0_diskio);
}

static int32_t sdcard_sdio_0_diskio_status(void)
{
	return sdcard_sdio_diskio_status(&sdcard_sdio_0_diskio);
}

static int32_t sdcard_sdio_0_diskio_read(void *buf, uint32_t sector, uint32_t count)
{
	return sdcard_sdio_diskio_read(&sdcard_sdio_0_diskio, buf, sector, count);
}

static int32_t sdcard_sdio_0_diskio_write(const void *buf, uint32_t sector, uint32_t count)
{
	return sdcard_sdio_diskio_write(&sdcard_sdio_0_diskio, buf, sector, count);
}

static int32_t sdcard_sdio_0_diskio_ioctl(uint32_t cmd, void *buf)
{
	return sdcard_sdio_diskio_ioctl(&sdcard_sdio_0_diskio, cmd, buf);
}

static void sdcard_sdio_0_diskio_timerproc(void)
{
	return;
}

FATFS_DISKIO sdcard_sdio_0_diskio = {
	(void *)&sdcard_sdio0,
	sdcard_sdio_0_diskio_initialize,
	sdcard_sdio_0_diskio_status,
	sdcard_sdio_0_diskio_read,
	sdcard_sdio_0_diskio_write,
	sdcard_sdio_0_diskio_ioctl,
	sdcard_sdio_0_diskio_timerproc
};

#endif /* USE_HSDK_SDCARD_SDIO_0 */

#endif /* MID_FATFS */
