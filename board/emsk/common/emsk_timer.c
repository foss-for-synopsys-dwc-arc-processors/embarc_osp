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
 * \defgroup	BOARD_EMSK_COMMON_TIMER	EMSK Common Timer Module
 * \ingroup	BOARD_EMSK_COMMON
 * \brief	provide basic board timer-related functions
 * \details
 * 		Provide a 1 MS (default) timer interrupt,
 *	provide a 64-bit counter value (no clear) count in the timer interrupt,
 *	provide MS-precision delay, with OS enabled-support delay
 */

/**
 * \file
 * \ingroup	BOARD_EMSK_COMMON_TIMER
 * \brief	provide emsk board timer-related functions
 */

/**
 * \addtogroup	BOARD_EMSK_COMMON_TIMER
 * @{
 */
#include "arc_builtin.h"
#include "arc.h"
#include "arc_timer.h"
#include "arc_exception.h"

#include "board.h"

#ifdef ENABLE_OS
#include "os_hal_inc.h"
#endif

#define MAX_SYS_COUNTER_VALUE		(0xffffffff)

#ifndef BOARD_SYS_TIMER_HZ
#define BOARD_SYS_TIMER_HZ		(1000)
#endif

/** emsk board timer interrupt reset count */
static uint32_t cyc_hz_count = (BOARD_CPU_CLOCK / BOARD_SYS_TIMER_HZ);

const uint32_t us_loop_count = ((BOARD_CPU_CLOCK / BOARD_SYS_TIMER_HZ) / 1000);

/** emsk board timer counter in timer interrupt */
volatile uint64_t gl_emsk_sys_hz_cnt = 0;
/** emsk board 1ms counter */
volatile uint32_t gl_emsk_ms_cnt = 0;

volatile uint64_t gl_loops_per_jiffy = 1;

volatile uint32_t gl_count = 1;

#define HZ_COUNT_CONV(precision, base)	((precision)/(base))

#define LPS_PREC 8
#if 0
void board_delay_us(uint32_t usecs)
{
	__asm__ __volatile__(
	"	.align 4					\n"
	"	.extern us_loop_count 		\n"
	"	cmp %0, 0					\n"
	"	jeq	1f						\n"
	"	ld %%r1, [us_loop_count] 	\n"
	"	mpy %%r1, %%r1, %0			\n"
	"	mov %%lp_count, %%r1		\n"
	"	lp  1f						\n"
	"	nop							\n"
	"1:								\n"
	:
	: "r"(usecs)
	: "lp_count", "r1");
}
uint64_t calibrate_delay(void)
{
	return cyc_hz_count;
}
#else
void board_delay_us(uint32_t usecs)
{
	__asm__ __volatile__(

	"	.extern gl_loops_per_jiffy 		\n"
	"	.extern gl_count 				\n"
	"	cmp %0, 0						\n"
	"	jeq	1f							\n"
	"	ld %%r1, [gl_loops_per_jiffy] 	\n"
	"	mpy %%r1, %%r1, %0				\n"
	"	ld %%r2, [gl_count] 			\n"
	"	divu %%r1, %%r1, %%r2			\n"
	"	.align 4						\n"
	"	mov %%lp_count, %%r1			\n"
	"	lp  1f							\n"
	"	nop								\n"
	"1:									\n"
	:
	: "r"(usecs)
	: "lp_count", "r1", "r2");
}
uint64_t calibrate_delay(void)
{
	unsigned long ticks, loopbit;
	int lps_precision = LPS_PREC;
	volatile uint64_t loops_per_jiffy;

	loops_per_jiffy = (1<<4);
	gl_loops_per_jiffy = 1;
	gl_count = 1;

	//printf("Calibrating delay loop... [cyc_hz_count = %lu] ", cyc_hz_count);

	while ((loops_per_jiffy <<= 1) != 0) {
		/* wait for "start of" clock tick */
		ticks = OSP_GET_CUR_MS();
		while (ticks == OSP_GET_CUR_MS())
			/* nothing */;
		/* Go .. */
		ticks = OSP_GET_CUR_MS();
		board_delay_us(loops_per_jiffy);
		ticks = OSP_GET_CUR_MS() - ticks;
		if (ticks)
			break;
	}
	loops_per_jiffy >>= 1;
	loopbit = loops_per_jiffy;
	//printf(" [loopbit = %lu] ", loopbit);

	while (lps_precision-- && (loopbit >>= 1)) {
		loops_per_jiffy |= loopbit;
		ticks = OSP_GET_CUR_MS();
		while (ticks == OSP_GET_CUR_MS())
			/* nothing */;
		ticks = OSP_GET_CUR_MS();
		board_delay_us(loops_per_jiffy);
		if (OSP_GET_CUR_MS() != ticks)	/* longer than 1 tick */
			loops_per_jiffy &= ~loopbit;
	}
	//printf("BogoMIPS (lpj=%lu)\r\n",loops_per_jiffy);
	gl_loops_per_jiffy = loops_per_jiffy;
	gl_count = 1000;
	return loops_per_jiffy;
}
#endif



