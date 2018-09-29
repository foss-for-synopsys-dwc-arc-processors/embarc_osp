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
#if NTSHELL_USE_CMDS_FS_MLOAD
#include "cmd_fs_common.h"

/* MCS file format */
#define MCS_START_CODE_SIZE		(1)
#define MCS_BYTE_COUNT_SIZE		(1)
#define MCS_ADDRESS_FIELD_SIZE		(2)
#define MCS_RECORD_TYPE_SIZE		(1)
#define MCS_DATA_FIELD_MAX_SIZE		(0x20)
#define MCS_CHECKSUM_SIZE		(1)
#define MCS_END_OF_LINE_SIZE		(1)

#define MCS_RECORD_BUFFER_SIZE		(MCS_BYTE_COUNT_SIZE+MCS_ADDRESS_FIELD_SIZE+MCS_RECORD_TYPE_SIZE+MCS_DATA_FIELD_MAX_SIZE+MCS_CHECKSUM_SIZE)
#define MCS_HEAD_SIZE			((MCS_BYTE_COUNT_SIZE+MCS_ADDRESS_FIELD_SIZE+MCS_RECORD_TYPE_SIZE)*2)
#define MCS_FIXED_SECTION_SIZE		(MCS_HEAD_SIZE+MCS_CHECKSUM_SIZE*2+MCS_END_OF_LINE_SIZE)
#define MCS_RECORD_LINE_MAX_SIZE	(MCS_FIXED_SECTION_SIZE+MCS_DATA_FIELD_MAX_SIZE*2)

/* the address space of the FPGA image */
#define FPGA_IMAGE_START			(0x000000)
#if HW_VERSION == 20 || HW_VERSION == 21
#define FPGA_IMAGE_SIZE				(0xD70000)
#else
#define FPGA_IMAGE_SIZE				(0x770000)
#endif

static NTSHELL_IO_PREDEF;

/* test mode */
#define MCS_DEST_MODE				(0x01) /* 0x00 to usart, 0x01 to spi flash, 0x02 test mode */
/* define the destination of the MCS file: spi flash, or compare with spi flash */
/* 0x01: write mcs file to spi flash, 0x02: compare data between spi flash with mcs file*/
static uint32_t mcs_dest_mode = 0x01;
#define BUFFER_SIZE					(1024)

#include "spi_flash_w25qxx.h"

extern W25QXX_DEF_PTR w25q128bv;


/** convert ASCII to int */
static char *convert_chars_to_int(char *ptr, int32_t length, void *data)
{
	uint8_t temp[10];
	uint8_t i = 0;

	*(uint8_t *)data = 0;

	for (i = 0; i < length; i++) {
		if ((*ptr >= '0') && (*ptr <= '9')) {
			temp[i] = *ptr - '0';
		} else if ((*ptr >= 'A') && (*ptr <= 'F')) {
			temp[i] = *ptr - 'A' + 10;
		} else if ((*ptr >= 'a') && (*ptr <= 'f')) {
			temp[i] = *ptr - 'a' + 10;
		} else {
			temp[i] = 0;
			CMD_DEBUG("file error! char= %c\r\n", *ptr);
		}

		ptr++;
		*(uint8_t *)data += ((uint8_t)(temp[i] & 0x0F) << (4 * (length-i-1)));
	}

	return ptr;
}

/** get the record data*/
static int32_t get_record(char *ptr, int32_t data_length, uint8_t *record)
{
	uint16_t i = 0, length = 0;
	uint8_t checksum = 0;
	int32_t ercd = E_OK;

	length = data_length + 4;
	checksum += data_length;

	for (i = 0; i<length; i++) {
		ptr=convert_chars_to_int(ptr, 2, &record[i]);
		checksum += record[i];
	}

	if (checksum != 0) {
		ercd = E_SYS;
		CMD_DEBUG("  check sum error!\r\n");
	}

	return ercd;
}

#define BIN_BUFFER_SIZE		0x400
static uint8_t *bin_buffer = cmd_fs_buffer;
static uint32_t bin_buffer_cnt = 0;
static uint32_t bin_address = 0x0;
static uint8_t *spi_flash_buffer = &cmd_fs_buffer[CMD_FS_BUF_SIZE / 2];

