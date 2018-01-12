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
 * \brief NTShell peripheral related commands configuration
 */

#ifndef _CMD_PERIPHERAL_CFG_H_
#define _CMD_PERIPHERAL_CFG_H_

#include "cmd_cfg.h"

#if NTSHELL_USE_CMDS_PERIPHERAL

#ifndef NTSHELL_USE_CMDS_PERIPHERAL_LEDFLASH
#define NTSHELL_USE_CMDS_PERIPHERAL_LEDFLASH		(1) /*!< use commands defined in cmd_ledflash.c*/
#endif

#ifndef NTSHELL_USE_CMDS_PERIPHERAL_LEDSWBTN
#define NTSHELL_USE_CMDS_PERIPHERAL_LEDSWBTN		(1) /*!< use commands defined in cmd_ledswbtn.c*/
#endif

#ifndef NTSHELL_USE_CMDS_PERIPHERAL_ADC
#define NTSHELL_USE_CMDS_PERIPHERAL_ADC			(1) /*!< use commands defined in cmd_adc.c*/
#endif

#ifndef NTSHELL_USE_CMDS_PERIPHERAL_TEMP
#define NTSHELL_USE_CMDS_PERIPHERAL_TEMP		(1) /*!< use commands defined in cmd_temp.c*/
#endif

#else /* Common commands disabled */

#define NTSHELL_USE_CMDS_PERIPHERAL_LEDFLASH		(0) /*!< don't use commands defined in cmd_ledflash.c*/
#define NTSHELL_USE_CMDS_PERIPHERAL_LEDSWBTN		(0) /*!< don't use commands defined in cmd_ledswbtn.c*/
#define NTSHELL_USE_CMDS_PERIPHERAL_ADC			(0) /*!< don't use commands defined in cmd_adc.c*/
#define NTSHELL_USE_CMDS_PERIPHERAL_TEMP		(0) /*!< don't use commands defined in cmd_temp.c*/
#endif /* NTSHELL_USE_CMDS_PERIPHERAL*/

#endif /* _CMD_PERIPHERAL_CFG_H_*/