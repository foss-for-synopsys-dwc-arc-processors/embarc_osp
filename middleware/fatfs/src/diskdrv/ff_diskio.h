/*
 *  Copyright (C) 2014, ChaN, all right reserved.
 *
 * + The FatFs module is a free software and there is NO WARRANTY.
 * + No restriction on use. You can use, modify and redistribute it for
 *   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
 * + Redistributions of source code must retain the above copyright notice.
 **/
/**
 * Copyright (C) 2015 Synopsys, Inc. All rights reserved.
 * Synopsys modifications to the FatFs module are license for use under the same
 * terms as FatFs module. Synopsys modifications include port for ARC processors
 * by Huaqi Fang huaqi.fang@synopsys.com, Synopsys, Inc.
 */

#ifndef _FF_DISKIO_H_
#define _FF_DISKIO_H_

#include "stdint.h"
#include "diskio.h"

/* SDCard card type flags (MMC_GET_TYPE) */
#define SDCARD_CT_MMC		0x01		/*!< MMC ver 3 */
#define SDCARD_CT_SD1		0x02		/*!< SD ver 1 */
#define SDCARD_CT_SD2		0x04		/*!< SD ver 2 */
#define SDCARD_CT_SDC		(SDCARD_CT_SD1|SDCARD_CT_SD2)	/*!< SD */
#define SDCARD_CT_BLOCK		0x08		/*!< Block addressing */

#define SDCARD_SPI_SELECTED	(1)		/*!< sdcard chip selected */
#define SDCARD_SPI_DESELECTED	(0)		/*!< sdcard chip de-selected */

typedef struct fatfs_sdcard_spi_strl {
	uint32_t card_type;			/*!< SDCard Type */
	volatile int32_t drv_status;		/*!< Physical drive status */
	volatile uint64_t ms_update;		/*!< 1ms update count , no clear */
	uint32_t init_freq;			/*!< spi frequence during initialization */
	uint32_t working_freq;			/*!< spi frequence during working */
	uint32_t clk_mode;			/*!< spi clock mode */
	int32_t (*spi_open)(uint32_t freq, uint32_t clk_mode);	/*!< open spi with this freq & clk_mode */
	int32_t (*spi_change_freq)(uint32_t freq);	/*!< change spi working freq */
	int32_t (*spi_close)(void);		/*!< close spi */
	int32_t (*spi_cs)(int32_t cs);		/*!< spi chip select control */
	int32_t (*spi_write)(const void *buf, uint32_t cnt);	/*!< write data by spi */
	int32_t (*spi_read)(void *buf, uint32_t cnt);		/*!< read data by spi */
	int32_t (*card_is_writeprotect)(void);			/*!< detect whether card is under write protect */
	int32_t (*card_is_inserted)(void);			/*!< detect whether card is insert */
} FS_SDCARD_SPI_CTRL, *FS_SDCARD_SPI_CTRL_PTR;

typedef struct fatfs_diskio {
	void *diskio_ctrl ;			/*!< diskio control related */
	int32_t (*diskio_initialize) (void);	/*!< initialize the disk */
	int32_t (*diskio_status) (void);	/*!< inquiry about the disk status */
	int32_t (*diskio_read) (void *buf, uint32_t sector, uint32_t count);	/*!< read data from disk */
	int32_t (*diskio_write) (const void *buf, uint32_t sector, uint32_t count);	/*!< write data to disk */
	int32_t (*diskio_ioctl) (uint32_t cmd, void *buf);	/*!< disk ioctl */
	void (*diskio_timerproc) (void);	/*!< This function must be called from timer interrupt routine
							in period of 1 ms to generate card control timing */
} FATFS_DISKIO, * FATFS_DISKIO_PTR;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief	gets current time
 * \retval
 * 	bit31:25	Year origin from the 1980 (0..127)
 * 	bit24:21	Month (1..12)
 * 	bit20:16	Day of the month(1..31)
 * 	bit15:11	Hour (0..23)
 * 	bit10:5		Minute (0..59)
 * 	bit4:0		Second / 2 (0..29)
 */
extern uint32_t diskio_get_fattime(void);
extern FATFS_DISKIO *get_fatfs_diskio(uint32_t drvid);

#ifdef __cplusplus
}
#endif

#endif /** _FF_DISKIO_H_ */