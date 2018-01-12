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
 * \defgroup	EMBARC_APP_FREERTOS_KERNEL	embARC FreeRTOS Kernel Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_OS_FREERTOS
 * \brief	embARC Example for testing FreeRTOS task switch and interrupt/exception handling
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *
 * ### Design Concept
 *     This example is designed to show the functionality of FreeRTOS.
 *
 * ### Usage Manual
 *     Test case for show how FreeRTOS is working by task switching and interrupt/exception processing.
 *     ![ScreenShot of freertos-demo under freertos](pic/images/example/emsk/emsk_freertos_demo.jpg)
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_FREERTOS_KERNEL
 * \brief	main source file of the freertos demo
 */

/**
 * \addtogroup	EMBARC_APP_FREERTOS_KERNEL
 * @{
 */

#include "embARC.h"
#include "embARC_debug.h"
#include <stdlib.h>

static void task1(void *par);
static void task2(void *par);
static void trap_exception(void *p_excinf);
static void soft_interrupt(void *p_excinf);

#ifdef INTNO_I2C0
	#define SWI_INTNO		INTNO_I2C0
#else
	#define SWI_INTNO		20
#endif

#define TSK_PRIOR_HI		(configMAX_PRIORITIES-1)
#define TSK_PRIOR_LO		(configMAX_PRIORITIES-2)

// Task IDs
static TaskHandle_t task1_handle = NULL;
static TaskHandle_t task2_handle = NULL;

// Mutex IDs
static SemaphoreHandle_t mux1_id;

// Semaphores
static SemaphoreHandle_t sem1_id;

// Events
static EventGroupHandle_t evt1_cb;
#define EVENT_WAIT_BITS		0x00000001

// Queues
static QueueHandle_t dtq1_id;

// Local parameters
static volatile unsigned int start = 0;
static unsigned int perf_id = 0xFF;

//#define ENABLE_DETAILED_RESULT_OUTPUT

// Change this macro if you want to run more or less times
#define MAX_SAMPLES		200

#define TASK_DELAY_MS		2

typedef struct _sample {
	unsigned int t_measure;
	unsigned int t_t2_t1;
	unsigned int t_int_t1;
	unsigned int t_t1_t2;
	unsigned int t_t1_int;
	unsigned int t_int_nest;
	unsigned int t_nest_int;
	unsigned int t_mux_rls;
	unsigned int t_sem_snd;
	unsigned int t_evt_snd;
	unsigned int t_dtq_wri;
} sample;


static sample x;
static sample x_aver = {0};

static sample sample_array[MAX_SAMPLES];
static int sample_count = 0;

