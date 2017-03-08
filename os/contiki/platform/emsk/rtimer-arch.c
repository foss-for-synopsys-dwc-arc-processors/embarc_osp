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
 * \file rtimer-arch.c
 * \brief real timer support of contiki
 */
#include "arc_timer.h"
#include "rtimer-arch.h"
#include "arc_exception.h"


/**
 * \brief  isr for rtimer (using timer1)
 * to use rtimer, timer1 must be present
 *
 * \param[in] ptr pointer transfered from interrupt entry
 */
static void timer1_isr(void *ptr)
{
	timer_int_clear(TIMER_1);
	int_disable(INTNO_TIMER1);
	rtimer_run_next();
}

/**
 * \brief architecture dependent init for rtimer
 * here, timer1 is initialized
 */
void rtimer_arch_init(void)
{
	int_handler_install(INTNO_TIMER1, timer1_isr);
	int_enable(INTNO_TIMER1);
}

/**
 * \brief  schedule a run of rtimer_run_next at t
 *
 * \param[in] t
 */
void rtimer_arch_schedule(rtimer_clock_t t)
{
	timer_start(TIMER_1, TIMER_CTRL_IE, t);
}

/**
 * \brief  get current timer tick
 *
 * \returns click value
 */
rtimer_clock_t rtimer_arch_now()
{
	uint32_t val;
	timer_current(TIMER_1, &val);
	return (rtimer_clock_t)val;
}