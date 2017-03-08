/*
 * Copyright (c) 2014, Swedish Institute of Computer Science.
 *
 * Copyright (c) 2017, Synopsys, Inc.
 * Modified for port to ARC processor by Wayne Ren wei.ren@synopsys.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \file clock.c
 * \brief clock support for contiki
 */

#include <sys/clock.h>
#include <sys/cc.h>
#include <sys/etimer.h>
#include "arc_timer.h"
#include "arc_exception.h"


static volatile clock_time_t current_clock = 0;		/* current system tick in contiki  */
static volatile unsigned long current_seconds = 0;	/* current system second in contiki */
static unsigned int second_countdown = CLOCK_SECOND;	/* system ticks in one second */


/**
 * \brief  timer isr for clock library
 *
 * \param[in] ptr pointer transfered from interrupt entry
 */
static void timer0_isr(void *ptr)
{
	timer_int_clear(TIMER_0);
	current_clock++;
	if (etimer_pending() && etimer_next_expiration_time() <= current_clock) {
		etimer_request_poll();
	}
	if (--second_countdown == 0) {
		current_seconds++;
		second_countdown = CLOCK_SECOND;
	}
}

/**
 * \brief  Initialize the clock library
 */
void clock_init(void)
{
	int_handler_install(INTNO_TIMER0, timer0_isr);
	int_enable(INTNO_TIMER0);
	timer_start(TIMER_0, TIMER_CTRL_IE | TIMER_CTRL_NH, BOARD_CPU_CLOCK / CLOCK_SECOND);
}

/**
 * \brief  Get the current clock time
 *
 * \returns  The current clock time, measured in system ticks
 */
CCIF clock_time_t clock_time(void)
{
	return current_clock;
}

/**
 * \brief  Get the current value of the platform seconds.
 *
 * \returns current seconds
 */
CCIF unsigned long clock_seconds(void)
{
	return current_seconds;
}

/**
 * \brief  Wait for a given number of ticks *
 * \param[in] i	timer tics
 */
void clock_wait(clock_time_t i)
{
	clock_time_t start;

	start = clock_time();
	while(clock_time() - start < (clock_time_t)i);
}

/**
 * \brief  Set the value of the platform seconds
 *
 * \param[in] sec	the value to set
 */
void clock_set_seconds(unsigned long sec)
{
	int_disable(INTNO_TIMER0);
	current_seconds = sec;
	int_enable(INTNO_TIMER0);
}

/**
 * \brief  Delay a given number of microseconds.
 *
 * \param[in] dt microseconds to delay
 */
void clock_delay_usec(uint16_t dt)
{
	uint32_t start, end;
	uint32_t clk;
	uint32_t pass = 0;
	clk = dt * (BOARD_CPU_CLOCK / 1000000);
	do {
		timer_current(TIMER_0, &start);
		timer_current(TIMER_0, &end);
		if (end >= start) {
			pass += end - start;
		} else {
			pass += end;
		}
	} while (pass < clk);
}

/**
 * \brief  Delay some time, an old version of clock_delay_usec
 *
 * \param[in] delay time to delay
 */
void clock_delay(unsigned int delay)
{
	clock_delay_usec(delay);
}