/** write data with binary */
static int32_t write_bin(uint32_t address, uint8_t *data, uint8_t data_length)
{
	uint32_t i = 0, j = 0;
	int32_t ercd = E_OK;
	int32_t cnt = 0;

	/* the address is discontinuous, and the buffer is not empty*/
	if (address != (bin_address + bin_buffer_cnt)) {
		/* write the data to destination */
		if (bin_buffer_cnt != 0) {
#if MCS_DEST_MODE == 0x00

			for (i = 0; i < bin_buffer_cnt; i += 16) {
				put_dump(&bin_buffer[i], (bin_address + i), 16, DW_CHAR);
			}

#else

			if (mcs_dest_mode == 0x01) {
				/* write mcs flie to spi flash */
				ercd = w25qxx_write(w25q128bv, bin_address, bin_buffer_cnt, bin_buffer);

				if (ercd == -1) {
					CMD_DEBUG("writing data to SPI flash failed, the address: %8x, the size: %4d\r\n", bin_address, bin_buffer_cnt);
					goto error_exit;
				}

				w25qxx_wait_ready(w25q128bv);
			} else if (mcs_dest_mode == 0x02) {
				/* check FPGA configure between spi flash and specified mcs file */
				cnt = w25qxx_read(w25q128bv, bin_address, bin_buffer_cnt, spi_flash_buffer);

				if ((cnt < 0) && (cnt != bin_buffer_cnt)) {
					CMD_DEBUG("Read SPI flash error!\r\n");
					ercd = E_SYS;
					goto error_exit;
				} else {
					for (i = 0; i < bin_buffer_cnt; i ++) {
						if (spi_flash_buffer[i] != bin_buffer[i]) {
							CMD_DEBUG("The data error, address:%6x spi_flash: %2x mcs file: %2x!\r\n", bin_address+i, spi_flash_buffer[i],
							          bin_buffer[i]);
							ercd = E_SYS;
							goto error_exit;
						}
					}
				}
			}

#endif
		}

		/* set the new address */
		bin_address = address;
		bin_buffer_cnt = 0;
	}

	/* send data to buffer */
	if ((bin_buffer_cnt + data_length) > BIN_BUFFER_SIZE) {
		/* fill the buffer until the buffer overflow */
		for (j = bin_buffer_cnt; j < BIN_BUFFER_SIZE; j ++) {
			bin_buffer[j] = data[j-bin_buffer_cnt];
		}

		/* write the data to destination */
#if MCS_DEST_MODE == 0x00

		for (i = 0; i < BIN_BUFFER_SIZE; i += 16) {
			put_dump(&bin_buffer[i], (bin_address + i), 16, DW_CHAR);
		}

#else

		if (mcs_dest_mode == 0x01) {
			/* write mcs flie to spi flash */
			ercd = w25qxx_write(w25q128bv, bin_address, bin_buffer_cnt, bin_buffer);

			if (ercd == -1) {
				CMD_DEBUG("writing data to SPI flash failed, the address: %8x, the size: %4d\r\n", bin_address, bin_buffer_cnt);
				goto error_exit;
			}

			w25qxx_wait_ready(w25q128bv);
		} else if (mcs_dest_mode == 0x02) {
			/* check FPGA configure between spi flash and specified mcs file */
			cnt = w25qxx_read(w25q128bv, bin_address, bin_buffer_cnt, spi_flash_buffer);

			if ((cnt < 0) && (cnt != bin_buffer_cnt)) {
				CMD_DEBUG("Read SPI flash error!\r\n");
				ercd = E_SYS;
				goto error_exit;
			} else {
				for (i = 0; i < bin_buffer_cnt; i ++) {
					if (spi_flash_buffer[i] != bin_buffer[i]) {
						CMD_DEBUG("The data error, address:%6x spi_flash: %2x mcs file: %2x!\r\n", bin_address+i, spi_flash_buffer[i],
						          bin_buffer[i]);
						ercd = E_SYS;
						goto error_exit;
					}
				}
			}
		}

#endif
		/* set the address */
		bin_address += BIN_BUFFER_SIZE;
		j -= bin_buffer_cnt;
		bin_buffer_cnt = 0;

		/* write the remainder to buffer */
		for (j=j; j<data_length ; j++) {
			bin_buffer[bin_buffer_cnt] = data[j];
			bin_buffer_cnt ++;
		}
	} else {
		for (i = 0; i < data_length; i++) {
			bin_buffer[bin_buffer_cnt] = data[i];
			bin_buffer_cnt ++;
		}
	}

	return E_OK;

error_exit:
	bin_buffer_cnt = 0;
	bin_address = 0;
	return ercd;
}

