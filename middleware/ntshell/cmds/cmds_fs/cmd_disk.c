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
#if NTSHELL_USE_CMDS_FS_DISK
#include "cmd_fs_common.h"

static NTSHELL_IO_PREDEF;

int32_t disk_initial(int disk_num, void *extobj)
{
	int32_t ercd = E_OK;
	uint8_t res = 0;

	VALID_EXTOBJ_NORTN(extobj);

	res = disk_initialize((uint8_t)disk_num);

	if (res != 0) {
		CMD_DEBUG("Disk initialize error! The error code: %d\r\n", res);
		ercd = E_SYS;
		goto error_exit;
	}

	return E_OK;

error_exit:
	return ercd;
}

int32_t disk_display_status(int disk_num, void *extobj)
{
	int32_t ercd = E_OK;
	int32_t p2;
	uint32_t s1;
	uint8_t b, res;

	VALID_EXTOBJ_NORTN(extobj);

	if ((res=disk_ioctl((uint8_t)disk_num, GET_SECTOR_COUNT, &p2)) == RES_OK) {
		CMD_DEBUG("Drive size: %lu sectors\r\n", p2);
	} else {
		CMD_DEBUG("Disk error! The error code: %d\r\n", res);
		ercd = E_SYS;
		goto error_exit;
	}

	if (disk_ioctl((uint8_t)disk_num, GET_BLOCK_SIZE, &p2) == RES_OK) {
		CMD_DEBUG("Block size: %lu sectors\r\n", p2);
	}

	if (disk_ioctl((uint8_t)disk_num, MMC_GET_TYPE, &b) == RES_OK) {
		CMD_DEBUG("Media type: %u\r\n", b);
	}

	if (disk_ioctl((uint8_t)disk_num, MMC_GET_CSD, cmd_fs_buffer) == RES_OK) {
		CMD_DEBUG("CSD:\r\n");
		cmds_put_dump(cmd_fs_buffer, 0, 16, DW_CHAR, extobj);
	}

	if (disk_ioctl((uint8_t)disk_num, MMC_GET_CID, cmd_fs_buffer) == RES_OK) {
		CMD_DEBUG("CID:\r\n");
		cmds_put_dump(cmd_fs_buffer, 0, 16, DW_CHAR, extobj);
	}

	if (disk_ioctl((uint8_t)disk_num, MMC_GET_OCR, cmd_fs_buffer) == RES_OK) {
		CMD_DEBUG("OCR:\r\n");
		cmds_put_dump(cmd_fs_buffer, 0, 4, DW_CHAR, extobj);
	}

	if (disk_ioctl((uint8_t)disk_num, MMC_GET_SDSTAT, cmd_fs_buffer) == RES_OK) {
		CMD_DEBUG("SD Status:\r\n");

		for (s1 = 0; s1 < 64; s1 += 16) {
			cmds_put_dump(cmd_fs_buffer+s1, s1, 16, DW_CHAR, extobj);
		}
	}

	return E_OK;

error_exit:
	return ercd;
}

/*command: disk*/
/* show help of command */
static void cmd_disk_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION] [DEVICE_ID]\r\n"
		"Copy source to destination\r\n"
		"  -h/H/?       show the help information\r\n"
		"  -i <pd#>     Initialize the storge device\r\n"
		"  -s <pd#>     Show the storge device status\r\n"
		"Examples: \r\n"
		"  disk -i 0    Initialize the storge device 0\r\n"
		"  disk -h      Show the help information\r\n", cmd_name);

error_exit:
	return;
}

static int cmd_disk(int argc, char **argv, void *extobj)
{
	int32_t ercd = E_OK;
	int32_t p1 = 0;
	int32_t opt;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	if (argc < 2) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	}

	opterr = 0;
	optind = 1;

	while ((opt=getopt(argc, argv, "i:s:hH?")) != -1) {
		switch (opt) {
			case 'h':
			case '?':
			case 'H':
				cmd_disk_help(argv[0], extobj);
				goto error_exit;
				break;

			case 'i':
				p1 = atoi(optarg);
				ercd = disk_initial(p1, extobj);

				if (ercd != E_OK) {
					goto error_exit;
				}

				break;

			case 's':
				p1 = atoi(optarg);
				ercd = disk_display_status(p1, extobj);

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

static CMD_TABLE_T disk_cmd = {"disk", "Operate the storge device", cmd_disk, NULL};
/**
 * register disk command
 */

CMD_TABLE_T *register_ntshell_cmd_disk(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&disk_cmd, prev);
}
#endif /*NTSHELL_USE_CMDS_FS_DISK*/