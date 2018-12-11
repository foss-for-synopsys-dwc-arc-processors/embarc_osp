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

/**
 * \defgroup	MID_FS_FATFS_MMCSPI	FatFS filesystem Middleware SPI SDCard Driver Implement
 * \ingroup	MID_FS_FATFS
 * \brief	implement sdcard driver based on spi for fatfs
 *
 * \file
 * \brief	spi sdcard driver
 */
#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

#include "xprintf.h"

#include "diskio.h"
#include "ff.h"

#include "ff_diskio.h"

#include "dev_spi.h"

#include "diskio.h"

//#define DEBUG
#include "embARC_debug.h"

#include "embARC_error.h"

#include "ff_sdcard_spi.h"

#define SDCARD_SPI_CHECK_EXP(EXPR, ERROR_CODE)	 CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)


/** wait sdcard ready for command or data transfer, if success return E_OK, else return E_SYS */
static int32_t sdcard_spi_wait_ready(FS_SDCARD_SPI_CTRL_PTR sd_ctrl, uint32_t wt)
{
	uint64_t cur_time = 0;
	uint8_t busy_byte = 0;
	DEV_SPI_PTR spi;

	spi = spi_get_dev(sd_ctrl->spi_master);
	cur_time = sd_ctrl->ms_update; /*!< record current start time */
	do {
		spi->spi_read((void *)(&busy_byte), 1);
	} while((busy_byte != 0xff) && \
		((sd_ctrl->ms_update-cur_time) < wt)); /* Wait for card goes ready or timeout */

	return (busy_byte == 0xff) ? E_OK : E_SYS;
}

/** deselected sdcard */
static int32_t sdcard_spi_deselect(FS_SDCARD_SPI_CTRL_PTR sd_ctrl)
{
	uint8_t byte = 0xff;
	DEV_SPI_PTR spi;

	spi = spi_get_dev(sd_ctrl->spi_master);

	spi->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID((uint32_t)sd_ctrl->cs));
	spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID((uint32_t)sd_ctrl->cs));
	/* selected then transfer 1 dummy byte */
	spi->spi_write((const void *)(&byte), 1);

	return spi->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID((uint32_t)sd_ctrl->cs));
}

/** selected sdcard, if select success return E_OK, else return E_SYS */
static int32_t sdcard_spi_select(FS_SDCARD_SPI_CTRL_PTR sd_ctrl)
{
	uint8_t byte = 0xff;
	DEV_SPI_PTR spi;

	spi = spi_get_dev(sd_ctrl->spi_master);

	spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID((uint32_t)sd_ctrl->cs));
	/* selected then transfer 1 dummy byte */
	spi->spi_write((const void *)(&byte), 1);

	if (sdcard_spi_wait_ready(sd_ctrl, 500) == E_OK) {
		return E_OK;
	}

	spi->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID((uint32_t)sd_ctrl->cs));

	return E_SYS;
}

/** read data block by spi , ok return E_OK */
static int32_t sdcard_spi_read_block(FS_SDCARD_SPI_CTRL_PTR sd_ctrl, uint8_t *buf, uint32_t cnt)
{
#ifdef SDCARD_SPI_CPULOCK_ENABLE
	uint32_t  cpu_status;
#endif
	uint8_t token = 0;
	uint8_t crc[2];
	uint64_t cur_time = 0;
	DEV_SPI_PTR spi;

	spi = spi_get_dev(sd_ctrl->spi_master);

	if (buf == NULL) {
		return E_SYS;
	}

	cur_time = sd_ctrl->ms_update; /*!< record current start time */
	do { /* Wait for DataStart token in timeout of 200ms */
		spi->spi_read((void *)(&token), 1);
	} while((token == 0xff) && \
		((sd_ctrl->ms_update-cur_time) < 200));

	if (token != 0xfe) {
		/* Function fails if invalid DataStart token or timeout */
		return E_SYS;
	}

#ifdef SDCARD_SPI_CPULOCK_ENABLE
	cpu_status = cpu_lock_save();
#endif
	spi->spi_read((void *)buf, cnt); /* Read Data Block */
	spi->spi_read((void *)crc, 2);   /* Discard CRC */

#ifdef SDCARD_SPI_CPULOCK_ENABLE
	cpu_unlock_status(cpu_status);
#endif

	return E_OK;
}

