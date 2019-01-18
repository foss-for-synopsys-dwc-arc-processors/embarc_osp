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

#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC_debug.h"

#include "board.h"
#include "dev_sdio.h"
#include "ff_sdcard_sdio.h"


static int32_t mmc_send_op_cond(FS_SDCARD_SDIO_CTRL_PTR sd_dev)
{
	int32_t ret;
	int32_t timeout = 1000;
	SDIO_CMD  cmd;
	DEV_SDIO_PTR sd_host = (DEV_SDIO_PTR) sd_dev->host;

	cmd.card = sd_dev->id;
	while (1) {
		/* 0xff8000 is supported vdd voltage windows */
		cmd.cmdidx = CMD1;
		cmd.resp_type = SDIO_RSP_R1;
		cmd.arg = 0xFF8000;

		ret = sd_host->cmd_poll(&cmd, NULL);

		if (ret < 0) {
			return ret;
		}

		if (cmd.resp[0] & SDIO_OCR_BUSY) {
			break;
		}

		if (timeout-- <= 0) {
			return E_TMOUT;
		}
	}

	sd_dev->card_info.version = SDCARD_CT_MMC;
	sd_dev->card_info.ocr = cmd.resp[0];

	return E_OK;
}

static int32_t sdio_send_op_cond(FS_SDCARD_SDIO_CTRL_PTR sd_dev)
{
	int32_t ret;
	int32_t timeout = 1000;
	SDIO_CMD  cmd;
	DEV_SDIO_PTR sd_host = (DEV_SDIO_PTR) sd_dev->host;

	cmd.card = sd_dev->id;
	while (1) {
		/* ACMD 41 */
		cmd.cmdidx = CMD55;
		cmd.resp_type = SDIO_RSP_R1;
		cmd.arg = 0;

		ret = sd_host->cmd_poll(&cmd, NULL);

		if (ret < 0) {
			return ret;
		}

		/* 0xff8000 is supported vdd voltage windows */
		cmd.cmdidx = CMD41;
		cmd.resp_type = SDIO_RSP_R3;
		cmd.arg = 0xff8000;

		if (sd_dev->card_info.version == SDCARD_CT_SD2) {
			cmd.arg |= SDIO_OCR_HCS;
		}

		ret = sd_host->cmd_poll(&cmd, NULL);

		if (ret < 0) {
			return ret;
		}

		if (cmd.resp[0] & SDIO_OCR_BUSY) {
			break;
		}

		if (timeout-- <= 0) {
			return E_TMOUT;
		}
	}

	/* MMC will not response to ACMD 41 */
	if (sd_dev->card_info.version != SDCARD_CT_SD2) {
		sd_dev->card_info.version = SDCARD_CT_SD1;
	}

	sd_dev->card_info.ocr = cmd.resp[0];
	sd_dev->card_info.high_capacity = ((cmd.resp[0] & SDIO_OCR_HCS) ==
					SDIO_OCR_HCS);
	sd_dev->card_info.rca = 0;

	return E_OK;
}

static int32_t sdio_send_if_cond(FS_SDCARD_SDIO_CTRL_PTR sd_dev)
{
	SDIO_CMD  cmd;
	DEV_SDIO_PTR sd_host = (DEV_SDIO_PTR) sd_dev->host;
	int32_t ret;

	cmd.card = sd_dev->id;
	cmd.cmdidx = CMD8;
	/* arg is the supported voltage. At most time, it's fixed to 3.3v */
	/* 0xaa is a recommended check pattern */
	cmd.arg = 0x1aa;
	cmd.resp_type= SDIO_RSP_R7;

	ret = sd_host->cmd_poll(&cmd, NULL);

	if (ret < 0) {
		return ret;
	}

	if ((cmd.resp[0] & 0xfff) != 0x1aa) {
		return E_NOSPT;  /* maybe SDv1 or MMCv3 */
	} else {
		/* high capacity possible */
		sd_dev->card_info.version = SDCARD_CT_SD2;
	}

	return E_OK;
}

static int32_t sdio_go_idle(FS_SDCARD_SDIO_CTRL_PTR sd_dev)
{
	DEV_SDIO_PTR sd_host = (DEV_SDIO_PTR) sd_dev->host;

	SDIO_CMD  cmd;

	cmd.card = sd_dev->id;
	cmd.cmdidx = CMD0;
	cmd.arg = 0;
	cmd.resp_type= SDIO_RSP_NONE;

	return sd_host->cmd_poll(&cmd, NULL);
}

