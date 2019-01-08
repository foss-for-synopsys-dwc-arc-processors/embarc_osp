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
/* embARC HAL */
#include "embARC.h"
#include "embARC_debug.h"


volatile static int t0 = 0;
volatile static int t1 = 0;

/** arc timer 0 interrupt routine */
static void timer0_isr(void *ptr)
{
	timer_int_clear(TIMER_0);
	t0++;
	EMBARC_PRINTF("timer0 interrupt with the highest priority(fiq if fiq enabled):%d\r\n", t0);
}

/** arc timer 1 interrupt routine */
static void timer1_isr(void *ptr)
{
	timer_int_clear(TIMER_1);
	t1++;
	EMBARC_PRINTF("timer1 interrupt:%d\r\n", t1);
}

int main(void)
{
	uint32_t val;
	unsigned int int_bcr;

	EMBARC_PRINTF("ARC timer and interrupt\r\n");
	int_bcr = _arc_lr_reg(AUX_BCR_IRQ);
	EMBARC_PRINTF("interrupt number:%d, ", (unsigned char)(int_bcr >> 8));
	EMBARC_PRINTF("extern interrupts:%d, ", (unsigned char)(int_bcr >> 16));
	EMBARC_PRINTF("interrupt priorities:%d, ", (unsigned char)((int_bcr >> 24) & 0xf) + 1);

	if (int_bcr & 0x10000000) {
		EMBARC_PRINTF("fiq enabled\r\n");
	} else {
		EMBARC_PRINTF("fiq disabled\r\n");
	}

	if (timer_present(TIMER_0)) {
		EMBARC_PRINTF("timer 0 is present\r\n");
		timer_current(TIMER_0, &val);
		EMBARC_PRINTF("cnt:%d\r\n", val);
		timer_stop(TIMER_0); /* Stop it first since it might be enabled before */
		int_handler_install(INTNO_TIMER0, timer0_isr);
		/* to enable fiq, interrupt priority must be the highest */
		int_pri_set(INTNO_TIMER0, INT_PRI_MIN);
		int_enable(INTNO_TIMER0);
	} else {
		EMBARC_PRINTF("timer 0 is not present\r\n");
	}

	if (timer_present(TIMER_1)) {
		EMBARC_PRINTF("timer 1 is present\n");
		timer_current(TIMER_1, &val);
		EMBARC_PRINTF("cnt:%d\r\n", val);
		timer_stop(TIMER_1); /* Stop it first since it might be enabled before */
		int_handler_install(INTNO_TIMER1, timer1_isr);
		int_enable(INTNO_TIMER1);
	} else {
		EMBARC_PRINTF("timer 1 is not present\r\n");
	}

	if (timer_present(TIMER_0)) {
		timer_start(TIMER_0, TIMER_CTRL_IE, BOARD_CPU_CLOCK);
		while (t0 < 2);
		timer_stop(TIMER_0);
		timer_current(TIMER_0, &val);
		EMBARC_PRINTF("stop timer0, cnt:%d\r\n", val);
	}

	if (timer_present(TIMER_1)) {
		timer_start(TIMER_1, TIMER_CTRL_IE, 2*BOARD_CPU_CLOCK);
		while (t1 < 2);
		timer_stop(TIMER_1);
		timer_current(TIMER_1, &val);
		EMBARC_PRINTF("stop timer1, cnt:%d\r\n", val);
	}

	if (timer_present(TIMER_0)) {
		EMBARC_PRINTF("enable timer 0 watchdog\r\n");
		timer_start(TIMER_0, TIMER_CTRL_W, 5*BOARD_CPU_CLOCK);
	}

	EMBARC_PRINTF("---end----\r\n");
	while(1);
	return E_SYS;
}
