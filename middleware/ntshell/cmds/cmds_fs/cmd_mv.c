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
* \brief filesystem operation commands: mv
*/

#include "cmds_fs_cfg.h"
#if NTSHELL_USE_CMDS_FS_MV
#include "cmd_fs_common.h"

static NTSHELL_IO_PREDEF;

static int32_t fatfs_rename(char *old_name, char *new_name, void *extobj)
{
	int32_t ercd = E_OK;
	uint8_t res = 0;
	FILINFO fno;
	char *ptr = NULL;

	check_path(old_name);
	check_path(new_name);

#if _USE_LFN
	fno.lfname = NULL;
	fno.lfsize = 0;
#endif
	res = f_stat(new_name, &fno);

	if ((res == FR_OK) && (((fno.fattrib & AM_DIR) == AM_DIR) || ((fno.fattrib & AM_VOL) == AM_VOL))) {
		/* new_name is folder or vol */
		/* get the file name*/
		ptr = get_new_filepath(new_name, old_name);

		if (ptr == NULL) {
			ercd = E_SYS;
			CMD_DEBUG("Out of memory!\r\n");
			goto error_exit;
		}

		res = f_rename(old_name, ptr);
		free(ptr);
	} else {
		res = f_rename(old_name, new_name);
	}

	if (res != FR_OK) {
		ercd = res;
		goto error_exit;
	}

	return E_OK;
error_exit:
	return ercd;
}

/* show help of command */
static void cmd_mv_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]... SOURCE DEST\r\n"
		"  Or: %s [OPTION]... SOURCE DIRECTORY\r\n"
		"Rename SOURCE to DEST, or move SOURCE to DIRECTORY\r\n"
		"  -h/H/?    Show the help information\r\n"
		"  -v        Explain what is being done\r\n"
		"Examples: \r\n"
		"  mv <old_name> <new_name>   Rename the file to <new_name>\r\n"
		"  mv -h     Show the help information\r\n", cmd_name, cmd_name);

error_exit:
	return;
}

/*command: mv*/
static int cmd_mv(int argc, char **argv, void *extobj)
{
	int32_t ercd = E_OK;
	int32_t opt;
	uint8_t print_flag = 0;
	int32_t valid_ind = 1;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	if ((argc == 3) && (*argv[1] != '-')) {
		valid_ind = 1;
	} else if (*argv[1] != '-') {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	} else {
		opterr = 0;
		optind = 1;

		while ((opt=getopt(argc, argv, "vhH?")) != -1) {
			switch (opt) {
				case 'h':
				case '?':
				case 'H':
					cmd_mv_help(argv[0], extobj);
					goto error_exit;
					break;

				case 'v':
					print_flag = 1;
					break;

				default:
					CMD_DEBUG("%s: unrecognized option:%c\r\n", argv[0], opt);
					CMD_DEBUG("Try '%s -h' for more information\r\n",argv[0]);
					goto error_exit;
					break;
			}
		}

		valid_ind = 2;
	}

	/*chech the parameter*/
	ercd = fatfs_rename(argv[valid_ind], argv[valid_ind+1], extobj);

	if (print_flag == 1) {
		CMD_DEBUG("%s: Rename or move '%s' -> '%s'\r\n", argv[0], argv[valid_ind], argv[valid_ind+1]);
	}

	if (ercd != E_OK) {
		if ((ercd > FR_OK) && (ercd <= FR_INVALID_PARAMETER)) {
			fs_put_err(ercd, extobj);
		}

		goto error_exit;
	}

	return E_OK;

error_exit:
	return ercd;
}

static CMD_TABLE_T mv_cmd = {"mv", "Rename file or move file", cmd_mv, NULL};
/**
* register mv command
*/
CMD_TABLE_T *register_ntshell_cmd_mv(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&mv_cmd, prev);
}
#endif /*NTSHELL_USE_CMDS_FS_MV*/