/** write data block by spi , ok return E_OK */
static int32_t sdcard_spi_write_block(FS_SDCARD_SPI_CTRL_PTR sd_ctrl, const uint8_t *buf, uint32_t cnt, uint8_t token)
{
#ifdef SDCARD_SPI_CPULOCK_ENABLE
	uint32_t  cpu_status;
#endif
	uint8_t resp[3];

	DEV_SPI_PTR spi;
	spi = spi_get_dev(sd_ctrl->spi_master);

	if (buf == NULL) {
		return E_SYS;
	}

	if (sdcard_spi_wait_ready(sd_ctrl, 500) != E_OK) {
		return E_SYS;
	}
	spi->spi_write((const void *)&token, 1);
	if (token != 0xfd) {
#ifdef SDCARD_SPI_CPULOCK_ENABLE
		cpu_status = cpu_lock_save();
#endif

		spi->spi_write((const void *)buf, cnt); /* Write Data Block */
		spi->spi_read((void *)resp, 3);   /* Discard CRC and read response */

#ifdef SDCARD_SPI_CPULOCK_ENABLE
		cpu_unlock_status(cpu_status);
#endif
		if ((resp[2] & 0x1f) != 0x5) {
			return E_SYS;
		}
	}
	return E_OK;
}

/** send sdcard spi command */
static uint8_t sdcard_spi_send_cmd(FS_SDCARD_SPI_CTRL_PTR sd_ctrl, uint8_t cmd, uint32_t arg)
{
#ifdef SDCARD_SPI_CPULOCK_ENABLE
	uint32_t  cpu_status;
#endif
	uint8_t res = 0;
	uint8_t buf[8];
	uint8_t cnt;
	DEV_SPI_PTR spi = spi_get_dev(sd_ctrl->spi_master);

	if (cmd & 0x80) {   /* Send a CMD55 prior to ACMD<n> */
		cmd &= 0x7F;
		if ((res=sdcard_spi_send_cmd(sd_ctrl, CMD55, 0)) > 1) {
			return res;
		}
	}

	/* Select the card and wait for ready except to stop multiple block read */
	if (cmd != CMD12) {
		sdcard_spi_deselect(sd_ctrl);
		if (sdcard_spi_select(sd_ctrl) != E_OK) return 0xFF;
	}

	/* Send command packet */
	buf[0] = (uint8_t)(0x40 | cmd);	/* Start + command index */
	buf[1] = (uint8_t)(arg >> 24);	/* Argument[31..24] */
	buf[2] = (uint8_t)(arg >> 16);	/* Argument[23..16] */
	buf[3] = (uint8_t)(arg >> 8);	/* Argument[15..8] */
	buf[4] = (uint8_t)arg;		/* Argument[7..0] */
	buf[5] = 0x01;			/* Dummy CRC + Stop */
	if (cmd == CMD0) buf[5] = 0x95;	/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) buf[5] = 0x87;	/* Valid CRC for CMD8(0x1AA) */

	/* Receive command resp */
	if (cmd == CMD12) {
		buf[6] = 0xff;
		cnt = 7;
	} else {
		cnt = 6;
	}

#ifdef SDCARD_SPI_CPULOCK_ENABLE
	cpu_status = cpu_lock_save();
#endif
	spi->spi_write((const void *)buf, cnt); /* Write Command Data */

	cnt = 10;		/* Wait for response (10 bytes max) */
	do {
		spi->spi_read((void *)(&res), 1);
	} while ((res & 0x80) && (--cnt));

#ifdef SDCARD_SPI_CPULOCK_ENABLE
	cpu_unlock_status(cpu_status);
#endif
	return res;   /* Return received response */
}

