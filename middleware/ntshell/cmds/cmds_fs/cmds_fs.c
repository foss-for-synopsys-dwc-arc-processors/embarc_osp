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
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIA\BILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */

/**
* \file
* \brief register filesystem operation commands
*/
#include "cmd_cfg.h"
#if NTSHELL_USE_CMDS_FS
#include "cmds_fs_cfg.h"
#include "cmd_fs_common.h"

/*ls register*/
#if NTSHELL_USE_CMDS_FS_LS
extern CMD_TABLE_T *register_ntshell_cmd_ls(CMD_TABLE_T *prev);
#endif

/*cd register*/
#if NTSHELL_USE_CMDS_FS_CD
extern CMD_TABLE_T *register_ntshell_cmd_cd(CMD_TABLE_T *prev);
#endif

/*mkdir register*/
#if NTSHELL_USE_CMDS_FS_MKDIR
extern CMD_TABLE_T *register_ntshell_cmd_mkdir(CMD_TABLE_T *prev);
#endif

/*rm register*/
#if NTSHELL_USE_CMDS_FS_RM
extern CMD_TABLE_T *register_ntshell_cmd_rm(CMD_TABLE_T *prev);
#endif

/*cat register*/
#if NTSHELL_USE_CMDS_FS_CAT
extern CMD_TABLE_T *register_ntshell_cmd_cat(CMD_TABLE_T *prev);
#endif

/*cp register*/
#if NTSHELL_USE_CMDS_FS_CP
extern CMD_TABLE_T *register_ntshell_cmd_cp(CMD_TABLE_T *prev);
#endif

/*mv register*/
#if NTSHELL_USE_CMDS_FS_MV
extern CMD_TABLE_T *register_ntshell_cmd_mv(CMD_TABLE_T *prev);
#endif

/*pwd register*/
#if NTSHELL_USE_CMDS_FS_PWD
extern CMD_TABLE_T *register_ntshell_cmd_pwd(CMD_TABLE_T *prev);
#endif

/*cls register*/
#if NTSHELL_USE_CMDS_FS_CLS
extern CMD_TABLE_T *register_ntshell_cmd_cls(CMD_TABLE_T *prev);
#endif

/*touch register*/
#if NTSHELL_USE_CMDS_FS_TOUCH
extern CMD_TABLE_T *register_ntshell_cmd_touch(CMD_TABLE_T *prev);
#endif

/*ymodem register*/
#if NTSHELL_USE_CMDS_FS_YMODEM
extern CMD_TABLE_T *register_ntshell_cmd_ymodem(CMD_TABLE_T *prev);
#endif

/*disk register*/
#if NTSHELL_USE_CMDS_FS_DISK
extern CMD_TABLE_T *register_ntshell_cmd_disk(CMD_TABLE_T *prev);
#endif

/*load register*/
#if NTSHELL_USE_CMDS_FS_LOAD
extern CMD_TABLE_T *register_ntshell_cmd_load(CMD_TABLE_T *prev);
#endif

/*load register*/
#if NTSHELL_USE_CMDS_FS_FLASH
extern CMD_TABLE_T *register_ntshell_cmd_flash(CMD_TABLE_T *prev);
#endif

/*spirw register*/
#if NTSHELL_USE_CMDS_FS_SPIRW
extern CMD_TABLE_T *register_ntshell_cmd_spirw(CMD_TABLE_T *prev);
#endif

/*mload register*/
#if NTSHELL_USE_CMDS_FS_MLOAD
extern CMD_TABLE_T *register_ntshell_cmd_mload(CMD_TABLE_T *prev);
#endif

/** indicate whether the commands were registered */
CMD_TABLE_T *ntshell_register_cmds_fs(CMD_TABLE_T *prev)
{
	if (prev == NULL) {
		return NULL;
	}

	/* ls register */
#if NTSHELL_USE_CMDS_FS_LS
	prev = register_ntshell_cmd_ls(prev);
#endif
	/* cd register */
#if NTSHELL_USE_CMDS_FS_CD
	prev = register_ntshell_cmd_cd(prev);
#endif
	/* rm register */
#if NTSHELL_USE_CMDS_FS_RM
	prev = register_ntshell_cmd_rm(prev);
#endif
	/* makdir register */
#if NTSHELL_USE_CMDS_FS_MKDIR
	prev = register_ntshell_cmd_mkdir(prev);
#endif
	/* cat register */
#if NTSHELL_USE_CMDS_FS_CAT
	prev = register_ntshell_cmd_cat(prev);
#endif
	/* cp register */
#if NTSHELL_USE_CMDS_FS_CP
	prev = register_ntshell_cmd_cp(prev);
#endif
	/* mv register */
#if NTSHELL_USE_CMDS_FS_MV
	prev = register_ntshell_cmd_mv(prev);
#endif
	/* pwd register */
#if NTSHELL_USE_CMDS_FS_PWD
	prev = register_ntshell_cmd_pwd(prev);
#endif
	/* cls register */
#if NTSHELL_USE_CMDS_FS_CLS
	prev = register_ntshell_cmd_cls(prev);
#endif
	/* touch register */
#if NTSHELL_USE_CMDS_FS_TOUCH
	prev = register_ntshell_cmd_touch(prev);
#endif
	/* ymodem register */
#if NTSHELL_USE_CMDS_FS_YMODEM
	prev = register_ntshell_cmd_ymodem(prev);
#endif
	/* disk register */
#if NTSHELL_USE_CMDS_FS_DISK
	prev = register_ntshell_cmd_disk(prev);
#endif
	/* load register */
#if NTSHELL_USE_CMDS_FS_LOAD
	prev = register_ntshell_cmd_load(prev);
#endif
	/* flash register */
#if NTSHELL_USE_CMDS_FS_FLASH
	prev = register_ntshell_cmd_flash(prev);
#endif
	/* spirw register */
#if NTSHELL_USE_CMDS_FS_SPIRW
	prev = register_ntshell_cmd_spirw(prev);
#endif
	/*spirw register*/
#if NTSHELL_USE_CMDS_FS_MLOAD
	prev = register_ntshell_cmd_mload(prev);
#endif

	return prev;
}
#endif /* NTSHELL_USE_CMDS_FS*/
