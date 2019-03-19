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
#ifndef _NSIM_H_
#define _NSIM_H_

#include "arc_feature_config.h"
#include "arc_em.h"

#include "drivers/uart/nsim_uart_obj.h"
#include "drivers/ntshell/ntshell_io.h"
#include "common/nsim_timer.h"

/** CPU Clock Frequency definition */
#if defined(BOARD_CPU_FREQ)
	/*!< Get cpu clock frequency definition from build system */
	#define CLK_CPU			(BOARD_CPU_FREQ)
#elif defined(ARC_FEATURE_CPU_CLOCK_FREQ)
	/*!< Get cpu clock frequency definition from tcf file */
	#define CLK_CPU			(ARC_FEATURE_CPU_CLOCK_FREQ)
#else
	/*!< Default cpu clock frequency */
	#define CLK_CPU			(1000000)
#endif

/* common macros must be defined by all boards */

#define BOARD_CONSOLE_UART_ID		NSIM_UART_0_ID
#define BOARD_CONSOLE_UART_BAUD		115200
#define BOARD_ONBOARD_NTSHELL_ID	NSIM_NTSHELL_0_ID
#define NTSHELL_CONSOLE_ID		(NSIM_NTSHELL_0_ID)

#define BOARD_SYS_TIMER_ID		TIMER_0
#define BOARD_SYS_TIMER_INTNO		INTNO_TIMER0
#define BOARD_SYS_TIMER_HZ		(1000)

/** board timer 1ms means what HZ count */
#define BOARD_SYS_TIMER_MS_HZ		(1000)
/** board ms counter convention based on the global timer counter */
#define BOARD_SYS_TIMER_MS_CONV		(BOARD_SYS_TIMER_MS_HZ/BOARD_SYS_TIMER_HZ)

#define BOARD_OS_TIMER_ID		TIMER_0
#define BOARD_OS_TIMER_INTNO		INTNO_TIMER0

#define BOARD_CPU_CLOCK			CLK_CPU

#define OSP_DELAY_OS_COMPAT_ENABLE	(1)
#define OSP_DELAY_OS_COMPAT_DISABLE	(0)

#define OSP_GET_CUR_SYSHZ()		(gl_nsim_sys_hz_cnt)
#define OSP_GET_CUR_MS()		(gl_nsim_ms_cnt)
#define OSP_GET_CUR_US()		board_get_cur_us()
#define OSP_GET_CUR_HWTICKS()		board_get_hwticks()

#define button_read(x) 			1
#define led_write(x, y)			EMBARC_PRINTF("led out: %x, %x\r\n", x, y)
#define BOARD_LED_MASK			0xff

#ifdef __cplusplus
extern "C" {
#endif

extern void board_init(void);
extern void board_timer_update(uint32_t precision);
extern void board_delay_ms(uint32_t ms, uint8_t os_compat);
extern uint64_t board_get_hwticks(void);
extern uint64_t board_get_cur_us(void);

#ifdef __cplusplus
}
#endif

#endif	/* _NSIM_H_ */
