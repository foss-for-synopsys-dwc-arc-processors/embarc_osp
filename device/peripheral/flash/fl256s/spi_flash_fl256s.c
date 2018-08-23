/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

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
#include "spi_flash_fl256s.h"

//#define DBG_MORE
#include "embARC_debug.h"


#define SF_READ_ID			0x9F
#define SF_READ_STS			0x05
#define SF_WRITE_ENABLE		0x06
#define SF_WRITE_DISABLE	0x04
#define SF_SECTOR_ERASE		0xD8
#define SF_PAGE_WRITE		0x02 //0x32  //Quad Page Program (TODO: unsupported)
#define SF_PAGE_READ		0x03 //0x6B  //Quad Page Read (TODO: unsupported)

#define FL256S_NOT_VALID	(0xFFFFFFFF)

Inline int32_t _spi_send_cmd(FL256S_DEF_PTR dev, DEV_SPI_TRANSFER *xfer){
	uint32_t cpu_status;
	DEV_SPI_PTR spi_flash;
	int32_t ercd = 0;

	spi_flash = spi_get_dev(dev->spi_master);

	cpu_status = cpu_lock_save();

	/* select device */
	spi_flash->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID((uint32_t)dev->cs));
	spi_flash->spi_control(SPI_CMD_MST_SET_FREQ, CONV2VOID(dev->spi_freq));
	spi_flash->spi_control(SPI_CMD_SET_CLK_MODE, CONV2VOID(SPI_CLK_MODE_0));
	ercd = spi_flash->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(xfer));
	/* deselect device */
	spi_flash->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID((uint32_t)dev->cs));

	cpu_unlock_restore(cpu_status);
	return ercd;
}

uint32_t fl256s_read_status(FL256S_DEF_PTR dev){

	uint8_t local_buf[2];
	DEV_SPI_TRANSFER cmd_xfer;

	local_buf[0] = SF_READ_STS;

	DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 1);
	DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, local_buf, 0, 2);
	DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

	if(_spi_send_cmd(dev, &cmd_xfer) == 0) {
		dbg_printf(DBG_MORE_INFO, "fl256s_read_status buf: 0x%x, 0x%x\r\n", local_buf[0], local_buf[1]);
		return local_buf[1];
	} else {
		dbg_printf(DBG_MORE_INFO, "fl256s_read_status buf: 0x%x, 0x%x\r\n", local_buf[0], local_buf[1]);
		return FL256S_NOT_VALID;
	}
}

uint32_t fl256s_read_id(FL256S_DEF_PTR dev){
	uint32_t id = 0;
	uint8_t local_buf[5];
	DEV_SPI_TRANSFER cmd_xfer;

	local_buf[0] = SF_READ_ID;

	DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 1);
	DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, local_buf, 0, 5);
	DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

	if(_spi_send_cmd(dev, &cmd_xfer) == 0) {
		id = (local_buf[1] << 24) | (local_buf[2] << 16) | (local_buf[3] << 8) | local_buf[4];
	} else {
		id = FL256S_NOT_VALID;
	}
	dbg_printf(DBG_MORE_INFO, "fl256s_read_id buf: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\r\n", local_buf[0], local_buf[1], local_buf[2], local_buf[3], local_buf[4]);

	return id;
}

int32_t fl256s_wait_ready(FL256S_DEF_PTR dev){
	uint32_t status = 0x01;
	do {
		status = fl256s_read_status(dev);

		if(status == FL256S_NOT_VALID) {
			return -1;
		}
	} while (status & 0x01);//Status Register 1 Bit 0

	return 0;
}

int32_t fl256s_init(FL256S_DEF_PTR dev, uint32_t freq){
	DEV_SPI_PTR spi_flash;
	int32_t ercd = 0;

	dev->spi_freq = freq;
	spi_flash = spi_get_dev(dev->spi_master);

	ercd = spi_flash->spi_open(DEV_MASTER_MODE, freq);

	if(ercd != E_OK && ercd != E_OPNED) {
		return ercd;
	}

	//spi_flash->spi_control(SPI_CMD_SET_DUMMY_DATA, CONV2VOID(0xFF));
	ercd = fl256s_wait_ready(dev);
	dbg_printf(DBG_MORE_INFO, "fl256s_wait_ready ret %d\r\n", ercd);
	return E_OK;
}

int32_t fl256s_write_enable(FL256S_DEF_PTR dev){
	uint8_t local_buf[3];
	DEV_SPI_TRANSFER cmd_xfer;

	uint32_t status = 0;
	do {
		local_buf[0] = SF_WRITE_ENABLE;

		DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 1);
		DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 0, 0);
		DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

		if (_spi_send_cmd(dev, &cmd_xfer) != 0) {
			return -1;
		}

		status = fl256s_read_status(dev);

		dbg_printf(DBG_MORE_INFO, "fl256s_write_enable status = 0x%x\r\n", status);
		if (status == FL256S_NOT_VALID) {
			return -1;
		}
	} while ( !(status & 0x02) );//Status Register 1 Bit 1

	return 0;
}

