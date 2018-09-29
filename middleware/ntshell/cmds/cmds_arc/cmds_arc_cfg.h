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
 * \brief arc kernel NTShell commands configuration
 */

#ifndef _CMDS_ARC_CFG_H_
#define _CMDS_ARC_CFG_H_

#include "cmd_cfg.h"

#if NTSHELL_USE_CMDS_ARC

#ifndef NTSHELL_USE_CMDS_ARC_AUX
#define NTSHELL_USE_CMDS_ARC_AUX		(1) /*!< use commands defined in cmd_aux.c*/
#endif

#ifndef NTSHELL_USE_CMDS_ARC_MAP
#define NTSHELL_USE_CMDS_ARC_MAP		(1) /*!< use commands defined in cmd_map.c*/
#endif

#ifndef	NTSHELL_USE_CMDS_ARC_MEM
#define NTSHELL_USE_CMDS_ARC_MEM		(1) /*!< use commands defined in cmd_mem.c*/
#endif

#else /* ARC kernel Related commands disabled */

#define NTSHELL_USE_CMDS_ARC_AUX		(0) /*!< don't use commands defined in cmd_aux.c*/
#define NTSHELL_USE_CMDS_ARC_MAP		(0) /*!< don't use commands defined in cmd_map.c*/
#define NTSHELL_USE_CMDS_ARC_MEM		(0) /*!< use commands defined in cmd_mem.c*/

#endif /* NTSHELL_USE_CMDS_ARC*/

#endif /* _CMDS_ARC_CFG_H_*/