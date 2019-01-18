/*------------------------------------------
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

#include "embARC.h"
#include "container1.h"
#include "container2.h"
#include "background_container.h"

/* General container set-up and main application */

static CONTAINER_AC_TABLE g_container1_act[] = {
	{init_secret, 0, SECURESHIELD_AC_INTERFACE},
	{operate_secret, 3, SECURESHIELD_AC_INTERFACE}
 };

static CONTAINER_AC_TABLE g_container2_act[] = {
	{trusted_ops, 0, SECURESHIELD_AC_INTERFACE}
};

static CONTAINER_AC_TABLE g_main_container_act[] = {
#ifdef BOARD_EMSK
#define PERIPHERAL_ADDR_BASE 0xf0000000
	/* PINMUX, UART1 and GPIO0 are required */
	{(void *)(PERIPHERAL_ADDR_BASE + REL_REGBASE_PINMUX), 0x1000, SECURESHIELD_ACDEF_UPERIPH},
	{(void *)(PERIPHERAL_ADDR_BASE + REL_REGBASE_UART1), 0x1000, SECURESHIELD_ACDEF_UPERIPH},
	{(void *)(PERIPHERAL_ADDR_BASE + REL_REGBASE_GPIO0), 0x1000, SECURESHIELD_ACDEF_UPERIPH},
	{default_interrupt_handler, INTNO_GPIO, SECURESHIELD_AC_IRQ},
	{default_interrupt_handler, INTNO_UART1, SECURESHIELD_AC_IRQ},
#endif
	/* Timer1, Timer 0, RTC and TIMIE BCR are accessible for background container*/
#if SECURESHIELD_VERSION == 1
	{(void *)0x21, 0x3, SECURESHIELD_AC_AUX},
	{(void *)0x100, 0x6, SECURESHIELD_AC_AUX},
	{(void *)0x75, 0x1, SECURESHIELD_AC_AUX},
#endif
	{soft_interrupt1, INTNO_SWI, SECURESHIELD_AC_IRQ},
	{default_interrupt_handler, INTNO_TIMER0, SECURESHIELD_AC_IRQ}
};

/* set the access control table of background container */
SECURESHIELD_CONTAINER_BACKGROUND(g_main_container_act);

/* configure the secret container */
/* container1 is secure container, it has access to most of the system resources */
SECURESHIELD_CONTAINER_CONFIG(container1, g_container1_act, 1024, SECURESHIELD_CONTAINER_SECURE);
SECURESHIELD_CONTAINER_CONFIG(container2, g_container2_act, 1024);