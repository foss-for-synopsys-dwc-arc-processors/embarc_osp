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
* \brief register peripheral operation commands
*/
#include "cmd_cfg.h"
#if NTSHELL_USE_CMDS_PERIPHERAL

#include "embARC.h"

#include "cmds_peripheral_cfg.h"

extern CMD_TABLE_T *register_ntshell_cmd_ledflash(CMD_TABLE_T *prev);
extern CMD_TABLE_T *register_ntshell_cmd_adc(CMD_TABLE_T *prev);
extern CMD_TABLE_T *register_ntshell_cmd_ledswbtn(CMD_TABLE_T *prev);
extern CMD_TABLE_T *register_ntshell_cmd_temp(CMD_TABLE_T *prev);

/** indicate whether the commands were registered */
CMD_TABLE_T *ntshell_register_cmds_peripheral(CMD_TABLE_T *prev)
{
	if (prev == NULL) {
		return NULL;
	}

	/* ledflash register */
#if NTSHELL_USE_CMDS_PERIPHERAL_LEDFLASH
	prev = register_ntshell_cmd_ledflash(prev);
#endif
	/* led, swt, btn, ledswbtn register */
#if NTSHELL_USE_CMDS_PERIPHERAL_LEDSWBTN
	prev = register_ntshell_cmd_ledswbtn(prev);
#endif
	/* adc register */
#if NTSHELL_USE_CMDS_PERIPHERAL_ADC
	prev = register_ntshell_cmd_adc(prev);
#endif
	/* temp register */
#if NTSHELL_USE_CMDS_PERIPHERAL_TEMP
	prev = register_ntshell_cmd_temp(prev);
#endif

	return prev;
}
#endif /* NTSHELL_USE_CMDS_PERIPHERAL*/
