/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#if defined(__MW__)

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"

#include "queue.h"
#include "semphr.h"
#include "task.h"

#include "arc_exception.h"
#include "embARC_toolchain.h"
#include "embARC_debug.h"

#ifdef ENABLE_FREERTOS_TLS_DEBUG
#define TLS_DEBUG(fmt, ...)	EMBARC_PRINTF(fmt, ##__VA_ARGS__)
#else
#define TLS_DEBUG(fmt, ...)
#endif

/*
 * Runtime routines to execute constructors and
 * destructors for task local storage.
 */
extern void __mw_run_tls_dtor();
extern void __mw_run_tls_ctor();

/*
 * Linker generated symbols to mark .tls section addresses
 * first byte .. last byte
 */
extern char _ftls[], _etls[];
#pragma weak _ftls
#pragma weak _etls

void executable_requires_tls_section(void)
{
#if _ARC
	for (;;) {
		_flag(1);
		_nop();
		_nop();
		_nop();
		_nop();
		_nop();
	}
#endif
}
#pragma off_inline(executable_requires_tls_section);
#pragma alias(executable_requires_tls_section, "executable_requires_.tls_section");

static void* init_task_tls(void)
{
	uint32_t len = (uint32_t)(_etls - _ftls);
	void *tls = NULL;

#if FREERTOS_HEAP_SEL == 3
	#warning "FreeRTOS TLS support is not compatible with heap 3 solution(FREERTOS_HEAP_SEL=3)!"
	#warning "You can change FREERTOS_HEAP_SEL in freertos.mk to select other heap solution."
#else
	tls = pvPortMalloc(len);
#endif
	if (tls) {
		TLS_DEBUG("Malloc task tls:%dbytes\r\n", len);
		memcpy(tls, _ftls, len);
		__mw_run_tls_ctor();	// Run constructors
	}
	return tls;
}

static void free_task_tls(void *pxTCB)
{
	TaskHandle_t task2free = (TaskHandle_t)pxTCB;

	if (task2free != NULL) {
		void *tls = pvTaskGetThreadLocalStoragePointer(task2free, 0);
		if (tls) {
			TLS_DEBUG("Free task tls\r\n");
			__mw_run_tls_dtor();
			vPortFree(tls);
			vTaskSetThreadLocalStoragePointer(task2free, 0, NULL);
		}
	}
}

void task_end_hook(void *pxTCB)
{
	free_task_tls(pxTCB);
}

static void* get_isr_tls(void)
{
	// In an ISR
	static int first = 1;
	if (_Rarely(first)) {
		first = 0;
		__mw_run_tls_ctor();	// Run constructors
	}
	return (void *)_ftls;
}
#pragma off_inline(get_isr_tls)

static void* get_task_tls(void)
{
	TaskHandle_t cur_task;

	cur_task = xTaskGetCurrentTaskHandle();
	if (cur_task == NULL) return get_isr_tls();
	void *tls = pvTaskGetThreadLocalStoragePointer(cur_task, 0);
	if (tls == NULL) {
		tls = init_task_tls();
		if (tls) {
			vTaskSetThreadLocalStoragePointer(cur_task, 0, tls);
		} else {
			tls = get_isr_tls();
		}
	}
	return tls;
}
#pragma off_inline(get_task_tls)

#if _ARC /* for ARC XCALLs need to preserve flags */
	extern void * _Preserve_flags _mwget_tls(void);
#endif

/*
 * Back end gens calls to find local data for this task
 */
void* _mwget_tls(void)
{
	if (_ftls == (char *)0) {
		executable_requires_tls_section();
	}
	if (exc_sense()) { /* In ISR */
		return get_isr_tls();
	} else { /* In Task */
		return get_task_tls();
	}
}


// simple interface of thread safe
typedef xSemaphoreHandle _lock_t;
#if configUSE_RECURSIVE_MUTEXES != 1
#error "configUSE_RECURSIVE_MUTEXES in FreeRTOSConfig.h need to 1"
#endif

void _mwmutex_create(_lock_t *mutex_ptr)
{
	*mutex_ptr = xSemaphoreCreateRecursiveMutex();
}

void _mwmutex_delete(_lock_t *mutex_ptr)
{
	if ((*mutex_ptr) != NULL) {
		vSemaphoreDelete(*mutex_ptr);
	}
}

void _mwmutex_lock(_lock_t mutex)
{
	if ((mutex) != NULL) {
		while (xSemaphoreTakeRecursive(mutex, portMAX_DELAY) != pdTRUE);
	}
}

void _mwmutex_unlock(_lock_t mutex)
{
	if ((mutex) != NULL) {
		xSemaphoreGiveRecursive(mutex);
	}
}

#else

#endif /* __MW__ */
