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
* \brief register commands
*/

#include "cmd.h"
#include "embARC_toolchain.h"
#include "arc_builtin.h"

#include "cmds_arc.h"
#include "cmds_peripheral.h"
#include "cmds_extend.h"
#include "cmds_fs.h"
#include "ntshell_common.h"

EMBARC_WEAK CMD_TABLE_T *ntshell_register_cmds_app(CMD_TABLE_T *prev)
{
	return prev;
}

/** indicate whether the commands were registered */
CMD_TABLE_T *ntshell_register_cmds(CMD_TABLE_T *prev)
{
	if (prev == NULL) {
		return NULL;
	}

	/* help command was registered default,
		so just register info command */
	prev = ntshell_register_cmd_info(prev);

	/* arc kernel command register */
#if NTSHELL_USE_CMDS_ARC
	prev = ntshell_register_cmds_arc(prev);
#endif
	/* peripheral command register */
#if NTSHELL_USE_CMDS_PERIPHERAL
	prev = ntshell_register_cmds_peripheral(prev);
#endif
	/* extend command register*/
#if NTSHELL_USE_CMDS_EXTEND
	prev = ntshell_register_cmds_extend(prev);
#endif
	/* filesystem related command register */
#if NTSHELL_USE_CMDS_FS
	prev = ntshell_register_cmds_fs(prev);
#endif
	/* register user defined cmds */
	prev = ntshell_register_cmds_app(prev);
	return prev;
}