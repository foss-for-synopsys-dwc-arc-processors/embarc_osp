/**
 * @file ntshell_task.h
 * @author Shinichiro Nakamura
 * @brief NT-Shell Task Entry
 */

/*
 * ===============================================================
 *  Natural Tiny Shell Task
 * ===============================================================
 * Copyright (c) 2010-2011 Shinichiro Nakamura
 *
 * Copyright (c) 2015 Synopsys, Inc.
 * Modified for port to ARC processor by Wayne Ren wei.ren@synopsys.com
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

#ifndef NTSHELL_TASK_H
#define NTSHELL_TASK_H

#include "stdint.h"

#include "ntshell_usrcmd.h"

void ntshell_task(void *p_arg);

typedef struct ntshell_io {
	ntshell_t   *ntshell ;			/*!< ntshell excuting object */
	CMD_TABLE_T *cmd_tbl_head;	/*!< ntshell command table head */
	CMD_TABLE_T *cmd_tbl_tail;		/*!< ntshell command table tail,
						after excute ntshell register command */
	const char *nt_prompt;			/*!< ntshell command excute prompt	*/
	void *extra_info;				/*!< ntshell extra information */
	int32_t  (*nt_ioinit)  (void);			/*!< init ntshell related io */
	int32_t  (*nt_read)    (void *buf, uint32_t cnt);	/*!< ntshell read data 	*/
	int32_t  (*nt_write)   (const void *buf, uint32_t cnt);	/*!< ntshell write data	*/
	void     (*nt_printf)  (const char *fmt, ...);		/*!< ntshell command use printf */
} NTSHELL_IO, * NTSHELL_IO_PTR;

extern NTSHELL_IO* get_ntshell_io(uint32_t id);
#endif

