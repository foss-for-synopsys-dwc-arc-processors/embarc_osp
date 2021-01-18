/* ------------------------------------------
 * Copyright (c) 2021, Synopsys, Inc. All rights reserved.

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
#include "embARC_debug.h"

#define LED_TOGGLE_MASK		BOARD_LED_MASK



#define LED0_TASK_PRIO	7
#define LED0_STK_SIZE	256
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
void led0_task(void *pdata);

#define LED1_TASK_PRIO	6
#define LED1_STK_SIZE	256
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
void led1_task(void *pdata);

/**
 * \brief	Test hardware board without any peripheral
 */
int main(void)
{
	OS_CPU_SR cpu_sr = 0;
	OS_ENTER_CRITICAL();
	OSSchedLock();
	OSTaskCreate(led0_task, (void *)0, (OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE - 1], LED0_TASK_PRIO);
	OSTaskCreate(led1_task, (void *)0, (OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE - 1], LED1_TASK_PRIO);
	OSSchedUnlock();
	OS_EXIT_CRITICAL();

	return E_SYS;
}

void led0_task(void *pdata)
{
	while (1)
	{
		EMBARC_PRINTF("led0_task\r\n");
		board_delay_ms(1000, 1);
	};
}

void led1_task(void *pdata)
{
	while (1)
	{
		EMBARC_PRINTF("led1_task\r\n");
		board_delay_ms(2000, 1);
	};
}
