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

 /**
 * \file
 * \brief filesystem operation commands: eflash
 */

#include "cmds_fs_cfg.h"
#if NTSHELL_USE_CMDS_FS_EFLASH
#include "cmd_fs_common.h"

static NTSHELL_IO_PREDEF;
/* show help of command */
static void cmd_eflash_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}
	CMD_DEBUG("Usage: %s [OPTION]...\r\n"
		"Write bin file to eflash\r\n"
		"  -h/H/?    Show the help information\r\n"
		"Examples: \r\n"
		"  eflash  test.bin    Write bin file to eflash\r\n"
		"  eflash -h    Show the help information\r\n", cmd_name);

error_exit:
	return;
}

uint8_t buffer[SMIC_EFLASH_PAGE_SIZE] = {0};
uint8_t buffer_r[SMIC_EFLASH_PAGE_SIZE] = {0};
/*command: eflash*/
static int cmd_eflash(int argc, char **argv, void *extobj)
{
	int32_t ercd = E_OK;
	uint8_t res = 0;
	int32_t opt;
	int32_t file_size;
	uint32_t page_size;
	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);
	EFLASH_DEFINE(eflash_test, EFLASH_CRTL_BASE);
	SMIC_EFLASH_INFO eflash_info;
	if(argc == 1) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	} else if (*argv[1] != '-') {
		res = f_open(&cmd_files[0], argv[1], FA_READ);
		if(res != FR_OK) {
			ercd = E_SYS;
			fs_put_err(res, extobj);
			goto error_exit;
		}
		file_size = f_size(&cmd_files[0]);
		if(file_size == -1) {
			ercd = E_SYS;
			CMD_DEBUG("filename: %s, file_size = -1\r\n", argv[1]);
			f_close(&cmd_files[0]);
			goto error_exit;
		}
		smic_eflash_open(eflash_test);
		smic_eflash_control(eflash_test, SMIC_EFLASH_GET_INFO, (void *)(&eflash_info));
		page_size = eflash_info.eflash_page_size;
		if(file_size > eflash_info.eflash_page_size * eflash_info.eflash_page_cnt) {
			ercd = E_SYS;
			CMD_DEBUG("filename: %s, file_size = %d > eflash size = %d \r\n", argv[1],
			file_size, eflash_info.eflash_page_size * eflash_info.eflash_page_cnt);
			f_close(&cmd_files[0]);
			smic_eflash_close(eflash_test);
			goto error_exit;
		}
		smic_eflash_control(eflash_test, SMIC_EFLASH_MACRO_ERASE, NULL);
		uint32_t txlen = file_size;
		int32_t buf_pt=0;
		while(txlen >0) {
			uint32_t send_size=0;
			if (txlen > page_size) {
				send_size = page_size;
			} else {
				send_size = txlen;
			}
			f_lseek(&cmd_files[0], buf_pt);
			f_read(&cmd_files[0], buffer, send_size, &send_size);
			smic_eflash_write(eflash_test, buf_pt, send_size, buffer);
			smic_eflash_read(eflash_test, buf_pt, send_size, buffer_r);
			for(int i = 0; i < send_size; i++) {
				if(buffer[i] != buffer_r[i]) {
					ercd = E_SYS;
					CMD_DEBUG("eflash write failed !\r\n");
					f_close(&cmd_files[0]);
					smic_eflash_close(eflash_test);
					goto error_exit;
				}
			}
			buf_pt += send_size;
			txlen -= send_size;
		}
		f_close(&cmd_files[0]);
		smic_eflash_close(eflash_test);
		return E_OK;
	}

	opterr = 0;
	optind = 1;

	while ((opt=getopt(argc, argv, "hH?")) != -1) {
		switch (opt) {
			case 'h':
			case '?':
			case 'H':
				cmd_eflash_help(argv[0], extobj);
				goto error_exit;
				break;
			default:
				CMD_DEBUG("%s: unrecognized option:%c\r\n", argv[0], opt);
				CMD_DEBUG("Try '%s -h' for more information\r\n",argv[0]);
				break;
		}
	}

	return E_OK;

error_exit:
	return ercd;
}

static CMD_TABLE_T eflash_cmd = {"eflash", "Write bin file to eflash", cmd_eflash, NULL};
/**
 * register eflash command
 */
CMD_TABLE_T * register_ntshell_cmd_eflash(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&eflash_cmd, prev);
}
#endif /*NTSHELL_USE_CMDS_FS_PWD*/