static int32_t sdio_send_ssr(FS_SDCARD_SDIO_CTRL_PTR sd_dev)
{
	int32_t ret;
	DEV_SDIO_PTR sd_host = (DEV_SDIO_PTR) sd_dev->host;
	uint32_t ssr_data[16];
	uint8_t *ssr_ptr;
	int32_t timeout = 3;

	SDIO_CMD cmd;
	SDIO_DATA data;

	cmd.card = sd_dev->id;
	cmd.cmdidx = CMD55;
	cmd.resp_type = SDIO_RSP_R1;
	cmd.arg = sd_dev->card_info.rca << 16;

	ret = sd_host->cmd_poll(&cmd, NULL);

	if (ret < 0) {
		return ret;
	}

	cmd.cmdidx = CMD13;
	cmd.resp_type= SDIO_RSP_R1;
	cmd.arg = 0;

retry_ssr:
	data.in = (uint8_t *)ssr_data;
	data.blksz = 64;
	data.blkcnt = 1;
	data.flags = SDIO_DATA_READ;


	ret = sd_host->cmd_poll(&cmd, &data);

	if (ret < 0) {
		if (timeout--) {
			goto retry_ssr;
		}
		return ret;
	}

	ssr_ptr = (uint8_t *)ssr_data;

	sd_dev->card_info.au = 16 << (ssr_ptr[10] >> 4);

	return E_OK;
}

static int32_t sdio_send_status(FS_SDCARD_SDIO_CTRL_PTR sd_dev, uint32_t timeout)
{
	int32_t ret = E_OK;
	uint32_t retries = 3;
	SDIO_CMD cmd;
	DEV_SDIO_PTR sd_host = (DEV_SDIO_PTR) sd_dev->host;
	uint32_t start = OSP_GET_CUR_MS();

	cmd.card = sd_dev->id;
	cmd.cmdidx = CMD13;
	cmd.resp_type = SDIO_RSP_R1;
	cmd.arg = sd_dev->card_info.rca << 16;

	while (1) {
		ret = sd_host->cmd_poll(&cmd, NULL);

		if (ret == E_OK) {
			if ((cmd.resp[0] & SDIO_STATUS_RDY_FOR_DATA) && (
			(cmd.resp[0] & SDIO_STATUS_CURR_STATE) != SDIO_STATUS_STATE_PRG)) {
				break;
			} else if (cmd.resp[0] & SDIO_STATUS_MASK_ERR) {
				DBG("sd card status error:%x\n", cmd.resp[0]);
				return E_SYS;
			}
		} else if (--retries <= 0) {
			return ret;
		}

		if ((OSP_GET_CUR_MS() - start) > timeout) {
			ret = E_TMOUT;
			break;
		}
	}

	return ret;
}