/** close the bin */
static int32_t close_bin(void)
{
	int32_t i = 0;
	int32_t ercd = E_OK;
	int32_t cnt = 0;
	i = i;

	/* write the data to destination */
#if MCS_DEST_MODE == 0x00

	for (i = 0; i < bin_buffer_cnt; i += 16) {
		put_dump(&bin_buffer[i], (bin_address + i), 16, DW_CHAR);
	}

#else

	if (mcs_dest_mode == 0x01) {
		/* write mcs flie to spi flash */
		ercd = w25qxx_write(w25q128bv, bin_address, bin_buffer_cnt, bin_buffer);

		if (ercd == -1) {
			CMD_DEBUG("writing data to SPI flash failed, the address: %8x, the size: %4d\r\n", bin_address, bin_buffer_cnt);
			goto error_exit;
		}

		w25qxx_wait_ready(w25q128bv);
	} else if (mcs_dest_mode == 0x02) {
		/* check FPGA configure between spi flash and specified mcs file */
		cnt = w25qxx_read(w25q128bv, bin_address, bin_buffer_cnt, spi_flash_buffer);

		if ((cnt < 0) && (cnt != bin_buffer_cnt)) {
			CMD_DEBUG("Read SPI flash error!\r\n");
			ercd = E_SYS;
			goto error_exit;
		} else {
			for (i = 0; i < bin_buffer_cnt; i ++) {
				if (spi_flash_buffer[i] != bin_buffer[i]) {
					CMD_DEBUG("The data error, address:%6x spi_flash: %2x mcs file: %2x!\r\n", bin_address+i, spi_flash_buffer[i],
					          bin_buffer[i]);
					ercd = E_SYS;
					goto error_exit;
				}
			}
		}
	}

#endif
	/* clean buffer and binary address */
	bin_buffer_cnt = 0;
	bin_address = 0x00;
	return E_OK;

error_exit:
	return ercd;
}

#define LINEAR_ADDRESS 		0x00
#define SEGMENT_ADDRESS 	0x01
static uint8_t address_type = LINEAR_ADDRESS;
static uint32_t segment_address_base = 0x00;
static uint32_t linear_address_base = 0x00;

/** parsing the record */
static int32_t record_paring(uint8_t *record, int32_t data_length)
{
	uint32_t address = 0;
	uint8_t record_type = 0;
	uint8_t data[MCS_DATA_FIELD_MAX_SIZE];
	uint8_t i = 0;
	int32_t ercd = E_OK;

	address = ((uint32_t)record[0] * 256 + (uint32_t)record[1]);
	record_type = record[2];

	for (i = 0; i < data_length; i++) {
		data[i] = record[i+3];
	}

	/* the record type */
	switch (record_type) {
		/* the data record */
		case 0x00:
			address += (address_type == SEGMENT_ADDRESS) ? segment_address_base : linear_address_base;
			ercd = write_bin(address, data, data_length);

			if (ercd != E_OK) {
				goto error_exit;
			}

			break;

		/* end of file */
		case 0x01:
			// CMD_DEBUG("The end of file!\r\n");
			ercd = close_bin();

			if (ercd != E_OK) {
				goto error_exit;
			}

			break;

		/* extended segment address */
		case 0x02:
			address_type = SEGMENT_ADDRESS;
			segment_address_base = ((uint32_t)data[0] * 256 + (uint32_t)data[1]) * 16;
			// CMD_DEBUG("segment address: %8x\r\n", segment_address_base);
			break;

		/* start segment address */
		case 0x03:
			// CMD_DEBUG("start segment address!\r\n");
			break;

		/* extended linear address */
		case 0x04:
			address_type = LINEAR_ADDRESS;
			linear_address_base = ((uint32_t) data[0] * 256 + (uint32_t)data[1]) << 16;
			// CMD_DEBUG("linear address: %8x\r\n", linear_address_base);
			break;

		/* start linear address */
		case 0x05:
			// CMD_DEBUG("start linear address!\r\n");
			break;
	}

	return E_OK;

error_exit:
	return ercd;
}