int32_t fl256s_write_disable(FL256S_DEF_PTR dev){
	uint8_t local_buf[3];
	DEV_SPI_TRANSFER cmd_xfer;

	uint32_t status = 0;
	do {
		local_buf[0] = SF_WRITE_DISABLE;

		DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 1);
		DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 0, 0);
		DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

		if (_spi_send_cmd(dev, &cmd_xfer) != 0) {
			return -1;
		}

		status = fl256s_read_status(dev);

		dbg_printf(DBG_MORE_INFO, "fl256s_write_disable status = 0x%x\r\n", status);
		if (status == FL256S_NOT_VALID) {
			return -1;
		}
	} while (status & 0x02);//Status Register 1 Bit 1

	return 0;

}


int32_t fl256s_erase(FL256S_DEF_PTR dev, uint32_t address, uint32_t size)
{
	uint32_t last_address;
	uint32_t count = 0;
	uint8_t local_buf[4];
	DEV_SPI_TRANSFER cmd_xfer;

	// start address of last sector
	last_address = (address + size) & (~(dev->sector_sz - 1));

	// start address of first sector
	address &= ~(dev->sector_sz - 1);

	do {
		if (fl256s_write_enable(dev) != 0) {
			return -1;
		}

		if (fl256s_wait_ready(dev) != 0) {
			return -1;
		}

		local_buf[0] = SF_SECTOR_ERASE;
		local_buf[1] = (address >> 16) & 0xff;
		local_buf[2] = (address >> 8) & 0xff;
		local_buf[3] =  address & 0xff;

		DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 4);
		DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 0, 0);
		DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

		if (_spi_send_cmd(dev, &cmd_xfer) != 0) {
			return -1;
		}

		address += dev->sector_sz;
		count++;
	} while(address <= last_address);

	if (fl256s_wait_ready(dev) != 0) {
		return -1;
	}
	if (fl256s_write_disable(dev) != 0) {
		return -1;
	}

	return (int32_t)count;
}


int32_t fl256s_write(FL256S_DEF_PTR dev, uint32_t address, uint32_t size, const void *data){

	uint8_t local_buf[4];
	DEV_SPI_TRANSFER cmd_xfer;
	DEV_SPI_TRANSFER data_xfer;

	uint32_t first = 0;
	uint32_t size_orig = size;

	if (fl256s_wait_ready(dev) != 0) {
		return -1;
	}

	first = FLASH_PAGE_SIZE - (address & (FLASH_PAGE_SIZE - 1));

	do {
		// send write enable command to flash
		if (fl256s_write_enable(dev) != 0) {
			return -1;
		}
		if (fl256s_wait_ready(dev) != 0) {
			return -1;
		}

		first = first < size ? first : size;

		local_buf[0] = SF_PAGE_WRITE;
		local_buf[1] = (address >> 16) & 0xff;
		local_buf[2] = (address >> 8) & 0xff;
		local_buf[3] = address  & 0xff;

		DEV_SPI_XFER_SET_TXBUF(&data_xfer, data, 0, first);
		DEV_SPI_XFER_SET_RXBUF(&data_xfer, NULL, 0, 0);
		DEV_SPI_XFER_SET_NEXT(&data_xfer, NULL);

		DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 4);
		DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 0, 0);
		DEV_SPI_XFER_SET_NEXT(&cmd_xfer, &data_xfer);

		if (_spi_send_cmd(dev, &cmd_xfer) != 0) {
			return -1;
		}

		size -= first;
		address += first;
		data += first;
		first = FLASH_PAGE_SIZE;

	} while (size);

	if (fl256s_wait_ready(dev) != 0) {
		return -1;
	}
	if (fl256s_write_disable(dev) != 0) {
		return -1;
	}

	return (int32_t)(size_orig);
}

int32_t fl256s_read(FL256S_DEF_PTR dev, uint32_t address, uint32_t size, void *data){
	uint8_t local_buf[4];
	DEV_SPI_TRANSFER cmd_xfer;
	DEV_SPI_TRANSFER data_xfer;
	uint32_t first = 0;
	uint32_t size_orig = size;

	first = FLASH_PAGE_SIZE - (address & (FLASH_PAGE_SIZE - 1));

	do {
		if (fl256s_wait_ready(dev) != 0) {
			return -1;
		}

		first = first < size ? first : size;

		local_buf[0] = SF_PAGE_READ;
		local_buf[1] = (address >> 16) & 0xff;
		local_buf[2] = (address >> 8) & 0xff;
		local_buf[3] = address  & 0xff;

		DEV_SPI_XFER_SET_TXBUF(&data_xfer, NULL, 0, 0);
		DEV_SPI_XFER_SET_RXBUF(&data_xfer, data, 0, first);
		DEV_SPI_XFER_SET_NEXT(&data_xfer, NULL);

		DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, local_buf, 0, 4);
		DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 0, 0);
		DEV_SPI_XFER_SET_NEXT(&cmd_xfer, &data_xfer);

		if (_spi_send_cmd(dev, &cmd_xfer) != 0) {
			return -1;
		}
		size -= first;
		address += first;
		data += first;
		first = FLASH_PAGE_SIZE;

	} while (size);

	return (int32_t)(size_orig);
}