static int32_t sdio_identify_and_standy_by(FS_SDCARD_SDIO_CTRL_PTR sd_dev)
{
	int32_t ret;
	DEV_SDIO_PTR sd_host = (DEV_SDIO_PTR) sd_dev->host;
	uint32_t csize, cmult;
	SDIO_CMD  cmd;
	SDIO_CTRL_CMD ctrl_cmd;

	cmd.card = sd_dev->id;
	/* Put the Card in Identify Mode, SPI: CMD10, SDIO: CMD2 */
	cmd.cmdidx = CMD2;
	cmd.arg = 0;
	cmd.resp_type = SDIO_RSP_R2;

	ret = sd_host->cmd_poll(&cmd, NULL);
	if (ret < 0) {
		return ret;
	}

	sd_dev->card_info.cid[0] = cmd.resp[0];
	sd_dev->card_info.cid[1] = cmd.resp[1];
	sd_dev->card_info.cid[2] = cmd.resp[2];
	sd_dev->card_info.cid[3] = cmd.resp[3];

	/*
	 * For MMC cards, set the Relative Address.
	 * For SD cards, get the Relative Address.
	 * This also puts the cards into Standby State
	 */
	cmd.cmdidx = CMD3;
	cmd.arg = sd_dev->card_info.rca << 16;
	cmd.resp_type = SDIO_RSP_R6;

	ret = sd_host->cmd_poll(&cmd, NULL);
	if (ret < 0) {
		return ret;
	}

	/* For SD cards, get rca from response */
	if (sd_dev->card_info.version & SDCARD_CT_SDC) {
		sd_dev->card_info.rca = (cmd.resp[0] >> 16) & 0xffff;
	}

	/* Get the Card-Specific Data */
	cmd.cmdidx = CMD9;
	cmd.arg = sd_dev->card_info.rca << 16;
	cmd.resp_type = SDIO_RSP_R2;

	ret = sd_host->cmd_poll(&cmd, NULL);
	if (ret < 0) {
		return ret;
	}

	sd_dev->card_info.csd[0] = cmd.resp[0];
	sd_dev->card_info.csd[1] = cmd.resp[1];
	sd_dev->card_info.csd[2] = cmd.resp[2];
	sd_dev->card_info.csd[3] = cmd.resp[3];

	/* \todo: For MMC cards, get the exact version from csd */

	/* get supported trans-speed */
	ret = cmd.resp[0] & 0xff;

	if (ret == 0x32) {
		sd_dev->card_info.trans_speed = 25000000;
	} else if (ret == 0x5a) {
		sd_dev->card_info.trans_speed = 50000000;
	} else {
		DBG("other trans speed:%x\r\n", ret);
		sd_dev->card_info.trans_speed = 25000000;
	}

	/* In CSD V2.0, fixed to 512 bytes */
	/* \to add the hanlde of MMC */
	sd_dev->card_info.read_bl_len = 512;
	sd_dev->card_info.write_bl_len = 512;


	if (sd_dev->card_info.high_capacity) {
		csize = (cmd.resp[1] & 0x3f) << 16
			| (cmd.resp[2] & 0xffff0000) >> 16;
		cmult = 8;
	} else {
		csize = (cmd.resp[1] & 0x3ff) << 2
			| (cmd.resp[2] & 0xc0000000) >> 30;
		cmult = (cmd.resp[2] & 0x00038000) >> 15;
	}

	sd_dev->card_info.c_size =  (csize + 1) << (cmult + 2);
	sd_dev->card_info.c_size *= sd_dev->card_info.read_bl_len;

	/* CMD4 is optional, not implemented now */

	/* Select the card, and put it into transfer state */
	cmd.cmdidx = CMD7;
	cmd.arg = sd_dev->card_info.rca << 16;
	cmd.resp_type = SDIO_RSP_R1;

	ret = sd_host->cmd_poll(&cmd, NULL);
	if (ret < 0) {
		return ret;
	}

	/* set block len to 512 bytes */
	if (sd_dev->card_info.high_capacity == 0) {
		cmd.cmdidx = CMD16;
		cmd.arg = sd_dev->card_info.read_bl_len;
		cmd.resp_type = SDIO_RSP_R1;

		ret = sd_host->cmd_poll(&cmd, NULL);
		if (ret < 0) {
			return ret;
		}
	}

	if (sd_dev->card_info.version & SDCARD_CT_SDC) {
		/* ACMD6, set bus width to 4 bit */
		cmd.cmdidx = CMD55;
		cmd.resp_type = SDIO_RSP_R1;
		cmd.arg = sd_dev->card_info.rca << 16;

		ret = sd_host->cmd_poll(&cmd, NULL);
		if (ret < 0) {
			return ret;
		}

		cmd.cmdidx = CMD6;
		cmd.resp_type = SDIO_RSP_R1;
		cmd.arg = 2;

		ret = sd_host->cmd_poll(&cmd, NULL);
		if (ret < 0) {
			return ret;
		}

		ctrl_cmd.card = sd_dev->id;
		ctrl_cmd.cmd = SDIO_CMD_SET_BUS_WIDTH;
		ret = sd_host->control(&ctrl_cmd, (void *) 4);

		if (ret < 0) {
			return ret;
		}

		ret = sdio_send_ssr(sd_dev);

		if (ret < 0) {
			return ret;
		}
	}

	ctrl_cmd.card = sd_dev->id;
	ctrl_cmd.cmd = SDIO_CMD_SET_BUS_FREQ;
	sd_host->control(&ctrl_cmd, (void *) sd_dev->card_info.trans_speed);

	return E_OK;
}

