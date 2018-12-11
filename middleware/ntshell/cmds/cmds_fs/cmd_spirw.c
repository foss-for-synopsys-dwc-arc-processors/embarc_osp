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
* \brief read and write the program image in the SPI flash
*/


#include "cmds_fs_cfg.h"
#if NTSHELL_USE_CMDS_FS_SPIRW
#include "cmd_fs_common.h"

#include "spi_flash_w25qxx.h"

typedef struct {
	uint32_t head;		/*!< 0x68656164 ='head' */
	uint32_t cpu_type;	/*!< = 0 - all images, reserved for future */
	uint32_t start;		/*!< start address of application image in spi flash */
	uint32_t size;		/*!< size of image in bytes */
	uint32_t ramaddr;	/*!< address of ram for loading image */
	uint32_t ramstart;	/*!< start address of application in RAM !!!! */
	uint32_t checksum;	/*!< checksum of all bytes in image */
} EMSK_FIRMWARE_DEF;


static NTSHELL_IO_PREDEF;

#define ENABLE_HELP 1

/* define the address in the SPI flash */
#define IMAGE_HEADER_LEN	(sizeof(EMSK_FIRMWARE_DEF))
#define BOOTIMAGE_HEAD		(0x68656164)
#if HW_VERSION == 10 || HW_VERSION == 11
#define BOOTIMAGE_START		(0x780000)
#define BOOTIMAGE_END		(0x800000)
#define BOOTIMAGE_MAX_SIZE	(BOOTIMAGE_END - BOOTIMAGE_START - IMAGE_HEADER_LEN)
#define MIN_RAM_START		(0x2000)
#else
#define BOOTIMAGE_START		(0xD80000)
#define BOOTIMAGE_END		(0x1000000)
#define BOOTIMAGE_MAX_SIZE	(BOOTIMAGE_END - BOOTIMAGE_START - IMAGE_HEADER_LEN)
#define MIN_RAM_START		(0x2000)
#endif


W25QXX_DEF(w25q128bv, DW_SPI_0_ID, EMSK_SPI_LINE_SFLASH, 0x100, 0x1000);

Inline unsigned int make_checksum(unsigned int init, unsigned char *buf, unsigned int len)
{
	while (len--) {
		init += *buf++;
	}

	return init;
}


static int32_t write_spi_image(FIL *file, uint32_t address, uint32_t ram_address, uint32_t ram_start)
{
	uint32_t cnt;
	uint32_t sector_addr, res;
	uint32_t num_of_erased_sectors = 0, erased_start = 0;
	EMSK_FIRMWARE_DEF header;

	// check address to preven earasing FPGA images
	if (address < BOOTIMAGE_START) {
		CMD_DEBUG("Wrong boot image address\r\n");
		return 0;
	}

	if (ram_start == 0) {
		CMD_DEBUG("Wrong ram start address\r\n");
		return 0;
	}

	if (ram_address < MIN_RAM_START) {
		CMD_DEBUG("Wrong ram address\r\n");
		return 0;
	}

	// write header
	header.head = BOOTIMAGE_HEAD; // 0x68656164 = 'head'
	header.cpu_type = 0;
	header.start = address + IMAGE_HEADER_LEN;  // image will be placed next address after the header.
	header.size = f_size(file);
	header.ramaddr = ram_address;
	header.ramstart = ram_start;
	header.checksum = 0;

	w25qxx_wait_ready(w25q128bv);

	// erase starting sector for header and first data block
	sector_addr = address & ~(FLASH_SECTOR_SIZE - 1);
	res = w25qxx_erase(w25q128bv,sector_addr, IMAGE_HEADER_LEN + header.size);
	// save statistic
	erased_start = sector_addr;
	num_of_erased_sectors += res;

	sector_addr += FLASH_SECTOR_SIZE * res;
	address += IMAGE_HEADER_LEN;

	CMD_DEBUG("  Erased %d sectors from 0x%x to 0x%x\r\n", num_of_erased_sectors, erased_start, sector_addr-1);


	CMD_DEBUG("\r\n** Writing SPI flash **\r\n");


	CMD_DEBUG("- data\r\n");

	// write data to the flash
	do {
		if (f_read(file, cmd_fs_buffer, CMD_FS_BUF_SIZE, &cnt) != FR_OK) {
			CMD_DEBUG("read file error, already read %d\r\n", cnt);
			return 0;
		}

		header.checksum = make_checksum(header.checksum, cmd_fs_buffer, cnt);

		CMD_DEBUG("write 0x%x - 0x%x check_sum:%x\r\n", address, address+cnt, header.checksum);

		w25qxx_write(w25q128bv, address, cnt, cmd_fs_buffer);
		w25qxx_wait_ready(w25q128bv);

		address += cnt;
	} while (cnt >= CMD_FS_BUF_SIZE);

	w25qxx_write(w25q128bv, BOOTIMAGE_START, IMAGE_HEADER_LEN, &header);
	w25qxx_wait_ready(w25q128bv);

	CMD_DEBUG("  Written %d bytes: header=%d and image=%d image from 0x%x to 0x%x\r\n",IMAGE_HEADER_LEN + header.size,
	          IMAGE_HEADER_LEN, header.size, header.start - IMAGE_HEADER_LEN, address-1);

	CMD_DEBUG("\r\n** Boot image header **\r\n");
	CMD_DEBUG("  head: 0x%x\r\n  cpu: 0x%x\r\n  start: 0x%x\r\n  size: 0x%x\r\n  ram addr: 0x%x\r\n  start: 0x%x\r\n  checksum: 0x%x\r\n",
	          header.head, header.cpu_type, header.start, header.size, header.ramaddr, header.ramstart, header.checksum);

	cnt = header.size + IMAGE_HEADER_LEN;

	return cnt;
}

