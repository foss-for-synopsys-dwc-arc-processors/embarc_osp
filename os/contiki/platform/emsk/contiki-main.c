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
 * \file contiki-main.c
 * \brief  entry for contiki
 */

#include "embARC.h"

#include "contiki.h"

#include "lib/random.h"
#include "dev/leds.h"

#define DBG_LESS
#include "embARC_debug.h"

int main(void)
{

	int idle_count = 0;
/* to be compatible with old embarc */
#ifndef EMBARC_USE_BOARD_MAIN
	board_init(); /* board level init */
#ifdef MID_COMMON
	xprintf_setup();
#endif
	cpu_unlock();
#endif

	DBG("Contiki starts to run\r\n");

	leds_init();
	random_init(EMSK_ID);
	clock_init();
	process_init();

	energest_init();

	process_start(&etimer_process, NULL);
	ctimer_init();

	autostart_start(autostart_processes);
	DBG("Processes running\r\n");

	while(1) {
		do {
		} while(process_run() > 0);

		idle_count++;
		/* Idle! */
		/* Stop processor clock */
#ifndef LIB_SECURESHIELD
		Asm("sleep"); /* sleep is privileged instruction */
#endif
	}

	return E_SYS; /* never come here */
}