/** parsing the MCS file */
static int32_t mcs_parsing(FIL *fp)
{
	uint32_t cnt = 0;
	uint8_t data_length = 0;
	uint8_t record_line[MCS_RECORD_BUFFER_SIZE] = {0};
	// char option[5];
	char mcs_buffer[BUFFER_SIZE];
	int32_t mcs_buffer_cnt = 0;
	int32_t ercd = E_OK;
	uint8_t res = 0;
	char *ptr = NULL;
	uint32_t file_size = f_size(fp);
	int32_t progress_size = 0;
	int32_t fp_offset = 0;

	while (1) {
		res = f_read(fp, mcs_buffer, BUFFER_SIZE, &cnt);

		if (res != FR_OK || cnt == 0) {
			break;
		} else {
			mcs_buffer_cnt = 0;

			while (mcs_buffer[mcs_buffer_cnt] == ':') {
				mcs_buffer_cnt ++;
				/* get the length of data field */
				ptr = convert_chars_to_int(&mcs_buffer[mcs_buffer_cnt], 2, &data_length);

				/* check whether the record line is complete */
				if ((mcs_buffer_cnt + data_length * 2 + MCS_FIXED_SECTION_SIZE) >= BUFFER_SIZE) {
					/* move the file pointer*/
					fp_offset = mcs_buffer_cnt - 1 - BUFFER_SIZE;
					f_lseek(fp, (f_tell(fp) + fp_offset));
					progress_size += fp_offset;
					break;
				} else {
					/* check the line terminators*/
					mcs_buffer_cnt += data_length * 2 + MCS_FIXED_SECTION_SIZE;

					if (mcs_buffer[mcs_buffer_cnt - 1] == 0x0D) {
						mcs_buffer_cnt ++;
					}

					if (mcs_buffer[mcs_buffer_cnt - 1] != 0x0A) {
						CMD_DEBUG("The line terminator format is in error!\r\n");
						ercd = E_SYS;
						goto error_exit;
					}
				}

				/* get the record line*/
				ercd = get_record(ptr, data_length, record_line);

				if (ercd == E_OK) {
					/* parsing the record line */
					ercd = record_paring(record_line, data_length);

					if (ercd != E_OK) {
						goto error_exit;
					}
				} else {
					goto error_exit;
				}

				/* check the mcs_buffer length */
				if ((mcs_buffer_cnt + MCS_FIXED_SECTION_SIZE) > BUFFER_SIZE) {
					/* move the file pointer */
					fp_offset = mcs_buffer_cnt - BUFFER_SIZE;
					f_lseek(fp, (f_tell(fp) + fp_offset));
					progress_size += fp_offset;
					break;
				}
			}
		}

		/* show the progress bar */
		progress_size += cnt;

		if (mcs_dest_mode == 0x01) {
			CMD_DEBUG("\rdownload: %d%%", 100 * progress_size / file_size);
		} else {
			CMD_DEBUG("\rcheck: %d%%", 100 * progress_size / file_size);
		}
	}

	CMD_DEBUG("\r\n");

	return E_OK;

error_exit:
	return ercd;
}

/* show help of command mload */
static void cmd_mload_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]... [FILE]\r\n"
		"Download FPGA configura file(MCS file) to SPI flash\r\n"
		"  -h/H/?        Show the help information\r\n"
		"  -i            Read device ID\r\n"
		"  -e            Erase the SPI flash\r\n"
		"  -c file.mcs   Check firmware between SPI flash and file.mcs\r\n"
		"  -w file.mcs   Load the file.mcs to SPI flash\r\n"
		"  -r add num    Read <num> data from <add> in SPI flash\r\n"
		"Examples: \r\n"
		"  mload -e -w emsk_11.mcs    Erase the SPI flash,load the configuration file of the emsk 1.1 to SPI flash\r\n"
		"  mload -h     Show the help information\r\n", cmd_name);

error_exit:
	return;
}

