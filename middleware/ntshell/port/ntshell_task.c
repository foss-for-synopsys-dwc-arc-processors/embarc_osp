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

/**
 * \file
 * \brief  implementation of nt shell task
 */
#include <stdio.h>
#include <unistd.h>

#include "arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"

#define DEBUG
#include "embARC_debug.h"

#include "cmd.h"
#include "ntshell_task.h"


static int func_ntshell_read(char *buf, int cnt, void *extobj);
static int func_ntshell_write(const char *buf, int cnt, void *extobj);

#define NTSHELL_TASK_CHECK_EXP(EXPR, ERROR_CODE)     CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

/**
 * \brief NT-Shell library read need function
 * \details this function will be blocked until read  specified number of bytes
 *
 * \param buf read buffer
 * \param cnt bytes count need to read
 *
 * \retval 0 success and the accurrate read count
 * \retval !0 fail
 */
static int func_ntshell_read(char *buf, int cnt, void *extobj)
{
    int ercd = 0;
    NTSHELL_IO *p_nt_io;

    NTSHELL_TASK_CHECK_EXP(extobj!=NULL, -1);
    NTSHELL_TASK_CHECK_EXP(buf!=NULL, -1);
    NTSHELL_TASK_CHECK_EXP(cnt>0, -1);

    p_nt_io = (NTSHELL_IO *)extobj;

    if (_arc_usually(p_nt_io->nt_read != NULL)) { /* check read func is valid */
        ercd = p_nt_io->nt_read((void *)buf, cnt);
        if (ercd <= 0) {
            ercd = -1;
            *buf = 0;
        } else {
            ercd = 0;
        }
    } else {
        ercd = -1;
    }

error_exit:
    return ercd;
}

/**
 * \brief NT-Shell Library write need fuction
 * \details this function will be blocked until write certain number of bytes
 *
 * \param buf write buffer
 * \param cnt byte number need to write
 *
 * \retval >0 success and the accurate count written
 * \retval <0 fail
 */
static int func_ntshell_write(const char *buf, int cnt, void *extobj)
{
    int ercd = 0;
    NTSHELL_IO *p_nt_io;

    NTSHELL_TASK_CHECK_EXP(extobj!=NULL, -1);
    NTSHELL_TASK_CHECK_EXP(buf!=NULL, -1);
    NTSHELL_TASK_CHECK_EXP(cnt>0, -1);

    p_nt_io = (NTSHELL_IO *)extobj;

    if (_arc_usually(p_nt_io->nt_write != NULL)) { /* check read func is valid */
        ercd = (int)p_nt_io->nt_write((const void *)buf, (uint32_t)cnt);
    } else {
        ercd = -1;
    }

error_exit:
    return ercd;
}

/**
 * \brief Console NT-Shell Task
 */
void ntshell_task(void *p_arg)
{
    int ercd = 0;
    NTSHELL_IO *p_nt_io;

    NTSHELL_TASK_CHECK_EXP(p_arg!=NULL, -1);

    p_nt_io = (NTSHELL_IO *)p_arg;

    if (p_nt_io->nt_ioinit() != E_OK) {
        DBG("NTSHELL IO Init Failed, Unable to run ntshell!\n");
        goto error_exit;
    }
    /** register exist commands */
    if (p_nt_io->cmd_tbl_head != NULL) {
        p_nt_io->cmd_tbl_tail = ntshell_register_cmds((CMD_TABLE_T *)(p_nt_io->cmd_tbl_head));  /** defined in cmds.c */
    }

    ntshell_init(p_nt_io->ntshell, func_ntshell_read, func_ntshell_write, ntshell_usrcmd_execute, p_arg);

    if (p_nt_io->nt_prompt != NULL) {
        ntshell_set_prompt(p_nt_io->ntshell, p_nt_io->nt_prompt);
    } else {
        ntshell_set_prompt(p_nt_io->ntshell, ">");
    }
    DBG("NTShell Task StartUp\n\r");
    ntshell_execute(p_nt_io->ntshell);   /* no return */

error_exit:
    return ;
}
