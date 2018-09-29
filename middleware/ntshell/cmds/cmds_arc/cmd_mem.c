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
* \brief filesystem operation commands: mem
*/

#include "cmds_arc_cfg.h"
#if NTSHELL_USE_CMDS_ARC_MEM

#include "ntshell_common.h"

static NTSHELL_IO_PREDEF;
/*command: mem*/
/* show help of command */
static void cmd_mem_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]... [ADDR] [LENGTH] [VALUE]\r\n"
		"Read or Write data in the memory\r\n"
		"  -h/H/?       Show the help information\r\n"
		"  -d <address> <length> Dump memory\r\n"
		"  -f <address> <length> <value> Fill memory\r\n"
		"  -e <address> <value>... Edit memory\r\n"
		"Examples: \r\n"
		"  mem -f <addr> <length> <value>    Fill <value> to memory from <addr> to <addr> + <length>\r\n"
		"  mem -d 0x0 1000    Dump memory from 0x0, length 1000\r\n"
		"  mem -h     Show the help information\r\n", cmd_name);

error_exit:
	return;
}

static int cmd_mem(int argc, char **argv, void *extobj)
{
	int32_t ercd = E_OK;
	int32_t opt;
	long ram_address, length, value;
	char *ptr;

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

	while ((opt=getopt(argc, argv, "d:f:e:hH?")) != -1) {
		switch (opt) {
			case 'h':
			case '?':
			case 'H':
				cmd_mem_help(argv[0], extobj);
				goto error_exit;
				break;

			case 'd':
				if (!xatoi(&argv[2], &ram_address) || !xatoi(&argv[3], &length)) {
					ercd = E_SYS;
					CMD_DEBUG("Parameter error!\r\n");
					goto error_exit;
				}

				for (ptr = (char *)ram_address; length >= 16 ; ptr += 16, length -= 16) {
					cmds_put_dump(ptr, (uint32_t)ptr, 16, 1, extobj);
				}

				if (length) {
					cmds_put_dump((uint8_t *)ptr, (uint32_t)ptr, length, 1, extobj);
				}

				break;

			case 'f':
				if (!xatoi(&argv[2], &ram_address) || !xatoi(&argv[3], &length) || !xatoi(&argv[4], &value)) {
					ercd = E_SYS;
					CMD_DEBUG("Parameter error!\r\n");
					goto error_exit;
				}

				while (length--) {
					*(uint8_t *)ram_address = (uint8_t)value;
					ram_address++;
				}

				break;

			case 'e':
				if (!xatoi(&argv[2], &ram_address) || !xatoi(&argv[3], &value)) {
					ercd = E_SYS;
					CMD_DEBUG("Parameter error!\r\n");
					goto error_exit;
				}

				length = argc - 3;

				while (length--) {
					*(uint8_t *)ram_address = (uint8_t)value;
					ram_address++;

					if (!xatoi(&argv[argc - length], &value)) {
						break;
					}
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

static CMD_TABLE_T mem_cmd = {"mem", "Operate the memory", cmd_mem, NULL};
/**
 * register memory command
 */
CMD_TABLE_T *register_ntshell_cmd_mem(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&mem_cmd, prev);
}

#endif /*NTSHELL_USE_CMDS_ARC_MEM*/