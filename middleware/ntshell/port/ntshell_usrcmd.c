/**
 * \file
 * \brief Implementation of natural tiny shell command task
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

/*
 * Modified by Qiang Gu Qiang.Gu@synopsys.com, Synopsys, Inc.
 */

#include "stddef.h"
#include "stdint.h"
#include "string.h"

#include "arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"

#define  DBG_MORE
#define  DEBUG
#include "embARC_debug.h"

#include "ntshell_config.h"
#include "ntshell_usrcmd.h"
#include "ntshell_task.h"


static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj);
static int usrcmd_help(int argc, char **argv, void *extobj);
static int usrcmd_info(int argc, char **argv, void *extobj);

/**
 * \brief NT-Shell Command Excute
 * \details
 *
 * \param text input text
 *
 * \retval 0 success
 * \retval !0 failed
 */
int ntshell_usrcmd_execute(const char *text, void *extobj)
{
    /*
     * use ntopt_parse to parse input text
     */
    return ntopt_parse(text, usrcmd_ntopt_callback, extobj);
}

/**
 * \brief run corresponding commands
 * \details
 *
 * \param argc arguments count
 * \param argv arguments
 *
 * \retval 0 success
 * \retval !0 failed
 */
static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj)
{
    NTSHELL_IO *p_nt_io;
    CMD_TABLE_T *cmd_t;

    /*
     * if no arguments input just return
     * you may just type enter here
     */
    if (_arc_rarely((argc == 0) || (extobj == NULL))) {
        return 0;
    }
    p_nt_io = (NTSHELL_IO *)extobj;
    cmd_t = (CMD_TABLE_T *)(p_nt_io->cmd_tbl_head);
    /*
     * try to find a command equal to your input command
     * and excute its function
     */
    while ((cmd_t!=NULL) && (cmd_t->cmd != NULL)) {
        if (ntlibc_strcmp((const char *)argv[0], cmd_t->cmd) == 0) {
            return cmd_t->func(argc, argv, extobj);
        }
        cmd_t = cmd_t->next;
    }

    /*
     * if run to here, there is no such command
     */
    {
        p_nt_io->nt_printf("Unknown command:%s. \r\n(help: display help.)\r\n", argv[0]);
    }
    return 0;
}

/**
 * \brief help command
 * \details
 *
 * \param argc argument count
 * \param argv arguments
 *
 * \retval 0 success
 * \retval !0 fail
 */
static int usrcmd_help(int argc, char **argv, void *extobj)
{
    NTSHELL_IO *p_nt_io;
    CMD_TABLE_T *cmd_t;

    if (_arc_rarely((extobj == NULL))) {
        return -1;
    }

    p_nt_io = (NTSHELL_IO *)extobj;
    cmd_t = (CMD_TABLE_T *)(p_nt_io->cmd_tbl_head);

    /*
     * display all available command help and discription
     */
    while ((cmd_t!=NULL) && (cmd_t->cmd != NULL)) {
    	if(strlen(cmd_t->cmd) < 8){
        	p_nt_io->nt_printf("%s\t\t:%s\r\n", cmd_t->cmd, cmd_t->desc);
    	}
        else{
        	p_nt_io->nt_printf("%s\t:%s\r\n", cmd_t->cmd, cmd_t->desc);
        }
        cmd_t = cmd_t->next;
    }
    return 0;
}

/**
 * \brief info command
 * \details
 *
 * \param argc argument count
 * \param argv arguments
 *
 * \retval 0 success
 * \retval !0 fail
 */
static int usrcmd_info(int argc, char **argv, void *extobj)
{
    int major, minor, release;
    NTSHELL_IO *p_nt_io;

    if (_arc_rarely((extobj == NULL))) {
        return -1;
    }
    p_nt_io = (NTSHELL_IO *)extobj;

    if (argc != 2) {
        static const char *msg = "info sys\r\ninfo ver";
        p_nt_io->nt_printf("%s\r\n", msg);
        return 0;
    }
    if (ntlibc_strcmp(argv[1], "sys") == 0) {
        static const char *msg = "NT-Shell Console System";
        p_nt_io->nt_printf("%s\r\n", msg);
        return 0;
    }
    if (ntlibc_strcmp(argv[1], "ver") == 0) {
        ntshell_version(&major, &minor, &release);
        p_nt_io->nt_printf("Version:%d.%d.%d\r\n", major, minor, release);
        return 0;
    }
    {
        static const char *msg = "Unknown sub command.";
        p_nt_io->nt_printf("%s\r\n", msg);
    }
    return -1;
}

/**
 * \brief   register a new ntshell command
 * \param   new        new command to be register
 * \param   prev       command insert before
 * \retval
 *          NULL    failed to register command
 *          !NULL   the ntshell command inserted
 */
CMD_TABLE_T* ntshell_usrcmd_register(CMD_TABLE_T *new, CMD_TABLE_T *prev)
{
    if ( _arc_usually((new!=NULL) && (prev!=NULL)) ) {
        if (_arc_usually((new->cmd != NULL) && (new->func!=NULL))) {
            while (prev->next != NULL) {
                if (ntlibc_strcmp(prev->cmd, new->cmd) == 0) {
                    dbg_printf(DBG_MORE_INFO, "ntshell command %s already existed!\r\n", prev->cmd);
                    return NULL;
                }
                prev = prev->next;
            }
            if (prev->next != NULL) { /* if prev is not end of list */
                new->next = prev->next;
            }
            prev->next = new;
            dbg_printf(DBG_MORE_INFO, "ntshell command %s(%s) was registered!\r\n", new->cmd, new->desc);
            return new;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

/**
 * \brief   find the tail of the command table
 * \param   cmd_t      the input command table
 * \retval
 *         NULL    not exist tail
 *         !NULL   tail of table
 */
CMD_TABLE_T* ntshell_usrcmd_tail(CMD_TABLE_T *cmd_t)
{
    if ( _arc_usually(cmd_t!=NULL) ) { /* find the tail */
        while (cmd_t->next != NULL) {
            cmd_t = cmd_t->next;
        }
        return cmd_t;
    }
    return NULL;
}

/** Goto main function in ntshell */
static int usrcmd_goto_main(int argc, char **argv, void *extobj) {
	return _arc_goto_main(argc, argv);
}

static CMD_TABLE_T main_cmd = {"main", "Call main function", usrcmd_goto_main, NULL};
static CMD_TABLE_T info_cmd = {"info", "Information", usrcmd_info, NULL};
CMD_TABLE_T help_cmd = {"help", "Help", usrcmd_help, NULL};

CMD_TABLE_T *ntshell_register_cmd_help(CMD_TABLE_T *prev)
{
    return ntshell_usrcmd_register(&help_cmd, prev);
}

CMD_TABLE_T *ntshell_register_cmd_info(CMD_TABLE_T *prev)
{
    prev = ntshell_usrcmd_register(&info_cmd, prev);
    return ntshell_usrcmd_register(&main_cmd, prev);
}