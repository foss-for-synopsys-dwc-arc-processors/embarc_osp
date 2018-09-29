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
 * \brief filesystem NTShell commands configuration
 */

#ifndef _NTSHELL_CMD_CFG_FS_H_
#define _NTSHELL_CMD_CFG_FS_H_

#include "cmd_cfg.h"

#if NTSHELL_USE_CMDS_FS

#ifdef MID_FATFS

#ifndef NTSHELL_USE_CMDS_FS_LS
#define NTSHELL_USE_CMDS_FS_LS		(1) /*!< use commands defined in cmd_ls.c*/
#endif

#ifndef NTSHELL_USE_CMDS_FS_CD
#define NTSHELL_USE_CMDS_FS_CD		(1) /*!< use commands defined in cmd_cd.c*/
#endif

#ifndef NTSHELL_USE_CMDS_FS_RM
#define NTSHELL_USE_CMDS_FS_RM		(1) /*!< use commands defined in cmd_rm.c*/
#endif

#ifndef NTSHELL_USE_CMDS_FS_MKDIR
#define NTSHELL_USE_CMDS_FS_MKDIR	(1) /*!< use commands defined in cmd_mkdir.c*/
#endif

#ifndef NTSHELL_USE_CMDS_FS_CAT
#define NTSHELL_USE_CMDS_FS_CAT		(1) /*!< use commands defined in cmd_cat.c*/
#endif

#ifndef NTSHELL_USE_CMDS_FS_CP
#define NTSHELL_USE_CMDS_FS_CP		(1) /*!< use commands defined in cmd_cp.c*/
#endif

#ifndef NTSHELL_USE_CMDS_FS_MV
#define NTSHELL_USE_CMDS_FS_MV		(1) /*!< use commands defined in cmd_mv.c*/
#endif

#ifndef NTSHELL_USE_CMDS_FS_PWD
#define NTSHELL_USE_CMDS_FS_PWD		(1) /*!< use commands defined in cmd_pwd.c*/
#endif

#ifndef NTSHELL_USE_CMDS_FS_CLS
#define NTSHELL_USE_CMDS_FS_CLS		(1) /*!< use commands defined in cmd_pwd.c*/
#endif

#ifndef NTSHELL_USE_CMDS_FS_TOUCH
#define NTSHELL_USE_CMDS_FS_TOUCH	(1) /*!< use commands defined in cmd_touch.c*/
#endif

#ifndef NTSHELL_USE_CMDS_FS_YMODEM
#define NTSHELL_USE_CMDS_FS_YMODEM	(1) /*!< use commands defined in cmd_ymodem.c*/
#endif

#ifndef NTSHELL_USE_CMDS_FS_DISK
#define NTSHELL_USE_CMDS_FS_DISK	(1) /*!< use commands defined in cmd_disk.c*/
#endif

#ifndef NTSHELL_USE_CMDS_FS_LOAD
#define NTSHELL_USE_CMDS_FS_LOAD	(1) /*!< use commands defined in cmd_load.c*/
#endif

#if defined(BOARD_IOTDK)
#ifndef NTSHELL_USE_CMDS_FS_FLASH
#define NTSHELL_USE_CMDS_FS_FLASH	(1) /*!< use commands defined in cmd_pwd.c*/
#endif
#endif

#if defined(BOARD_EMSK)
#ifndef NTSHELL_USE_CMDS_FS_SPIRW
#define NTSHELL_USE_CMDS_FS_SPIRW	(1) /*!< use commands defined in cmd_spirw.c*/
#endif

#ifndef NTSHELL_USE_CMDS_FS_MLOAD
#define NTSHELL_USE_CMDS_FS_MLOAD	(1) /*!< use commands defined in cmd_mload.c*/
#endif
#endif

#endif /* MID_FATFS */

#else /* Fatfs Related commands disabled */

#define NTSHELL_USE_CMDS_FS_LS		(0) /*!< use commands defined in cmd_ls.c*/
#define NTSHELL_USE_CMDS_FS_CD		(0) /*!< use commands defined in cmd_cd.c*/
#define NTSHELL_USE_CMDS_FS_RM		(0) /*!< use commands defined in cmd_rm.c*/
#define NTSHELL_USE_CMDS_FS_MKDIR	(0) /*!< use commands defined in cmd_mkdir.c*/
#define NTSHELL_USE_CMDS_FS_CAT		(0) /*!< use commands defined in cmd_cat.c*/
#define NTSHELL_USE_CMDS_FS_CP		(0) /*!< use commands defined in cmd_cp.c*/
#define NTSHELL_USE_CMDS_FS_MV		(0) /*!< use commands defined in cmd_mv.c*/
#define NTSHELL_USE_CMDS_FS_PWD		(0) /*!< use commands defined in cmd_pwd.c*/
#define NTSHELL_USE_CMDS_FS_TOUCH	(0) /*!< use commands defined in cmd_touch.c*/
#define NTSHELL_USE_CMDS_FS_YMODEM	(0) /*!< use commands defined in cmd_ymodem.c*/
#define NTSHELL_USE_CMDS_FS_DISK	(0) /*!< use commands defined in cmd_disk.c*/
#define NTSHELL_USE_CMDS_FS_LOAD	(0) /*!< use commands defined in cmd_load.c*/
#define NTSHELL_USE_CMDS_FS_SPIRW	(0) /*!< use commands defined in cmd_spirw.c*/
#define NTSHELL_USE_CMDS_FS_FLASH	(0) /*!< use commands defined in cmd_pwd.c*/
#define NTSHELL_USE_CMDS_FS_CLS		(0) /*!< use commands defined in cmd_pwd.c*/

#endif /* NTSHELL_USE_CMDS_FS*/

#endif /* _NTSHELL_CMD_CFG_FS_H_*/