int32_t sdcard_spi_diskio_initialize(FATFS_DISKIO_PTR diskio)
{
#ifdef SDCARD_SPI_CPULOCK_ENABLE
	uint32_t  cpu_status;
#endif
	int32_t ercd = E_OK;
	FS_SDCARD_SPI_CTRL_PTR sd_ctrl;
	uint8_t n, cmd, ty, ocr[4];
	uint8_t buf[20];
	uint64_t cur_time;
	DEV_SPI_PTR spi;


	SDCARD_SPI_CHECK_EXP(diskio!=NULL, STA_NOINIT);
	SDCARD_SPI_CHECK_EXP(diskio->diskio_ctrl!=NULL, STA_NOINIT);

	sd_ctrl = (FS_SDCARD_SPI_CTRL_PTR)diskio->diskio_ctrl;
	spi = spi_get_dev(sd_ctrl->spi_master);

	/* Initialize SPI */
	ercd = spi->spi_open(DEV_MASTER_MODE, sd_ctrl->init_freq);
	if (ercd != E_OK && ercd != E_OPNED) {
		sd_ctrl->drv_status = STA_NOINIT;
		ercd = STA_NOINIT;
		goto error_exit;
	}

	if (spi->spi_control(SPI_CMD_SET_CLK_MODE, CONV2VOID((uint32_t)sd_ctrl->clk_mode)) != E_OK) {
		sd_ctrl->drv_status = STA_NOINIT;
		ercd = STA_NOINIT;
		goto error_exit;
	}
	if (sd_ctrl->drv_status & STA_NODISK) { /* Is card existing in the socket? */
		ercd = STA_NODISK;
		goto error_exit;
	}

	/** change working freq to slow freq to prepare for init */
	spi->spi_control(SPI_CMD_MST_SET_FREQ, CONV2VOID(sd_ctrl->init_freq));

	sdcard_spi_select(sd_ctrl);

#ifdef SDCARD_SPI_CPULOCK_ENABLE
	cpu_status = cpu_lock_save();
#endif
	for (n = 0; n < 10; n++) { /* Send 80 dummy clocks */
		buf[n] = 0xff;
	}
	spi->spi_write((const void *)buf, n);

	ty = 0;
	if (sdcard_spi_send_cmd(sd_ctrl, CMD0, 0) == 1) {	/* Put the card SPI/Idle state */
		cur_time = sd_ctrl->ms_update;			/* Initialization timeout = 1 sec */
		if (sdcard_spi_send_cmd(sd_ctrl, CMD8, 0x1AA) == 1) {	/* SDv2? */
			spi->spi_read((void *)ocr, 4);	/* Get 32 bit return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {	/* Is the card supports vcc of 2.7-3.6V? */
				DBG("Card Supports the Operating Voltage\n");
				/* Wait for end of initialization with ACMD41(HCS) */
				while ( ((sd_ctrl->ms_update - cur_time) < 1000) \
					&& sdcard_spi_send_cmd(sd_ctrl, ACMD41, 1UL << 30) );
				if ( ((sd_ctrl->ms_update - cur_time) < 1000) \
					&& sdcard_spi_send_cmd(sd_ctrl, CMD58, 0) == 0) { /* Check CCS bit in the OCR */
					spi->spi_read((void *)ocr, 4);
					DBG("CCS Bit: %x %x %x %x\n\r", ocr[0], ocr[1], ocr[2], ocr[3]);
					ty = (ocr[0] & 0x40) ? SDCARD_CT_SD2 | SDCARD_CT_BLOCK : SDCARD_CT_SD2;	/* Card id SDv2 */
				}
			} else {
				DBG("Card don't Supports the Operating Voltage\n");
			}
		} else {	/* Not SDv2 card */
			DBG("Not SDv2 Card\n");
			if (sdcard_spi_send_cmd(sd_ctrl, ACMD41, 0) <= 1) { /* SDv1 or MMC? */
				ty = SDCARD_CT_SD1; cmd = ACMD41;	/* SDv1 (ACMD41(0)) */
			} else {
				ty = SDCARD_CT_MMC; cmd = CMD1;	/* MMCv3 (CMD1(0)) */
			}
			/* Wait for end of initialization */
			while (((sd_ctrl->ms_update - cur_time) < 1000) \
				&& sdcard_spi_send_cmd(sd_ctrl, cmd, 0)) ;

			if (((sd_ctrl->ms_update - cur_time) >= 1000) \
				|| sdcard_spi_send_cmd(sd_ctrl, CMD16, 512) != 0) {
				/* Set block length: 512 */
				ty = 0;
			}
		}
	}
	sd_ctrl->card_type = ty; /* Card type */
