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
 * \brief  swap the endianess of data
 *
 */

#include "cmds_arc_cfg.h"
#if NTSHELL_USE_CMDS_ARC_SWAPE

#include "ntlibc.h"
#include "ntshell_common.h"

static NTSHELL_IO_PREDEF;

/* show help of command swape */
static void cmd_swape_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}
	CMD_DEBUG("Usage: %s [OPTION]... <VAL>\r\n"
		"Swap endianness of <VAL>, in order to test the instruction\r\n"
		"  -h/H/?    Show the help information\r\n"
		"  -s <VAL>  Swap endianness of <VAL>\r\n"
		"Examples: \r\n"
		"  swape -s 0x1234    Swap endianness of 0x1234, the result is 0x3412\r\n"
		"  swape -h    Show the help information\r\n", cmd_name);

error_exit:
	return;
}

int cmd_swape (int argc, char **argv, void *extobj)
{
	long val;
	int32_t short_flag = 0;
	int32_t index = 1;
	uint32_t hexvalue = 0;
	uint32_t orgvalue;
	int32_t ercd = E_OK;
	int32_t opt;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	if ((argc > 4) || (argc < 2)) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	}

	opterr = 0;
	optind = 1;

	while ((opt=getopt(argc, argv, "shH?")) != -1) {
		switch (opt) {
			case 'h':
			case '?':
			case 'H':
				cmd_swape_help(argv[0], extobj);
				goto error_exit;
				break;
			case 's':
				short_flag = 1;
				index = 2;
				if (!xatoi(&argv[index], &val)) {
					ercd = E_SYS;
					CMD_DEBUG("command error!\r\n");
					CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
					goto error_exit;
				}
				break;
			default:
				CMD_DEBUG("%s: unrecognized option:%c\r\n", argv[0], opt);
				CMD_DEBUG("Try '%s -h' for more information\r\n",argv[0]);
				break;
		}
	}

	if (short_flag) {
		hexvalue = _arc_swap16((unsigned short) val);
		orgvalue = (unsigned short) val;
	} else {
		hexvalue = _arc_swap32((unsigned int) val);
		orgvalue = (unsigned int) val;
	}

	CMD_DEBUG("0x%x after swape is 0x%x\r\n", orgvalue, hexvalue);

	return E_OK;

error_exit:
	return ercd;
}

static CMD_TABLE_T swape_cmd = {"swape", "Swap endianness of input variable", cmd_swape, NULL};
/**
 * register dump command
 */
CMD_TABLE_T * register_ntshell_cmd_swape(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&swape_cmd, prev);
}

#endif /* NTSHELL_USE_CMDS_ARC_SWAPE */
