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
 * \defgroup	BOARD_EMSDP_DRV_MID_NTSHELL_IO	EMSDP NTShell Middleware IO Driver
 * \ingroup	BOARD_EMSDP_DRIVER
 * \brief	EMSDP NTShell Middleware IO Driver
 * \details
 *		Realize the driver which the middleware ntshell need, the driver implementation
 *	is based on the standard of ntshell_io structure, the ntshell io can be uart,
 *	network and other similar io driver.
 */

/**
 * \file
 * \ingroup	BOARD_EMSDP_DRV_MID_NTSHELL_IO
 * \brief	middleware ntshell io interface driver
 */

/**
 * \addtogroup	BOARD_EMSDP_DRV_MID_NTSHELL_IO
 * @{
 */
#include "embARC_toolchain.h"

#ifdef MID_NTSHELL /* only available when enable ntshell middleware */
#include "ntshell_io.h"

/** get ntshell interface by index */
NTSHELL_IO* get_ntshell_io(uint32_t id)
{
	switch (id) {
#if (USE_EMSDP_NTSHELL_UART_1)
		case EMSDP_NTSHELL_0_ID:
			return &ntshell_uart_1;
			break;
#endif
#if (USE_EMSDP_NTSHELL_NETCONN_0)
		case EMSDP_NTSHELL_1_ID:
			return &ntshell_netconn_0;
			break;
#endif
		default:
			break;
	}
	return NULL;
}

#endif /** MID_NTSHELL */

/** @} end of group BOARD_EMSDP_DRV_MID_NTSHELL_IO */