#ifdef SDCARD_SPI_CPULOCK_ENABLE
	cpu_unlock_status(cpu_status);
#endif
	sdcard_spi_deselect(sd_ctrl);

	if (ty) { /* OK */
		 /* Set fast clock */
		spi->spi_control(SPI_CMD_MST_SET_FREQ, CONV2VOID(sd_ctrl->working_freq));
		sd_ctrl->drv_status &= ~STA_NOINIT;	/* Clear STA_NOINIT flag */
	} else { /* Failed */
		sd_ctrl->drv_status = STA_NOINIT;
	}
	DBG("CardType:%d, Status:%d\n", sd_ctrl->card_type, sd_ctrl->drv_status);
	ercd = sd_ctrl->drv_status;

error_exit:
	return ercd;
}

int32_t sdcard_spi_diskio_status(FATFS_DISKIO_PTR diskio)
{
	int32_t ercd = E_OK;
	FS_SDCARD_SPI_CTRL_PTR sd_ctrl;

	SDCARD_SPI_CHECK_EXP(diskio!=NULL, STA_NOINIT);
	SDCARD_SPI_CHECK_EXP(diskio->diskio_ctrl!=NULL, STA_NOINIT);

	sd_ctrl = (FS_SDCARD_SPI_CTRL_PTR)diskio->diskio_ctrl;
	ercd = sd_ctrl->drv_status;

error_exit:
	return ercd;
}

int32_t sdcard_spi_diskio_read(FATFS_DISKIO_PTR diskio, void *buf, uint32_t sector, uint32_t count)
{
	int32_t ercd = RES_OK;
	FS_SDCARD_SPI_CTRL_PTR sd_ctrl;

	/* Check parameter */
	SDCARD_SPI_CHECK_EXP(diskio!=NULL, RES_PARERR);
	SDCARD_SPI_CHECK_EXP(diskio->diskio_ctrl!=NULL, RES_PARERR);
	SDCARD_SPI_CHECK_EXP(buf!=NULL, RES_PARERR);
	SDCARD_SPI_CHECK_EXP(count>0, RES_PARERR);

	sd_ctrl = (FS_SDCARD_SPI_CTRL_PTR)diskio->diskio_ctrl;

	/* Check if drive is ready */
	SDCARD_SPI_CHECK_EXP(!(sd_ctrl->drv_status & STA_NOINIT), RES_NOTRDY);

	/* LBA ot BA conversion (byte addressing cards) */
	if (!(sd_ctrl->card_type & SDCARD_CT_BLOCK)) {
		sector *= 512;
	}

	if (count == 1) { /* Single sector read */
		if ( (sdcard_spi_send_cmd(sd_ctrl, CMD17, sector) == 0) \
			&& (sdcard_spi_read_block(sd_ctrl, buf, 512) == E_OK) ) {
			count = 0;
		}
	} else { /* READ_MULTIPLE_BLOCK */
		if (sdcard_spi_send_cmd(sd_ctrl, CMD18, sector) == 0) {
			do {
				if (sdcard_spi_read_block(sd_ctrl, buf, 512) != E_OK) {
					break;
				}
				buf += 512;
			} while (--count);
			if (sdcard_spi_send_cmd(sd_ctrl, CMD12, 0) != 0) { /* STOP_TRANSMISSION */
				count = 0;
			}
		}
	}
	sdcard_spi_deselect(sd_ctrl);

	ercd = count ? RES_ERROR : RES_OK;  /* Return result */

error_exit:
	return ercd;
}

