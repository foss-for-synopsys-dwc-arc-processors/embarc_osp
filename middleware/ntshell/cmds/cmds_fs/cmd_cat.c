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
* \brief filesystem operation commands: cat
*/

#include "cmds_fs_cfg.h"
#if NTSHELL_USE_CMDS_FS_CAT
#include "cmd_fs_common.h"

static NTSHELL_IO_PREDEF;

static int32_t fs_cat(char *pathname, int32_t mode, void *extobj)
{
	int32_t ercd = E_OK;
	uint8_t res = 0;
	uint32_t s1, blen = sizeof(cmd_fs_buffer);
	char *ptr;
	uint32_t ofs = 0;
	char line[5];
	int32_t base_address = 0;

	VALID_EXTOBJ_NORTN(extobj);

	res = f_open(&cmd_files[0], pathname, FA_OPEN_EXISTING | FA_READ);

	if (res) {
		ercd = E_SYS;
		fs_put_err(res, extobj);
		goto error_exit;
	}

	for (;;) {
		res = f_read(&cmd_files[0], cmd_fs_buffer, blen, &s1);

		if (res || s1 == 0) {
			break;    /* error or eof */
		}

		if (mode == 1) {
			for (ptr=(char *)cmd_fs_buffer, ofs = 0; ofs < s1; ptr += 16, ofs += 16) {
				cmds_put_dump((uint8_t *)ptr, (base_address+ofs), 16, DW_CHAR, extobj);
			}
		} else {
			CMD_WRITE(cmd_fs_buffer, s1);
			CMD_DEBUG("\r\n");
		}

		base_address += blen;

		/* check end of file*/
		if (s1 < blen) {
			CMD_DEBUG("\r\n");
			break;
		}

		for (;;) {
			CMD_DEBUG("Continue ? (y or n)");
			// xgets(Line, sizeof Line);
			CMD_READ(line, 1);
			CMD_DEBUG("\r\n");
			ptr = line;

			while (*ptr == ' ') {
				ptr++;
			}

			if (line[0] == 'n') {
				CMD_DEBUG("\r\n");
				f_close(&cmd_files[0]);
				goto error_exit;
			} else if (line[0] == 'y') {
				CMD_DEBUG("\r\n");
				break;
			}
		}
	}

	f_close(&cmd_files[0]);
	return E_OK;

error_exit:
	return ercd;
}
/* show help of command */
static void cmd_cat_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]... FILE\r\n"
		"Concatenate File, or standard input, to standard output\r\n"
		"  -h/H/?    Show the help information\r\n"
		"  -b        Output with both hexadecimal format and ASCII\r\n"
		"Examples: \r\n"
		"  cat <file> Output file contents, then standard intput, then file contents\r\n"
		"  cat -h     Show the help information\r\n", cmd_name);

error_exit:
	return;
}

/*command: cat*/
static int cmd_cat(int argc, char **argv, void *extobj)
{
	int32_t ercd = E_OK;
	int32_t opt;
	uint8_t mode=0;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	if (argc < 2) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	} else if (*argv[1] != '-') {
		ercd = fs_cat(argv[1], mode, extobj);

		if (ercd != E_OK) {
			goto error_exit;
		}

		return E_OK;
	}

	opterr = 0;
	optind = 1;

	while ((opt=getopt(argc, argv, "bhH?")) != -1) {
		switch (opt) {
			case 'h':
			case '?':
			case 'H':
				cmd_cat_help(argv[0], extobj);
				goto error_exit;
				break;

			case 'b':
				mode = 1;
				break;

			default:
				CMD_DEBUG("%s: unrecognized option:%c\r\n", argv[0], opt);
				CMD_DEBUG("Try '%s -h' for more information\r\n",argv[0]);
				break;
		}
	}

	/*chech the parameter*/
	if ((optind+1) == argc) {
		ercd = fs_cat(argv[optind], mode, extobj);

		if (ercd != E_OK) {
			goto error_exit;
		}
	} else {
		ercd = E_SYS;
		CMD_DEBUG("%s: parameter error\r\n", argv[0]);
		CMD_DEBUG("Try '%s -h' for more information\r\n",argv[0]);
		goto error_exit;
	}

	return E_OK;

error_exit:
	return ercd;
}

static CMD_TABLE_T cat_cmd = {"cat", "Output file contents", cmd_cat, NULL};
/**
 * register cat command
 */
CMD_TABLE_T *register_ntshell_cmd_cat(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&cat_cmd, prev);
}
#endif /*NTSHELL_USE_CMDS_FS_CAT*/