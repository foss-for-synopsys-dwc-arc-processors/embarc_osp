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
* \brief show the memory map of bootloader
*/

#include "cmds_arc_cfg.h"
#if NTSHELL_USE_CMDS_ARC_MAP

#include "ntshell_common.h"

static NTSHELL_IO_PREDEF;
/*
  Example of sections in memory map

.vector  text   00000000  0000007f  00000080
.text    text   00000100  0000e898  0000e79C
.rodata  lit    0000e89c  0000e99c  00000104
.data    data   80000000  8000130f  00001310
.bss     bss    80001310  80001ee3  00000bd4
*/

extern uint32_t _f_vector;
extern uint32_t _e_vector;
extern uint32_t _f_text;
extern uint32_t _e_text;
extern uint32_t _f_rodata;
extern uint32_t _e_rodata;
extern uint32_t _f_data;
extern uint32_t _e_data;
extern uint32_t _f_bss;
extern uint32_t _e_bss;
extern uint32_t _f_stack;
extern uint32_t _e_stack;
extern uint32_t _f_heap;
extern uint32_t _e_heap;

/* show help of command dump */
static void cmd_map_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]\r\n"
		"Show the memory map of bootloader\r\n"
		"  -h/H/?    Show the help information\r\n"
		"Examples: \r\n"
		"  map       Show the memory map of bootloader\r\n"
		"  map -h    Show the help information\r\n", cmd_name);
error_exit:
	return;
}

int cmd_map (int argc, char **argv, void *extobj)
{
	int32_t size;
	char str[40];
	int32_t ercd = E_OK;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	if (argc == 2 && ntlibc_strcmp(argv[1],"-h") == 0x00) {
		cmd_map_help(argv[0], extobj);
		goto error_exit;
	}

	if (argc > 1) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	}

	CMD_DEBUG("**********************************\r\n");
	CMD_DEBUG("** VECTOR (%d bytes)\r\n", (int32_t)((int32_t)(&_e_vector) - (int32_t)(&_f_vector)));
	CMD_DEBUG("**********************************\r\n");
	CMD_DEBUG(" _f_vector = 0x%08x\r\n",(int32_t)&_f_vector);
	CMD_DEBUG(" _e_vector = 0x%08x\r\n",(int32_t)&_e_vector);

	CMD_DEBUG("**********************************\r\n");
	CMD_DEBUG("** TEXT (%dKB)\r\n", (int32_t)((int32_t)(&_e_text) - (int32_t)(&_f_text))/1024);
	CMD_DEBUG("**********************************\r\n");
	CMD_DEBUG(" _f_text   = 0x%08x\r\n",(int32_t)&_f_text);
	CMD_DEBUG(" _e_text   = 0x%08x\r\n",(int32_t)&_e_text);

	CMD_DEBUG("**********************************\r\n");
	CMD_DEBUG("** RODATA (%dKB)\r\n", (int32_t)((int32_t)(&_e_rodata) - (int32_t)(&_f_rodata))/1024);
	CMD_DEBUG("**********************************\r\n");
	CMD_DEBUG(" _f_rodata = 0x%08x\r\n",(int32_t)&_f_rodata);
	CMD_DEBUG(" _e_rodata = 0x%08x\r\n",(int32_t)&_e_rodata);

	CMD_DEBUG("**********************************\r\n");
	size = (int32_t)((int32_t)(&_e_data) - (int32_t)(&_f_data));

	if (size >= (1024)) {
		xsprintf(str,"%dKB",size/1024);
	} else {
		xsprintf(str,"%dbytes",size);
	}

	CMD_DEBUG("** DATA (%s)\r\n", str);
	CMD_DEBUG("**********************************\r\n");
	CMD_DEBUG(" _f_data   = 0x%08x\r\n",(int32_t)&_f_data);
	CMD_DEBUG(" _e_data   = 0x%08x\r\n",(int32_t)&_e_data);

	size = (int32_t)((int32_t)(&_e_bss) - (int32_t)(&_f_bss));

	if (size >= (1024*1024)) {
		xsprintf(str,"%dMB",size/1024/1024);
	} else {
		xsprintf(str,"%dKB",size/1024);
	}

	CMD_DEBUG("**********************************\r\n");
	CMD_DEBUG("** BSS (%s)\r\n", str);
	CMD_DEBUG("**********************************\r\n");
	CMD_DEBUG(" _f_bss    = 0x%08x\r\n",(int32_t)&_f_bss);
	CMD_DEBUG(" _e_bss    = 0x%08x\r\n",(int32_t)&_e_bss);

	size = (int32_t)((int32_t)(&_e_stack) - (int32_t)(&_f_stack));

	if (size >= (1024)) {
		xsprintf(str,"%dKB",size/1024);
	} else {
		xsprintf(str,"%dbytes",size);
	}

	CMD_DEBUG("**********************************\r\n");
	CMD_DEBUG("** STACK (%s)\r\n", str );
	CMD_DEBUG("**********************************\r\n");
	CMD_DEBUG(" _f_stack  = 0x%08x\r\n",(int32_t)&_f_stack);
	CMD_DEBUG(" _e_stack  = 0x%08x\r\n",(int32_t)&_e_stack);

	size = (int32_t)((int32_t)(&_e_heap) - (int32_t)(&_f_heap));

	if (size >= (1024)) {
		xsprintf(str,"%dKB",size/1024);
	} else {
		xsprintf(str,"%dbytes",size);
	}

	CMD_DEBUG("**********************************\r\n");
	CMD_DEBUG("** HEAP (%s)\r\n", str);
	CMD_DEBUG("**********************************\r\n");
	CMD_DEBUG(" _f_heap  = 0x%08x\r\n",(int32_t)&_f_heap);
	CMD_DEBUG(" _e_heap  = 0x%08x\r\n",(int32_t)&_e_heap);

	return E_OK;
error_exit:
	return ercd;
}

static CMD_TABLE_T map_cmd = {"map", "Show the memory map of bootloader", cmd_map, NULL};
/**
 * register map command
 */
CMD_TABLE_T *register_ntshell_cmd_map(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&map_cmd, prev);
}

#endif /* NTSHELL_USE_CMDS_ARC_MAP */
