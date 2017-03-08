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
 * \file platform-conf.h
 * \brief  contiki platform configuration
 */

#ifndef _PLATFORM_CONF_H_
#define _PLATFORM_CONF_H_

#include "board.h"
#include "embARC_toolchain.h"


/* Platform-dependent definitions */
#define CC_CONF_REGISTER_ARGS		0
#define CC_CONF_FUNCTION_POINTER_ARGS	1
#define CC_CONF_FASTCALL
#define CC_CONF_VA_ARGS			1
#define CC_CONF_INLINE			inline

#define CCIF
#define CLIF

#define CLOCK_CONF_SECOND	100
#define RTIMER_ARCH_SECOND	BOARD_CPU_CLOCK

typedef unsigned int clock_time_t;
typedef unsigned int rtimer_clock_t;
typedef unsigned int uip_stats_t;

#define RTIMER_CLOCK_LT(a,b)	((a) < (b))


#ifndef EMSK_ID
#define EMSK_ID 1
#endif

#if EMSK_ID == 1
#define MRF24J40_PAN_COORDINATOR
#endif

#define PLATFORM_HAS_BUTTON		1	/* EMSK always has buttons */
#define PLATFORM_HAS_LEDS		1	/* EMSK always has leds */
#define PLATFORM_HAS_RADIO		0	/* if PMOD RF is attached, set 1 */
#define PLATFORM_HAS_TEMP		1	/* if PMOD temp is attached, set 1 */

#endif	/* _PLATFORM_CONF_H_ */