/**
 * \brief	Update timer counter and other MS period operation
 * 	in cycling interrupt and must be called periodically. When the OS timer
 *	interrupt is in conflict with the bare-metal timer interrupt,
 *	put this function into the OS timer interrupt
 * \param[in]	precision	interrupt-period precision in Hz
 */
void board_timer_update(uint32_t precision)
{
	static uint32_t sys_hz_update = 0;
	static uint32_t sys_ms_update = 0;
	uint32_t hz_conv = 0;

	/** count sys hz */
	hz_conv = HZ_COUNT_CONV(precision, BOARD_SYS_TIMER_HZ);
	sys_hz_update ++;
	if (sys_hz_update >= hz_conv) {
		sys_hz_update = 0;
		gl_emsk_sys_hz_cnt ++;
	}

	/** count ms */
	hz_conv = HZ_COUNT_CONV(precision, BOARD_SYS_TIMER_MS_HZ);
	sys_ms_update ++;
	if (sys_ms_update >= hz_conv) {
		sys_ms_update = 0;
		gl_emsk_ms_cnt ++;

#ifdef MID_FATFS
		emsk_sdcard_1ms_update();
#endif
	}
}

/**
 * \brief	emsk bare-metal timer interrupt.
 * 	the Interrupt frequency is based on the defined \ref BOARD_SYS_TIMER_HZ
 */
static void emsk_timer_isr(void *ptr)
{
	timer_int_clear(BOARD_SYS_TIMER_ID);

	board_timer_update(BOARD_SYS_TIMER_HZ);
}

/**
 * \brief	init bare-metal emsk board timer and interrupt
 * \details
 * 		This function is called in \ref board_init, and
 * 	it initializes the 1-MS timer interrupt for bare-metal mode
 */
void emsk_timer_init(void)
{
	if (timer_present(BOARD_SYS_TIMER_ID)) {
		int_disable(BOARD_SYS_TIMER_INTNO); /* disable first then enable */
		int_handler_install(BOARD_SYS_TIMER_INTNO, emsk_timer_isr);
		timer_start(BOARD_SYS_TIMER_ID, TIMER_CTRL_IE|TIMER_CTRL_NH, cyc_hz_count);  /* start 1ms timer interrupt */

		int_enable(BOARD_SYS_TIMER_INTNO);
		if (arc_locked()) {
			arc_unlock();
			calibrate_delay();
			arc_lock();
		} else {
			calibrate_delay();
		}

	}
}

/**
 * \brief	get current cpu hardware ticks
 * \retval	hardware ticks count in 64bit format
 */
uint64_t board_get_hwticks(void)
{
	uint32_t sub_ticks;
	uint64_t total_ticks;
	timer_current(TIMER_0, &sub_ticks);

	total_ticks = (uint64_t)OSP_GET_CUR_MS() * (BOARD_CPU_CLOCK/BOARD_SYS_TIMER_HZ);
	total_ticks += (uint64_t)sub_ticks;

	return total_ticks;
}

/**
 * \brief	get current passed us since timer init
 * \retval	us count in 64bit format
 */
uint64_t board_get_cur_us(void)
{
	uint32_t sub_us;
	uint64_t total_us;
	timer_current(TIMER_0, &sub_us);

	sub_us = ((uint64_t)sub_us * 1000000) / BOARD_CPU_CLOCK;
	total_us = ((uint64_t)OSP_GET_CUR_MS()) * 1000 + (uint64_t)sub_us;

	return total_us;
}

/**
 * \brief	provide MS delay function
 * \details
 * 		this function needs a 1-MS timer interrupt to work.
 * 	For bare-metal, it is implemented in this file.
 * 	For OS, you must call \ref board_timer_update in
 * 	the OS 1-MS timer interrupt when the bare-metal timer interrupt
 * 	is not ready
 * \param[in]	ms		MS to delay
 * \param[in]	os_compat	Enable or disable
 *	When this delay is enabled, use the OS delay function, if one is provided.
 *	See \ref OSP_DELAY_OS_COMPAT_ENABLE and
 *	\ref OSP_DELAY_OS_COMPAT_DISABLE
 */
void board_delay_ms(uint32_t ms, uint8_t os_compat)
{
	uint64_t start_us, us_delayed;

#ifdef ENABLE_OS
	if (os_compat == OSP_DELAY_OS_COMPAT_ENABLE) {
		/** \todo add different os delay functions */
#ifdef OS_FREERTOS
		vTaskDelay(ms);
		return;
#endif
	}
#endif
	us_delayed = ((uint64_t)ms * 1000);
	start_us = board_get_cur_us();
	while ((board_get_cur_us() - start_us) < us_delayed);
}

/** @} end of group BOARD_EMSK_COMMON_TIMER */
