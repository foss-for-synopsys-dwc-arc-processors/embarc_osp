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
* \brief filesystem operation commands: touch
*/

#include "cmds_fs_cfg.h"
#if NTSHELL_USE_CMDS_FS_TOUCH
#include "cmd_fs_common.h"

static NTSHELL_IO_PREDEF;

/* show help of command */
static void cmd_touch_help(char *cmd_name, void *extobj)
{
	NTSHELL_IO_GET(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]... FILE\r\n"
		"A FILE that does not exist is created empty\r\n"
		"  -h/H/?    Show the help information\r\n"
		"  -v        Explain what is being done\r\n"
		"Examples: \r\n"
		"  touch <file>    Create the <file>\r\n"
		"  touch -h    Show the help information\r\n", cmd_name);

error_exit:
	return;
}
/*command: touch*/
static int cmd_touch(int argc, char **argv, void *extobj)
{
	int32_t ercd = E_OK;
	uint8_t res = 0;
	int32_t opt = 0;
	uint8_t print_flag = 0;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);


	if (argc == 1) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	} else if (*argv[1] != '-') {

		res = f_open(&cmd_files[0], argv[1], FA_CREATE_NEW);

		if (res != FR_OK) {
			ercd = E_SYS;
			fs_put_err(res, extobj);
			goto error_exit;
		}

		f_close(&cmd_files[0]);
		return E_OK;
	}

	opterr = 0;
	optind = 1;

	while ((opt=getopt(argc, argv, "vhH?")) != -1) {
		switch (opt) {
			case 'h':
			case '?':
			case 'H':
				cmd_touch_help(argv[0], extobj);
				goto error_exit;
				break;

			case 'v':
				print_flag = 1;
				break;

			default:
				CMD_DEBUG("%s: unrecognized option:%c\r\n", argv[0], opt);
				CMD_DEBUG("Try '%s -h' for more information\r\n",argv[0]);
				break;
		}
	}

	/*chech the parameter*/
	if ((optind+1) == argc) {
		res = f_open(&cmd_files[0], argv[optind], FA_CREATE_NEW);

		if (res != FR_OK) {
			ercd = E_SYS;
			fs_put_err(res, extobj);
			goto error_exit;
		}

		if (print_flag == 1) {
			CMD_DEBUG("%s: created file '%s'\r\n",argv[0], argv[optind]);
		}

		f_close(&cmd_files[0]);
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
static CMD_TABLE_T touch_cmd = {"touch", "Create a file", cmd_touch, NULL};
/**
 * register mkdir command
 */
CMD_TABLE_T *register_ntshell_cmd_touch(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&touch_cmd, prev);
}
#endif /*NTSHELL_USE_CMDS_FS_TOUCH*/