static int32_t read_raw_spi(FIL *file, uint32_t address, uint32_t size, uint32_t *checksum)
{
	int32_t cnt, datalen, len;
	uint32_t csum;
	uint32_t temp;

	if ( checksum == NULL || file == NULL ) {
		return 0;
	}

	w25qxx_wait_ready(w25q128bv);

	csum = *checksum;

	datalen = 0;

	do {
		cnt = w25qxx_read(w25q128bv, address, CMD_FS_BUF_SIZE, cmd_fs_buffer);

		if (cnt < 0) {
			*checksum = 0;
			return 0;
		}

		len = (size < cnt) ? size : cnt;
		csum = make_checksum(csum, cmd_fs_buffer, len);

		if (f_write(file, cmd_fs_buffer, len, &temp)) {
			*checksum = 0;
			return 0;
		}

		size -= len;
		CMD_DEBUG("  read 0x%x: 0x%x-0x%x, check_sum:%x\r\n", cnt, address, address+len, csum);
		datalen += len;
		address += len;
	} while (size);

	*checksum = csum;

	return datalen;
}


static int32_t read_spi_image(FIL *file)
{
	EMSK_FIRMWARE_DEF header;
	uint32_t datalen;
	uint32_t checksum = 0;

	// read spi header
	datalen = w25qxx_read(w25q128bv, BOOTIMAGE_START, IMAGE_HEADER_LEN, &header);

	CMD_DEBUG("\r\n** Read SPI flash **\r\n");
	CMD_DEBUG("  Read %d bytes of header\r\n", datalen);
	CMD_DEBUG("\r\n** Boot image header **\r\n");
	CMD_DEBUG("  head: 0x%x\r\n  cpu: 0x%x\r\n  start: 0x%x\r\n  size: 0x%x\r\n  ram addr: 0x%x\r\n  start: 0x%x\r\n  checksum: 0x%x\r\n",
	          header.head, header.cpu_type, header.start, header.size, header.ramaddr, header.ramstart, header.checksum);

	if (BOOTIMAGE_HEAD == header.head && \
	    header.start >= BOOTIMAGE_START &&  header.start < BOOTIMAGE_START + BOOTIMAGE_MAX_SIZE && \
	    header.start >= BOOTIMAGE_START &&  header.start < BOOTIMAGE_START + BOOTIMAGE_MAX_SIZE && \
	    header.size > 0  && header.size <= BOOTIMAGE_MAX_SIZE ) {
		CMD_DEBUG("\r\n  Read %d bytes of image from 0x%x to 0x%x\r\n", datalen, header.start, header.start + header.size);
		datalen = read_raw_spi(file, header.start, header.size, &checksum);

		if (checksum != header.checksum) {
			CMD_DEBUG("\r\n  ERROR: Wrong checksum = 0x%x\r\n", checksum);
			datalen = 0;
		} else {
			CMD_DEBUG("\r\n  Image verified, checksum = 0x%x\r\n", checksum);
		}

	} else {
		CMD_DEBUG("\r\n  ERROR: Wrong header parameters \r\n");
		datalen = 0;
	}

	return datalen;
}

