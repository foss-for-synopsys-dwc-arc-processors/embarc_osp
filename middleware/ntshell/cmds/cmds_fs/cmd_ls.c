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
* \brief filesystem operation commands: ls
*/

#include "cmds_fs_cfg.h"
#if NTSHELL_USE_CMDS_FS_LS
#include "cmd_fs_common.h"

static NTSHELL_IO_PREDEF;

/* list the files or folders*/
static int32_t fs_ls(char *pathname, int mode, void *extobj)
{
	int32_t ercd = E_OK;
	int32_t p1;
	uint8_t res = 0;
	uint32_t s1, s2 = sizeof(cmd_fs_buffer);
	FATFS *fs;
	char *filename = NULL;
	FILINFO Finfo;
	DIR Dir;

	VALID_EXTOBJ_NORTN(extobj);

#if _USE_LFN
	Finfo.lfname = working_directory;
	Finfo.lfsize = sizeof(working_directory);
#endif

	res = f_opendir(&Dir, pathname);

	if (res) {
		fs_put_err(res, extobj); goto error_exit;
	}

	if (mode == 1) {
		p1 = s1 = s2 = 0;

		for (;;) {
			res = f_readdir(&Dir, &Finfo);

			if ((res != FR_OK) || !Finfo.fname[0]) {
				break;
			}

			if (Finfo.fattrib & AM_DIR) {
				s2++;
			} else {
				s1++; p1 += Finfo.fsize;
			}

#if _USE_LFN
			filename = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
#else
			filename = Finfo.fname;
#endif
			CMD_DEBUG("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  %-12s\t\r\n",
			          (Finfo.fattrib & AM_DIR) ? 'D' : '-',
			          (Finfo.fattrib & AM_RDO) ? 'R' : '-',
			          (Finfo.fattrib & AM_HID) ? 'H' : '-',
			          (Finfo.fattrib & AM_SYS) ? 'S' : '-',
			          (Finfo.fattrib & AM_ARC) ? 'A' : '-',
			          (Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
			          (Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63,
			          Finfo.fsize, filename);
		}

		CMD_DEBUG("%4u File(s),%10lu bytes total\r\n%4u Dir(s)", s1, p1, s2);
		res = f_getfree(pathname, (uint32_t *)&p1, &fs);

		if (res == FR_OK) {
			CMD_DEBUG(", %10lu bytes free\r\n", p1 * fs->csize * 512);
		} else {
			fs_put_err(res, extobj);
		}
	} else {
		for (;;) {
			res = f_readdir(&Dir, &Finfo);

			if ((res != FR_OK) || !Finfo.fname[0]) {
				break;
			}

#if _USE_LFN
			filename = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
#else
			filename = Finfo.fname;
#endif
			CMD_DEBUG(" %s\t", filename);
		}

		CMD_DEBUG("\r\n");
	}

	return E_OK;
error_exit:
	return ercd;
}

/* show help of command */
static void cmd_ls_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]... [DIR]\r\n"
		"List information about the files(the current working_directory by default)\r\n"
		"  -h/H/?   Show the help information\r\n"
		"  -l [dir] Show the details of the files\r\n"
		"Examples: \r\n"
		"  ls            List information of the files in the current working_directory\r\n"
		"  ls -l [dir]   List details of the files in the [dir]\r\n"
		"  ls -h         Show the help information\r\n", cmd_name);

error_exit:
	return;
}

/** the callback function of "ls" command*/
static int cmd_ls(int argc, char **argv, void *extobj)
{
	int32_t ercd = E_OK;
	char *working_directory = NULL;
	int32_t opt;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	working_directory = fs_working_dir();

	if (argc == 1) {
		ercd = fs_ls(working_directory, 0, extobj);

		if (ercd != E_OK) {
			goto error_exit;
		}

		return E_OK;
	}

	if (argv[1][0] != '-') {
		check_path(argv[1]);
		ercd = fs_ls(argv[1], 0, extobj);

		if (ercd != E_OK) {
			goto error_exit;
		}

		return E_OK;
	}

	opterr = 0;
	optind = 1;

	while ((opt=getopt(argc, argv, "lhH?")) != -1) {
		switch (opt) {
			case 'h':
			case '?':
			case 'H':
				cmd_ls_help(argv[0], extobj);
				goto error_exit;
				break;

			case 'l':
				if (argc == 2) {
					ercd = fs_ls(working_directory, 1, extobj);
				} else {
					check_path(argv[2]);
					ercd = fs_ls(argv[2], 1, extobj);
				}

				if (ercd != E_OK) {
					goto error_exit;
				}

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

static CMD_TABLE_T ls_cmd = {"ls", "List information about files", cmd_ls, NULL};
/**
 * register ls command
 */
CMD_TABLE_T *register_ntshell_cmd_ls(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&ls_cmd, prev);
}

#endif /*NTSHELL_USE_CMDS_FS_LS*/