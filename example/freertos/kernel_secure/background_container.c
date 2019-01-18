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

#include "embARC.h"
#include "embARC_debug.h"

#include "background_container.h"
#include "container1.h"
#include "container2.h"

static void task1(void * par);
static void task2(void * par);
static void interrupt_high_pri(void *p_excinf);
static void interrupt_low_pri(void *p_excinf);

/**
 * \var		task1_handle
 * \brief	handle of task1
 * \details	If task handle is not used, set Null.
 */
static TaskHandle_t task1_handle = NULL;
/**
 * \var		task2_handle
 * \brief	handle of task2
 * \details	If task handle is not used, set Null.
 */
static TaskHandle_t task2_handle = NULL;

static volatile unsigned int start = 0;
static unsigned int perf_id = 0xFF;

static unsigned int t_int_t2;
static unsigned int t_t2_t1;
static unsigned int t_t1_int;
static unsigned int t_int_nest;
static unsigned int t_nest_int;

/** performance timer initialization */
static void perf_init(unsigned int id)
{
	if (timer_start(id, TIMER_CTRL_NH, 0xFFFFFFFF) < 0) {
		EMBARC_PRINTF("perf timer init failed\r\n");
		while(1);
	}
	perf_id = id;
}

/** performance timer start */
static void perf_start(void)
{
	if (timer_current(perf_id, (void *)(&start)) < 0) {
		start = 0;
	}
}

/** performance timer end, and return the time passed */
static unsigned int perf_end(void)
{
	unsigned int end = 0;

	if (timer_current(perf_id, (void *)(&end)) < 0) {
		return 0;
	}

	if (start < end) {
		return (end - start);
	} else {
		return (0xFFFFFFFF - start + end);
	}
}


static uint8_t public_data[SECRET_LEN];


void default_interrupt_handler(void *p_exinf)
{
	EMBARC_PRINTF("default interrupt handler\r\n");
}


/**
 * \brief  call FreeRTOS API, create and start tasks
 */
int main(void)
{
	uint32_t ret;

	vTaskSuspendAll();
	ret = container_call(container1, init_secret);
	xTaskResumeAll();

	if (ret) {
		EMBARC_PRINTF("secret key initialized error\r\n");
		return -1;
	}


	int_handler_install(INTNO_LOW_PRI, (INT_HANDLER)interrupt_low_pri);
	int_pri_set(INTNO_LOW_PRI, INT_PRI_MAX);
	int_enable(INTNO_LOW_PRI);

	int_handler_install(INTNO_HIGH_PRI, (INT_HANDLER)interrupt_high_pri);
	int_pri_set(INTNO_HIGH_PRI, INT_PRI_MIN);
	int_enable(INTNO_HIGH_PRI);


	vTaskSuspendAll();
	if (xTaskCreate(task1, "task1", 256, (void *)1, configMAX_PRIORITIES-2, &task1_handle)
		!= pdPASS) {	/*!< FreeRTOS xTaskCreate() API function */
		EMBARC_PRINTF("create task1 error\r\n");
		return -1;
	}
	if (xTaskCreate(task2, "task2", 256, (void *)2, configMAX_PRIORITIES-1, &task2_handle)
		!= pdPASS) {	/*!< FreeRTOS xTaskCreate() API function */
		EMBARC_PRINTF("create task2 error\r\n");
		return -1;
	}
	xTaskResumeAll();

	return 0;
}


/**
 * \brief  task1 in FreeRTOS
 * \details Call vTaskDelayUntil() to execute task1 with a fixed period 1 second.
 * \param[in] *par
 */
static void task1(void * par)
{
	while(1) {
		perf_start();
		_arc_aux_write(AUX_IRQ_HINT, INTNO_LOW_PRI);	/*!< activate low priority interrupt */
		t_t2_t1 = perf_end();

		EMBARC_PRINTF("The performance data is:\r\n");
		EMBARC_PRINTF("\ttask2->task1:%d cycles\r\n",t_t2_t1);
		EMBARC_PRINTF("\ttask1->int:%d cycles\r\n", t_t1_int);
		EMBARC_PRINTF("\tint->nest int:%d cycles\r\n", t_int_nest);
		EMBARC_PRINTF("\tnest int->int:%d cycles\r\n", t_nest_int);
		EMBARC_PRINTF("\tint->task2:%d cycles\r\n", t_int_t2);
		EMBARC_PRINTF("\r\n");

		EMBARC_PRINTF("Task1 is running and makes a container call...\r\n");
		vTaskSuspendAll();
		container_call(container2, trusted_ops);
		xTaskResumeAll();

		vTaskDelay(500);
	}

}

/**
 * \brief  task2 in FreeRTOS
 * \details Print information in task2 and suspend task2.
 * \param[in] *par
 */
static void task2(void * par)
{
	int i = 0;
	uint32_t ret = 0;

	perf_init(TIMER_1);
	while(1) {
		perf_start();
		vTaskSuspend(NULL);	/*!< suspend task2 */
		t_int_t2 = perf_end();

		EMBARC_PRINTF("\r\nRounds: %d\r\n", i++);
		EMBARC_PRINTF("\r\nTask2 is running and makes a container call...\r\n");
		vTaskSuspendAll();
		ret = container_call(container1, operate_secret, "embarc", GET_SECRET, public_data);
		xTaskResumeAll();
		if (ret) {
			EMBARC_PRINTF("get secret failed\r\n");
		} else {
			EMBARC_PRINTF("the secret is:%s\r\n", public_data);
			memset(public_data, 0, SECRET_LEN);
		}
	}
}

/**
 * \brief  high priority interrupt
 * \details Call xTaskResumeFromISR() to resume task2 that can be called from within ISR.
 * If resuming the task2 should result in a context switch, call vPortYieldFromIsr() to generate task switch request.
 * \param[in] *p_excinf
 */
static void interrupt_high_pri(void *p_excinf)
{
	// show exception frame
	t_int_nest = perf_end();
	if (xTaskResumeFromISR(task2_handle) == pdTRUE) {
		portYIELD_FROM_ISR();	/* need to make task switch */
	}
	perf_start();
}

/**
 * \brief  low priority interrupt
 * \details write AUX_IRQ_HINT to raise higher priority interrupt.
 * \param[in] *p_excinf
 */
static void interrupt_low_pri(void *p_exinf)
{
	t_t1_int = perf_end();
	perf_start();
	_arc_aux_write(AUX_IRQ_HINT, INTNO_HIGH_PRI);	/*!< activate high priority interrupt */
	t_nest_int = perf_end();
	perf_start();
}
