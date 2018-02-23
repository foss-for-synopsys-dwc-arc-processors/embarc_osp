/**
 * @file ntshell_usrcmd.h
 * @author Shinichiro Nakamura
 * @brief ntshell user defined command definitions
 * @details
 */

/*
 * ===============================================================
 *  Natural Tiny Shell - User Command Module
 * ===============================================================
 * Copyright (c) 2010-2011 Shinichiro Nakamura
 *
 * Copyright (c) 2015 Synopsys, Inc.
 * Modified for port to ARC processor by Huaqi Fang huaqi.fang@synopsys.com
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * ===============================================================
 */

#ifndef NTSHELL_USRCMD_H
#define NTSHELL_USRCMD_H

#include "ntshell_config.h"

typedef int (* ntshell_call_func)(int argc, char **argv, void *extobj);

typedef struct cmd_table_t CMD_TABLE_T;

/**
 * @brief command table
 */
struct cmd_table_t {
	char *cmd;		/**< command name */
	char *desc;		/**< comand description */
	ntshell_call_func func;	/**< command related function */
	CMD_TABLE_T *next;	/**< next command */
};

#ifdef USE_NTSHELL_EXTOBJ
#define NTSHELL_IO_PREDEF			NTSHELL_IO *p_nt_io
#define NTSHELL_IO_GET(extobj)			p_nt_io = (NTSHELL_IO *)(extobj)
#define CMD_DEBUG(fmt, ...)			p_nt_io->nt_printf(fmt, ##__VA_ARGS__)
#define CMD_WRITE(data, cnt)			p_nt_io->nt_write((const void *)(data), (uint32_t)(cnt))
#define CMD_READ(data, cnt)			p_nt_io->nt_read((void *)(data), (uint32_t)(cnt))
#define VALID_EXTOBJ(extobj, ERROR_CODE)	CHECK_EXP(((extobj)!=NULL), ercd, ERROR_CODE, error_exit)
#define VALID_EXTOBJ_NORTN(extobj)		CHECK_EXP_NOERCD(((extobj)!=NULL), error_exit)
#else /* don't use ntshell extobj */
#define NTSHELL_IO_PREDEF
#define NTSHELL_IO_GET(extobj)
#define CMD_DEBUG(fmt, ...)		EMBARC_PRINTF(fmt, ##__VA_ARGS__)
#define CMD_WRITE(data, cnt)		console_putstr(data, cnt)
#define CMD_READ(data, cnt)		console_getstr(data, cnt)

#define VALID_EXTOBJ(EXPR, ERROR_CODE)
#define VALID_EXTOBJ_NORTN(extobj)
#endif


extern CMD_TABLE_T help_cmd;

CMD_TABLE_T *ntshell_register_cmd_help(CMD_TABLE_T *prev);
CMD_TABLE_T *ntshell_register_cmd_info(CMD_TABLE_T *prev);

CMD_TABLE_T* ntshell_usrcmd_register(CMD_TABLE_T *new, CMD_TABLE_T *prev);
CMD_TABLE_T* ntshell_usrcmd_tail(CMD_TABLE_T *cmd_t);

int ntshell_usrcmd_execute(const char *text, void *extobj);

#endif

