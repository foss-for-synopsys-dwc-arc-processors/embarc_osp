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
#include "dev_sdio.h"

/* SDCard card type flags (MMC_GET_TYPE) */
#define SDCARD_CT_MMC		0x01		/*!< MMC ver 3 */
#define SDCARD_CT_SD1		0x02		/*!< SD ver 1 */
#define SDCARD_CT_SD2		0x04		/*!< SD ver 2 */
#define SDCARD_CT_SDC		(SDCARD_CT_SD1|SDCARD_CT_SD2)	/*!< SD */
#define SDCARD_CT_BLOCK		0x08		/*!< Block addressing */

#define SDCARD_SPI_SELECTED	(1)		/*!< sdcard chip selected */
#define SDCARD_SPI_DESELECTED	(0)		/*!< sdcard chip de-selected */


/**
 * \defgroup	MID_FS_FATFS_MMCSPI_CMDS	MMC/SD command
 * \ingroup	MID_FS_FATFS_MMCSPI
 * \brief	mmc/sd command
 * @{
 */
/* MMC/SD command */
#define CMD0		(0)		/*!< GO_IDLE_STATE */
#define CMD1		(1)		/*!< SEND_OP_COND (MMC) */
#define CMD2 		(2)
#define CMD3 		(3)
#define	ACMD41		(0x80+41)	/*!< SEND_OP_COND (SDC) */
#define CMD41		(41)		/*!< SEND_OP_COND (SDC) */
#define CMD6 		(6)		/*!< SWITCH FUNC  */
#define CMD7 		(7)
#define ACMD6		(0x80+6) 	/*!< SET BUS WIDTH */
#define CMD8		(8)		/*!< SEND_IF_COND */
#define CMD9		(9)		/*!< SEND_CSD */
#define CMD10		(10)		/*!< SEND_CID */
#define CMD12		(12)		/*!< STOP_TRANSMISSION */
#define CMD13		(13)
#define ACMD13		(0x80+13)	/*!< SD_STATUS (SDC) */
#define CMD16		(16)		/*!< SET_BLOCKLEN */
#define CMD17		(17)		/*!< READ_SINGLE_BLOCK */
#define CMD18		(18)		/*!< READ_MULTIPLE_BLOCK */
#define CMD23		(23)		/*!< SET_BLOCK_COUNT (MMC) */
#define	ACMD23		(0x80+23)	/*!< SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD23 		(23)		/*!< SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24		(24)		/*!< WRITE_BLOCK */
#define CMD25		(25)		/*!< WRITE_MULTIPLE_BLOCK */
#define CMD32		(32)		/*!< ERASE_ER_BLK_START */
#define CMD33		(33)		/*!< ERASE_ER_BLK_END */
#define CMD38		(38)		/*!< ERASE */
#define CMD55		(55)		/*!< APP_CMD */
#define CMD58		(58)		/*!< READ_OCR */
/** @} end of group MID_FS_FATFS_MMCSPI_CMDS */

typedef struct fatfs_sdcard_spi_ctrl {
	uint8_t card_type;			/*!< SDCard Type */
	volatile int8_t drv_status;		/*!< Physical drive status */
	volatile uint64_t ms_update;		/*!< 1ms update count , no clear */
	uint32_t init_freq;			/*!< spi frequency during initialization */
	uint32_t working_freq;			/*!< spi frequency during working */
	uint8_t clk_mode;			/*!< spi clock mode */
	uint8_t spi_master; 			/*!< spi master id */
	uint8_t cs;				/*!< the cs pin of sd card */
	int32_t (*card_is_writeprotect)(void);	/*!< detect whether card is under write protect */
	int32_t (*card_is_inserted)(void);	/*!< detect whether card is insert */
} FS_SDCARD_SPI_CTRL, *FS_SDCARD_SPI_CTRL_PTR;


typedef struct fs_sdcard_info {
	uint32_t csd[4];
	uint32_t cid[4];
	uint32_t ocr;
	uint32_t rca;
	uint8_t version;
	uint8_t high_capacity;
	uint32_t trans_speed;
	uint64_t c_size;
	uint32_t read_bl_len;
	uint32_t write_bl_len;
	uint32_t au; /* in sectors (512 Bytes) */
} FS_SDCARD_INFO, *FS_SDCARD_INFO_PTR;


typedef struct fs_sdcard_sdio_ctrl {
	uint8_t id;
	volatile int8_t drv_status;
	void * host;
	FS_SDCARD_INFO card_info;
} FS_SDCARD_SDIO_CTRL, *FS_SDCARD_SDIO_CTRL_PTR;

typedef struct fatfs_diskio {
	void * diskio_ctrl;			/*!< diskio control related */
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