/* show help of command spirw */
static void cmd_spirw_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]... <FILE> <RAM_ADD> <START_ADD>\r\n"
		"Read/Write the image in SPI flash, especially for update firmware\r\n"
		"  -h/H/?    Show the help information\r\n"
		"  -i        Read device id\r\n"
		"  -r <file>  Read boot image from spi flash to [file] in sdcard\r\n"
		"  -w <file> <ram_addr> <start_addr>    Read binary [file] in sdcard and write it to spi flash, the image in spi flash will be loaded to [ram_addr] and run from [start_addr] in ddr when board starts up\r\n"
		"Examples: \r\n"
		"  spirw -w em6_2bt.bin 0x17f00000 0x17f00004 Write binary file to spi flash. When board starts up, the image in spi flash will be loaded to 0x17f00000 in ddr, run from 0x17f00004 in ddr\r\n"
		"  spirw -h    Show the help information\r\n", cmd_name);

error_exit:
	return;
}

static int cmd_spirw(int argc, char **argv, void *extobj)
{
	FIL file;
	FRESULT res;

	long ram_addr = 0, ram_start = 0;
	char *file_name = 0;
	uint32_t ID;
	int32_t ercd = E_OK, opt;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	if (argc < 2) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	}

	w25qxx_init(w25q128bv, 1000000);

	opterr = 0;
	optind = 1;

	// parse command line
	while ((opt=getopt(argc, argv, "ir:w:hH?")) != -1) {
		switch (opt) {
			case 'h':
			case '?':
			case 'H':
				cmd_spirw_help(argv[0], extobj);
				goto error_exit;
				break;

			case 'i':
				ID = w25qxx_read_id(w25q128bv);
				CMD_DEBUG("Device ID = %x\r\n", ID);
				break;

			case 'r':
				file_name = optarg;
				CMD_DEBUG("Read spi flash image and write it to the file %s\r\n", file_name);
				res = f_open(&file, file_name, FA_WRITE | FA_CREATE_NEW);

				if (res) {
					ercd = E_SYS;
					CMD_DEBUG("ERROR: File exists in sdcard or for other reasons, error code:0x%x\r\n", res);
					goto error_exit;
				}

				read_spi_image(&file);
				f_close(&file);
				break;

			case 'w':
				if ((argc - optind) >= 2) {
					// read file name
					file_name = optarg;

					res = f_open(&file, file_name, FA_READ | FA_OPEN_EXISTING);

					if (res) {
						CMD_DEBUG("ERROR: Cannot open file, error code:0x%x\r\n", res);
						ercd = E_SYS;
						goto error_exit;
					}

					// read ram address
					if (!xatoi(&argv[optind], &ram_addr)) {
						CMD_DEBUG("ERROR: Wrong ram address format\r\n");
						ercd = E_SYS;
						goto error_exit;
					}

					// read image size
					if (!xatoi(&argv[optind+1], &ram_start)) {
						CMD_DEBUG("ERROR: Wrong ram start address format\r\n");
						ercd = E_SYS;
						goto error_exit;
					}

					optind += 2;

					CMD_DEBUG("** Writing file %s; to address 0x%x start at 0x%x \r\n", file_name, ram_addr, ram_start);
					write_spi_image(&file, BOOTIMAGE_START, ram_addr, ram_start);
					f_close(&file);
				} else {
					CMD_DEBUG("ERROR: Wrong number of arguments for write\r\n");
				}

				break;

			default:
				CMD_DEBUG("%s: unrecognized option:%c\r\n", argv[0], opt);
				CMD_DEBUG("Try '%s -h' for more information\r\n",argv[0]);
				break;
		}
	}

	CMD_DEBUG("SPI Flash Command Done \r\n");
	return E_OK;

error_exit:
	return ercd;
}

static CMD_TABLE_T spirw_cmd = {"spirw", "Read/Write the image in spi flash for updating bootloader/application image", cmd_spirw, NULL};
/**
 * register spirw command
 */
CMD_TABLE_T *register_ntshell_cmd_spirw(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&spirw_cmd, prev);
}
#endif /* NTSHELL_USE_CMDS_FS_SPIRW */