static int32_t sdio_read_blocks(FS_SDCARD_SDIO_CTRL_PTR sd_dev, void *buf, uint32_t start, uint32_t blkcnt)
{
	DEV_SDIO_PTR sd_host = (DEV_SDIO_PTR) sd_dev->host;
	int32_t ret;

	SDIO_CMD cmd;
	SDIO_DATA data;

	if (blkcnt > 1) {
		cmd.cmdidx = CMD18;
	} else {
		cmd.cmdidx = CMD17;
	}

	if (sd_dev->card_info.high_capacity) {
		cmd.arg = start;
	} else {
		cmd.arg = start * sd_dev->card_info.read_bl_len;
	}

	cmd.card = sd_dev->id;
	cmd.resp_type= SDIO_RSP_R1;

	data.in = buf;
	data.blkcnt = blkcnt;
	data.blksz = sd_dev->card_info.read_bl_len;
	data.flags = SDIO_DATA_READ;

	ret = sd_host->cmd_poll(&cmd, &data);
	if (ret < 0) {
		return ret;
	}

	if (blkcnt > 1) {
		cmd.cmdidx = CMD12;
		cmd.arg = 0;
		cmd.resp_type= SDIO_RSP_R1b;

		ret = sd_host->cmd_poll(&cmd, NULL);
		if (ret < 0 ) {
			DBG("fail to send stop cmd\r\n");
			return ret;
		}
	}

	return blkcnt;
}

static int32_t sdio_write_blocks(FS_SDCARD_SDIO_CTRL_PTR sd_dev, const void *buf, uint32_t start, uint32_t blkcnt)
{
	DEV_SDIO_PTR sd_host = (DEV_SDIO_PTR) sd_dev->host;
	int32_t ret;

	SDIO_CMD cmd;
	SDIO_DATA data;

	cmd.card = sd_dev->id;
	if (!sd_dev->card_info.high_capacity) {
		start *= sd_dev->card_info.write_bl_len;
	}

	if (blkcnt == 1) {
		cmd.cmdidx = CMD24;
	} else if (blkcnt > 1) {
		/* send acmd 23 before cmd25 */
		if (sd_dev->card_info.version & SDCARD_CT_SDC) {
			cmd.cmdidx = CMD55;
			cmd.resp_type = SDIO_RSP_R1;
			cmd.arg = sd_dev->card_info.rca << 16;;

			ret = sd_host->cmd_poll(&cmd, NULL);

			if (ret < 0) {
				return ret;
			}
		}

		cmd.cmdidx = CMD23;
		cmd.resp_type = SDIO_RSP_R1;
		cmd.arg = blkcnt;

		ret = sd_host->cmd_poll(&cmd, NULL);
		if (ret < 0) {
			return ret;
		}
		cmd.cmdidx = CMD25;
	}

	cmd.arg = start;
	cmd.resp_type= SDIO_RSP_R1;

	data.out = buf;
	data.blkcnt = blkcnt;
	data.blksz = sd_dev->card_info.write_bl_len;
	data.flags = SDIO_DATA_WRITE;

	ret = sd_host->cmd_poll(&cmd, &data);
	if (ret < 0) {
		return ret;
	}

	if (blkcnt > 1) {
		cmd.cmdidx = CMD12;
		cmd.arg = 0;
		cmd.resp_type= SDIO_RSP_R1b;

		ret = sd_host->cmd_poll(&cmd, NULL);
		if (ret < 0 ) {
			DBG("fail to send stop cmd\r\n");
			return ret;
		}
	}


	return blkcnt;
}

static int32_t sdio_erase_blocks(FS_SDCARD_SDIO_CTRL_PTR sd_dev, uint32_t start, uint32_t end)
{
	int32_t ret;
	SDIO_CMD cmd;
	DEV_SDIO_PTR sd_host = (DEV_SDIO_PTR) sd_dev->host;

	if (sd_dev->card_info.high_capacity == 0) {
		start *= sd_dev->card_info.read_bl_len;
		end *= sd_dev->card_info.read_bl_len;
	}

	cmd.card = sd_dev->id;
	cmd.cmdidx = CMD32;
	cmd.arg = start;
	cmd.resp_type = SDIO_RSP_R1;

	ret = sd_host->cmd_poll(&cmd, NULL);

	if (ret < 0) {
		return ret;
	}

	cmd.cmdidx = CMD33;
	cmd.arg = end;
	cmd.resp_type = SDIO_RSP_R1;

	ret = sd_host->cmd_poll(&cmd, NULL);

	if (ret < 0) {
		return ret;
	}

	cmd.cmdidx = CMD38;
	cmd.arg = 0;
	cmd.resp_type = SDIO_RSP_R1;

	ret = sd_host->cmd_poll(&cmd, NULL);

	if (ret < 0) {
		return ret;
	}

	ret = sdio_send_status(sd_dev, 500);

	return ret;
}