int32_t sdcard_spi_diskio_write(FATFS_DISKIO_PTR diskio, const void *buf, uint32_t sector, uint32_t count)
{
	int32_t ercd = RES_OK;
	int8_t multi_block_writeready = 1;
	FS_SDCARD_SPI_CTRL_PTR sd_ctrl;

	/* Check parameter */
	SDCARD_SPI_CHECK_EXP(diskio!=NULL, RES_PARERR);
	SDCARD_SPI_CHECK_EXP(diskio->diskio_ctrl!=NULL, RES_PARERR);
	SDCARD_SPI_CHECK_EXP(buf!=NULL, RES_PARERR);
	SDCARD_SPI_CHECK_EXP(count>0, RES_PARERR);

	sd_ctrl = (FS_SDCARD_SPI_CTRL_PTR)diskio->diskio_ctrl;

	/* Check if drive is ready */
	SDCARD_SPI_CHECK_EXP(!(sd_ctrl->drv_status & STA_NOINIT), RES_NOTRDY);
	/* Check write protect */
	SDCARD_SPI_CHECK_EXP(!(sd_ctrl->drv_status & STA_PROTECT), RES_WRPRT);

	/* LBA ot BA conversion (byte addressing cards) */
	if (!(sd_ctrl->card_type & SDCARD_CT_BLOCK)) {
		sector *= 512;
	}

	if (count == 1) { /* Single sector write */
		if ( (sdcard_spi_send_cmd(sd_ctrl, CMD24, sector) == 0) \
			&& (sdcard_spi_write_block(sd_ctrl, (const uint8_t *)buf, 512, 0xFE) == E_OK) ) {
			count = 0;
		}
	} else { /* Multiple sector write */
		if (sd_ctrl->card_type & SDCARD_CT_SDC) {
			if (sdcard_spi_send_cmd(sd_ctrl, ACMD23, count) != 0) {
				multi_block_writeready = 0;
			} else {
				multi_block_writeready = 1;
			}
		}
		if (multi_block_writeready & (sdcard_spi_send_cmd(sd_ctrl, CMD25, sector) == 0)) {
			do {
				if (sdcard_spi_write_block(sd_ctrl, (const uint8_t *)buf, 512, 0xFC) != E_OK) {
					break;
				}
				buf += 512;
			} while (--count);
			if (sdcard_spi_write_block(sd_ctrl, (const uint8_t *)buf, 512, 0xFD) != E_OK) {
				count = 1;
			}
		}
	}
	sdcard_spi_deselect(sd_ctrl);

	ercd = count ? RES_ERROR : RES_OK;  /* Return result */

error_exit:
	return ercd;
}

