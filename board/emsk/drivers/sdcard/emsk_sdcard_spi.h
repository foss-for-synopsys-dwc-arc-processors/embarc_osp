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
 * \file
 * \ingroup	BOARD_EMSK_DRV_MID_FS_SDCARD_SPI
 * \brief	header file of sdcard driver using spi interface for fatfs
 */

/**
 * \addtogroup	BOARD_EMSK_DRV_MID_FS_SDCARD_SPI
 * @{
 */
#ifndef _EMSK_SDCARD_SPI_H_
#define _EMSK_SDCARD_SPI_H_

#ifdef MID_FATFS /* only available when enable fatfs middleware */
#include "ff_diskio.h"

#define USE_EMSK_SDCARD_SPI_0 	1

#define SDCARD_SPI_FREQ_400K		(400000)	/*!< sdcard spi freq:400KHz */
#define SDCARD_SPI_FREQ_1M		(1000000)	/*!< sdcard spi freq:2MHz */
#define SDCARD_SPI_FREQ_2M		(2000000)	/*!< sdcard spi freq:2MHz */
#define SDCARD_SPI_FREQ_4M 		(4000000)	/*!< sdcard spi freq:4MHz */

#ifdef __cplusplus
extern "C" {
#endif

#if USE_EMSK_SDCARD_SPI_0
extern FATFS_DISKIO sdcard_spi_0_diskio;
#endif /* USE_EMSK_SDCARD_SPI_0 */

#ifdef __cplusplus
}
#endif



#endif /* MID_FATFS */

#endif /** _EMSK_SDCARD_SPI_H_ */

/** @} end of group BOARD_EMSK_DRV_MID_FS_SDCARD_SPI */