/** the callback function of command "mload "*/
static int cmd_mload(int argc, char **argv, void *extobj)
{

	char *file_name = NULL;
	char *check_filename = NULL;
	uint8_t res;
	int32_t opt;
	uint32_t ID;
	uint8_t local_buf[128];

	int32_t ercd = E_OK;
	long spi_address = 0, length = 0;
	int32_t cnt = 0, len = 0;
	uint8_t *ptr = NULL;
	/*bit 0: read spi flash;
	 bit1: write mcs file to spi flash
	 bit2: check mcs file between spi flash and specified mcs file
	 bit3: erase the spi flash*/
	uint8_t mode = 0;

	NTSHELL_IO_GET(extobj);

	if (argc < 2) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	}

	w25qxx_init(w25q128bv, BOARD_SPI_FREQ);

	opterr = 0;
	optind = 1;

	// parse command line
	while ((opt=getopt(argc, argv, "irc:w:ehH?")) != -1) {
		switch (opt) {
			case '?':
			case 'h':
				cmd_mload_help(argv[0], extobj);
				goto error_exit;
				break;

			case 'i':
				ID = w25qxx_read_id(w25q128bv);
				CMD_DEBUG("Device ID = %x\r\n", ID);
				goto error_exit;
				break;

			case 'r':
				mode = mode | 0x01;

				if (argc == 4) {
					if (!xatoi(&argv[2], &spi_address) || !xatoi(&argv[3], &length)) {
						ercd = E_SYS;
						CMD_DEBUG("Parameter error!\r\n");
						goto error_exit;
					}
				} else {
					ercd = E_SYS;
					CMD_DEBUG("command error!\r\n");
					CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
					goto error_exit;
				}

				break;

			case 'w':
				/* write the data in MCS file to SPI flash */
				mode = mode | 0x02;
				/* read file name */
				file_name = optarg;
				/* open the file */
				res = f_open(&cmd_files[0], file_name, FA_READ | FA_OPEN_EXISTING);

				if (res) {
					CMD_DEBUG("ERROR: Cannot open file %s\r\n", file_name);
					ercd = E_SYS;
					goto error_exit;
				}

				break;

			case 'c':
				/* check MCS file in spi flash */
				mode = mode | 0x04;
				/* read file name */
				check_filename = optarg;
				/* open the file */
				res = f_open(&cmd_files[1], check_filename, FA_READ | FA_OPEN_EXISTING);

				if (res) {
					CMD_DEBUG("ERROR: Cannot open file %s\r\n", check_filename);
					ercd = E_SYS;
					goto error_exit;
				}

				break;

			case 'e':
				/* erase the spi flash */
				mode = mode | 0x08;
				break;

			default:
				CMD_DEBUG("%s: unrecognized option:%c\r\n", argv[0], opt);
				CMD_DEBUG("Try '%s -h' for more information\r\n",argv[0]);
				ercd = E_SYS;
				goto error_exit;
				break;
		}
	}

	/* erase the whole SPI flash */
	if (mode & 0x08) {
		CMD_DEBUG("Erasing SPI flash ...\r");
		ercd = w25qxx_erase(w25q128bv, FPGA_IMAGE_START, FPGA_IMAGE_SIZE);

		if (ercd == -1) {
			CMD_DEBUG("Erase SPI flash error!\r\n");
			goto error_exit;
		} else {
			CMD_DEBUG("Erase SPI flash successfully!\r\n");
		}

		w25qxx_wait_ready(w25q128bv);
	}

	/* read the data from SPI flash */
	if (mode & 0x01) {
		cnt = w25qxx_read(w25q128bv, spi_address, 128, local_buf);

		if (cnt < 0) {
			ercd = E_SYS;
			goto error_exit;
		}

		len = (length < cnt) ? length : cnt;

		for (ptr = local_buf; len >= 16 ; ptr += 16, len -= 16, spi_address += 16) {
			put_dump(ptr, (DWORD)spi_address, 16, 1);
		}

		if (len > 0) {
			put_dump(ptr, (DWORD)spi_address, len, 1);
		}
	}

	/* write data to SPI flash from MCS file */
	if (mode & 0x02) {
		/* set the destination: spi flash*/
		mcs_dest_mode = 0x01;
		/* mcs file parsing */
		ercd = mcs_parsing(&cmd_files[0]);

		/* close the file */
		if (f_close(&cmd_files[0]) != FR_OK) {
			ercd = E_SYS;
			CMD_DEBUG("The file close error!\r\n");
			goto error_exit;
		}
	}

	/* write data to SPI flash from MCS file */
	if (mode & 0x04) {
		/* check FPGA configure between spi flash and specified mcs file */
		mcs_dest_mode = 0x02;
		/* mcs file parsing */
		ercd = mcs_parsing(&cmd_files[1]);

		/* close the file */
		if (f_close(&cmd_files[1]) != FR_OK) {
			ercd = E_SYS;
			CMD_DEBUG("The file close error!\r\n");
			goto error_exit;
		}
	}

	return E_OK;

error_exit:
	return ercd;
}

static CMD_TABLE_T mload_cmd = {"mload", "Update firmware, read or write MCS file in the SPI flash", cmd_mload, NULL};
/**
 * register spirw command
 */
CMD_TABLE_T *register_ntshell_cmd_mload(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&mload_cmd, prev);
}
#endif /* NTSHELL_USE_CMDS_FS_MLOAD */