int32_t sdcard_spi_diskio_ioctl(FATFS_DISKIO_PTR diskio, uint32_t cmd, void *buf)
{
	int32_t ercd = RES_OK;
	uint8_t n, csd[16], *ptr = buf, temp;
	uint32_t csize;
	FS_SDCARD_SPI_CTRL_PTR sd_ctrl;
	DEV_SPI_PTR spi;

	/* Check parameter */
	SDCARD_SPI_CHECK_EXP(diskio!=NULL, RES_PARERR);
	SDCARD_SPI_CHECK_EXP(diskio->diskio_ctrl!=NULL, RES_PARERR);

	sd_ctrl = (FS_SDCARD_SPI_CTRL_PTR)diskio->diskio_ctrl;

	/* Check if drive is ready */
	SDCARD_SPI_CHECK_EXP(!(sd_ctrl->drv_status & STA_NOINIT), RES_NOTRDY);

	spi =  spi_get_dev(sd_ctrl->spi_master);
	ercd = RES_ERROR;
	switch (cmd) {
		case CTRL_SYNC : /* Wait for end of internal write process of the drive */
			if (sdcard_spi_select(sd_ctrl) == E_OK) {
				ercd = RES_OK;
			}
			break;

		case GET_SECTOR_COUNT : /* Get drive capacity in unit of sector (DWORD) */
			if ((sdcard_spi_send_cmd(sd_ctrl, CMD9, 0) == 0) \
					&& (sdcard_spi_read_block(sd_ctrl, csd, 16) == E_OK) ) {
				if ((csd[0] >> 6) == 1) { /* SDC ver 2.00 */
					csize = csd[9] + ((uint32_t)csd[8] << 8) + ((uint32_t)(csd[7] & 63) << 16) + 1;
					*(DWORD *)buf = csize << 10;
				} else { /* SDC ver 1.XX or MMC ver 3 */
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					csize = (csd[8] >> 6) + ((uint32_t)csd[7] << 2) + ((uint32_t)(csd[6] & 3) << 10) + 1;
					*(DWORD *)buf = csize << (n - 9);
				}
				ercd = RES_OK;
			}
			break;

		case GET_BLOCK_SIZE : /* Get erase block size in unit of sector (DWORD) */
			if (sd_ctrl->card_type & SDCARD_CT_SD2) { /* SDC ver 2.00 */
				if (sdcard_spi_send_cmd(sd_ctrl, ACMD13, 0) == 0) { /* Read SD status */
					spi->spi_read((void *)csd, 1);
					if (sdcard_spi_read_block(sd_ctrl, csd, 16) == E_OK) { /* Read partial block */
						for (n = 64 - 16; n; n--) { /* Purge trailing data */
							spi->spi_read((void *)(&temp), 1);
						}
						*(DWORD *)buf = 16UL << (csd[10] >> 4);
						ercd = RES_OK;
					}
				}
			} else { /* SDC ver 1.XX or MMC */
				if ((sdcard_spi_send_cmd(sd_ctrl, CMD9, 0) == 0) \
					&& (sdcard_spi_read_block(sd_ctrl, csd, 16) == E_OK) ) { /* Read CSD */
					if (sd_ctrl->card_type & SDCARD_CT_SD1) { /* SDC ver 1.XX */
						*(DWORD *)buf = (((csd[10] & 63) << 1) + ((uint32_t)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
					} else { /* MMC */
						*(DWORD *)buf = ((uint32_t)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
					}
					ercd = RES_OK;
				}
			}
			break;


		case MMC_GET_TYPE : /* Get card type flags (1 byte) */
			*ptr = (uint8_t)(sd_ctrl->card_type);
			ercd = RES_OK;
			break;

		case MMC_GET_CSD : /* Receive CSD as a data block (16 bytes) */
			if ( (sdcard_spi_send_cmd(sd_ctrl, CMD9, 0) == 0) \
				&& (sdcard_spi_read_block(sd_ctrl, ptr, 16) == E_OK) ) { /* READ_CSD */
				ercd = RES_OK;
			}
			break;

		case MMC_GET_CID : /* Receive CID as a data block (16 bytes) */
			if ( (sdcard_spi_send_cmd(sd_ctrl, CMD10, 0) == 0) \
					&& (sdcard_spi_read_block(sd_ctrl, ptr, 16) == E_OK)) { /* READ_CID */
				ercd = RES_OK;
			}
			break;

		case MMC_GET_OCR : /* Receive OCR as an R3 resp (4 bytes) */
			if (sdcard_spi_send_cmd(sd_ctrl, CMD58, 0) == 0) { /* READ_OCR */
				spi->spi_read((void *)ptr, 4);
				ercd = RES_OK;
			}
			break;

		case MMC_GET_SDSTAT : /* Receive SD status as a data block (64 bytes) */
			if (sdcard_spi_send_cmd(sd_ctrl, ACMD13, 0) == 0) { /* SD_STATUS */
				spi->spi_read((void *)(&temp), 1);
				if (sdcard_spi_read_block(sd_ctrl, ptr, 64) == E_OK) {
					ercd = RES_OK;
				}
			}
			break;

		default:
			ercd = RES_PARERR;
			break;
	}
	sdcard_spi_deselect(sd_ctrl);

error_exit:
	return ercd;
}

void sdcard_spi_diskio_timerproc(FATFS_DISKIO_PTR diskio)
{
	uint64_t ercd = E_OK;
	FS_SDCARD_SPI_CTRL_PTR sd_ctrl;

	/* Check parameter */
	SDCARD_SPI_CHECK_EXP(diskio!=NULL, 0);
	SDCARD_SPI_CHECK_EXP(diskio->diskio_ctrl!=NULL, 0);

	sd_ctrl = (FS_SDCARD_SPI_CTRL_PTR)diskio->diskio_ctrl;

	sd_ctrl->ms_update ++;

	/* Write protected check */
	if (sd_ctrl->card_is_writeprotect()) {
		sd_ctrl->drv_status |= STA_PROTECT;
	} else {
		sd_ctrl->drv_status &= ~STA_PROTECT;
	}

	/* Card in socket check */
	if (sd_ctrl->card_is_inserted()) {
		sd_ctrl->drv_status &= ~STA_NODISK;
	} else {
		sd_ctrl->drv_status |= (STA_NODISK | STA_NOINIT);
	}

error_exit:
	return ;
}

/** @} */ /* MID_FS_FATFS_MMCSPI */