/** performance timer initialization */
static void perf_init(unsigned int id)
{
	if (timer_start(id, TIMER_CTRL_NH, 0xFFFFFFFF) < 0) {
		EMBARC_PRINTF("perf timer init failed\r\n");

		while (1);
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

/**
 * \brief  call FreeRTOS API, create and start tasks
 */
int main(void)
{
	EMBARC_PRINTF("Benchmark CPU Frequency: %d Hz\r\n", BOARD_CPU_CLOCK);
	EMBARC_PRINTF("Benchmark will run %d times, please wait about %d ms\r\n", MAX_SAMPLES, (TASK_DELAY_MS*MAX_SAMPLES));

	// Register interrupts
	int_handler_install(SWI_INTNO, (EXC_HANDLER)soft_interrupt);
	//int_pri_set(SWI_INTNO, INT_PRI_MIN);
	int_enable(SWI_INTNO);

	exc_handler_install(EXC_NO_TRAP, trap_exception); /*!< install the exception handler */

	vTaskSuspendAll();

	// Create Tasks
	if (xTaskCreate(task1, "task1", 128, (void *)1, TSK_PRIOR_LO, &task1_handle)
	    != pdPASS) {	/*!< FreeRTOS xTaskCreate() API function */
		EMBARC_PRINTF("create task1 error\r\n");
		return -1;
	}

	if (xTaskCreate(task2, "task2", 128, (void *)2, TSK_PRIOR_HI, &task2_handle)
	    != pdPASS) {	/*!< FreeRTOS xTaskCreate() API function */
		EMBARC_PRINTF("create task2 error\r\n");
		return -1;
	}

	// Create Mutex
	mux1_id = xSemaphoreCreateMutex();

	// Create Semaphores
	sem1_id = xSemaphoreCreateBinary();
	xSemaphoreGive(sem1_id);

	// Create Events
	evt1_cb = xEventGroupCreate();

	// Create Queues
	dtq1_id = xQueueCreate(1, sizeof(uint32_t));

	xTaskResumeAll();

	vTaskSuspend(NULL);
	vTaskSuspend(NULL);

	return 0;
}


/**
 * \brief  task1 in FreeRTOS
 * \details Call vTaskDelayUntil() to execute task1 with a fixed period 1 second.
 * \param[in] *par
 */
static void task1(void *par)
{
	uint32_t queue_data = 1;
	TickType_t xLastExecutionTime;
	xLastExecutionTime = xTaskGetTickCount(); /*!< initialize current tick */

	while (1) {
		//// 1-E: Task2 --> Task1
		x.t_t2_t1 = perf_end();
		//// 2-S: Task1 --> Int
		perf_start();
		_arc_aux_write(AUX_IRQ_HINT, SWI_INTNO); /*!< activate soft_interrupt */
		//// 5-E: Int --> Task1
		x.t_int_t1 = perf_end();

		// task delay, to control benchmark run speed
		vTaskDelayUntil( &xLastExecutionTime, TASK_DELAY_MS);
		// Task 1 acquire mutex first
		xSemaphoreTake(mux1_id, portMAX_DELAY);
		// Task 1 acquire semaphore first
		xSemaphoreTake(sem1_id, portMAX_DELAY);

		//// 6-S: Task1 --> Task2
		perf_start();
		vTaskResume(task2_handle);

		//// 8-S: Mutex Release -> Mutex Acquire
		perf_start();
		// Task 1 release mutex, task 2 acquire it
		xSemaphoreGive(mux1_id);

		//// 9-S: Sem Post -> Sem Acquire
		perf_start();
		// Task 1 post sem, task 2 acquire it
		xSemaphoreGive(sem1_id);

		//// 10-S: Event Write -> Event Read
		perf_start();
		// Task 1 write event, task 2 read it
		xEventGroupSetBits(evt1_cb, EVENT_WAIT_BITS);

		//// 11-S: Queue Write -> Queue Read
		perf_start();
		// Task 1 write queue, task 2 read it
		xQueueSend(dtq1_id, (void *)(&queue_data),  portMAX_DELAY);
		queue_data ++;
	}

}

/**
 * \brief  task2 in FreeRTOS
 * \details Print information in task2 and suspend task2.
 * \param[in] *par
 */
static void task2(void *par)
{
	static uint32_t perf_times = 0;
	uint32_t queue_data = 0;
	perf_init(TIMER_1);

	while (1) {
		//// 1-S: Task2 --> Task1
		perf_start();
		vTaskSuspend(task2_handle); /*!< suspend task2 */
		//// 6-E: Task1 --> Task2
		x.t_t1_t2 = perf_end();
		//// 7-S: Benchmark extra cost
		perf_start();
		//// 7-E: Benchmark extra cost
		x.t_measure = perf_end();

		// Task 2 wait for task 1 release mutex
		xSemaphoreTake(mux1_id, portMAX_DELAY);
		//// 8-E: Mutex Release -> Mutex Acquire
		x.t_mux_rls = perf_end();
		xSemaphoreGive(mux1_id);

		// Task 2 wait for task 1 release sem
		xSemaphoreTake(sem1_id, portMAX_DELAY);
		//// 9-E: Sem Post -> Sem Acquire
		x.t_sem_snd = perf_end();
		xSemaphoreGive(sem1_id);

		// Task 2 wait for task 1 set event
		xEventGroupWaitBits(evt1_cb, EVENT_WAIT_BITS, pdTRUE, pdFALSE, portMAX_DELAY);

		//// 10-E: Event Write -> Event Read
		x.t_evt_snd = perf_end();

		// Task 2 read queue
		xQueueReceive(dtq1_id, (void *)(&queue_data), portMAX_DELAY);
		//// 11-E: Queue Write -> Queue Read
		x.t_dtq_wri = perf_end();

		if (perf_times) {
			if (sample_count < MAX_SAMPLES) {
				sample_array[sample_count] = x;
			} else {
				/* Suspend task 1 */
				vTaskSuspend(task1_handle);

				for (int i = 0; i < MAX_SAMPLES; i ++) {
					x_aver.t_measure += sample_array[i].t_measure;
					x_aver.t_t2_t1 += sample_array[i].t_t2_t1;
					x_aver.t_int_t1 += sample_array[i].t_int_t1;
					x_aver.t_t1_t2 += sample_array[i].t_t1_t2;
					x_aver.t_t1_int += sample_array[i].t_t1_int;
					x_aver.t_int_nest += sample_array[i].t_int_nest;
					x_aver.t_nest_int += sample_array[i].t_nest_int;
					x_aver.t_mux_rls += sample_array[i].t_mux_rls;
					x_aver.t_sem_snd += sample_array[i].t_sem_snd;
					x_aver.t_evt_snd += sample_array[i].t_evt_snd;
					x_aver.t_dtq_wri += sample_array[i].t_dtq_wri;
				}

				x_aver.t_measure /= MAX_SAMPLES;
				x_aver.t_t2_t1 /= MAX_SAMPLES;
				x_aver.t_int_t1 /= MAX_SAMPLES;
				x_aver.t_t1_t2 /= MAX_SAMPLES;
				x_aver.t_t1_int /= MAX_SAMPLES;
				x_aver.t_int_nest /= MAX_SAMPLES;
				x_aver.t_nest_int /= MAX_SAMPLES;
				x_aver.t_mux_rls /= MAX_SAMPLES;
				x_aver.t_sem_snd /= MAX_SAMPLES;
				x_aver.t_evt_snd /= MAX_SAMPLES;
				x_aver.t_dtq_wri /= MAX_SAMPLES;

				EMBARC_PRINTF("\r\n");
				EMBARC_PRINTF("Average benchmark result list as follows:\r\n");
				EMBARC_PRINTF("extra measurement cost : %d cycles\r\n", x_aver.t_measure);
				EMBARC_PRINTF("task2     ->  task1    : %d cycles\r\n", x_aver.t_t2_t1);
				EMBARC_PRINTF("task1     ->  int      : %d cycles\r\n", x_aver.t_t1_int);
				EMBARC_PRINTF("int       ->  nest int : %d cycles\r\n", x_aver.t_int_nest);
				EMBARC_PRINTF("nest int  ->  int      : %d cycles\r\n", x_aver.t_nest_int);
				EMBARC_PRINTF("int       ->  task1    : %d cycles\r\n", x_aver.t_int_t1);
				EMBARC_PRINTF("task1     ->  task2    : %d cycles\r\n", x_aver.t_t1_t2);
				EMBARC_PRINTF("mux: tsk1 ->  tsk2     : %d cycles\r\n", x_aver.t_mux_rls);
				EMBARC_PRINTF("sem: tsk1 ->  tsk2     : %d cycles\r\n", x_aver.t_sem_snd);
				EMBARC_PRINTF("evt: tsk1 ->  tsk2     : %d cycles\r\n", x_aver.t_evt_snd);
				EMBARC_PRINTF("dtq: tsk1 ->  tsk2     : %d cycles\r\n", x_aver.t_dtq_wri);
				EMBARC_PRINTF("\r\n");

#ifdef ENABLE_DETAILED_RESULT_OUTPUT
				EMBARC_PRINTF("Detailed benchmark result table list as follows:\r\n");
				EMBARC_PRINTF("t_measure t_t2_t1 t_t1_int t_int_nest t_nest_int t_int_t1 t_t1_t2 t_mux_rls t_sem_snd t_evt_snd t_dtq_wri\r\n");

				for (int i = 0; i < MAX_SAMPLES; i ++) {
					EMBARC_PRINTF("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\r\n", \
					              sample_array[i].t_measure, sample_array[i].t_t2_t1, \
					              sample_array[i].t_t1_int, sample_array[i].t_int_nest, \
					              sample_array[i].t_nest_int, sample_array[i].t_int_t1, \
					              sample_array[i].t_t1_t2, sample_array[i].t_mux_rls, \
					              sample_array[i].t_sem_snd, sample_array[i].t_evt_snd, \
					              sample_array[i].t_dtq_wri);
				}

#endif
				exit(0); /* Exit benchmark application */
			}

			sample_count ++;
		}

		perf_times ++;
	}
}

/**
 * \brief  trap exception
 * \details Call xTaskResumeFromISR() to resume task2 that can be called from within ISR.
 * If resuming the task2 should result in a context switch, call vPortYieldFromIsr() to generate task switch request.
 * \param[in] *p_excinf
 */
static void trap_exception(void *p_excinf)
{
	//// 3-E: Int --> Nest Int
	x.t_int_nest = perf_end();
	//// 4-S: Nest Int --> Int
	perf_start();
}

/**
 * \brief  soft interrupt
 * \details Call trap_s instruction to raise the exception.
 * \param[in] *p_excinf
 */
static void soft_interrupt(void *p_exinf)
{
	//// 2-E: Task1 --> Int
	x.t_t1_int = perf_end();
	//// 3-S: Int --> Nest Int
	perf_start();
	Asm("trap_s 1");
	//// 4-E: Nest Int --> Int
	x.t_nest_int = perf_end();
	//// 5-S: Int --> Task1
	perf_start();
}

/** @} */