int32_t sdcard_sdio_diskio_initialize(FATFS_DISKIO *diskio)
{
	int32_t ret;
	FS_SDCARD_SDIO_CTRL_PTR sd_dev =  (FS_SDCARD_SDIO_CTRL_PTR) diskio->diskio_ctrl;
	DEV_SDIO_PTR sd_host = (DEV_SDIO_PTR) sd_dev->host;
	uint8_t id;

	sd_dev->drv_status = STA_NOINIT;

	id = sd_dev->id;

	/* open and power on sdio host */
	sd_host->open(id);

	if (!sd_host->cd(id)) {
		/* no card exist */
		DBG("no sdcard detect\r\n");
		return STA_NOINIT;
	}
	/*---- Card is 'idle' state ----*/
	sdio_go_idle(sd_dev);

	/* MMC and SDv2 may not response to CMD8
	 * so no matter how, go ahead */
	sdio_send_if_cond(sd_dev);

	ret = sdio_send_op_cond(sd_dev);

	if (ret == E_TMOUT) {
		/* mmc card */
		ret = mmc_send_op_cond(sd_dev);
		if (ret < 0) {
			return STA_NOINIT;
		}
	}

	ret = sdio_identify_and_standy_by(sd_dev);

	if (ret < 0) {
		return STA_NOINIT;
	}

	sd_dev->drv_status &= ~STA_NOINIT;

	return RES_OK;
}

int32_t sdcard_sdio_diskio_status(FATFS_DISKIO *diskio)
{
	FS_SDCARD_SDIO_CTRL_PTR sd_dev =  (FS_SDCARD_SDIO_CTRL_PTR) diskio->diskio_ctrl;
	DEV_SDIO_PTR sd_host = (DEV_SDIO_PTR) sd_dev->host;

	if (sd_host->cd(sd_dev->id)) {
		sd_dev->drv_status &= ~STA_NODISK;
	} else {
		sd_dev->drv_status |= STA_NODISK;
	}

	if (sd_host->wp(sd_dev->id)) {
		sd_dev->drv_status |= STA_PROTECT;
	} else {
		sd_dev->drv_status &= ~STA_PROTECT;
	}

	return sd_dev->drv_status;
}

int32_t sdcard_sdio_diskio_read(FATFS_DISKIO *diskio, void *buf, uint32_t sector, uint32_t count)
{
	FS_SDCARD_SDIO_CTRL_PTR sd_dev =  (FS_SDCARD_SDIO_CTRL_PTR) diskio->diskio_ctrl;

	if (sdio_read_blocks(sd_dev, buf, sector, count) == count) {
		return RES_OK;
	}

	return RES_ERROR;

}
int32_t sdcard_sdio_diskio_write(FATFS_DISKIO *diskio, const void *buf, uint32_t sector, uint32_t count)
{
	FS_SDCARD_SDIO_CTRL_PTR sd_dev =  (FS_SDCARD_SDIO_CTRL_PTR) diskio->diskio_ctrl;

	if (sdio_write_blocks(sd_dev, buf, sector, count) == count) {
		return RES_OK;
	}

	return RES_ERROR;
}

int32_t sdcard_sdio_diskio_ioctl(FATFS_DISKIO *diskio, uint32_t cmd, void *buf)
{
	FS_SDCARD_SDIO_CTRL_PTR sd_dev =  (FS_SDCARD_SDIO_CTRL_PTR) diskio->diskio_ctrl;
	int32_t ret = RES_OK;

	switch (cmd) {
		case CTRL_SYNC:
			if (sdio_send_status(sd_dev, 500) != E_OK) {
				ret = RES_ERROR;
			}
			break;
		case GET_SECTOR_COUNT:
			*(uint32_t *)buf = sd_dev->card_info.c_size / sd_dev->card_info.read_bl_len;
			break;
		case GET_BLOCK_SIZE:
			*(uint32_t *) buf = sd_dev->card_info.au;
			break;
		case GET_SECTOR_SIZE:
			*(uint32_t *)buf = sd_dev->card_info.read_bl_len;
			break;
		default:
			ret =  RES_PARERR;
			break;
	}
	return ret;
}