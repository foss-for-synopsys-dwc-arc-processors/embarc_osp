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
/*
 * Implementation of functions defined in portable.h
 */

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"

#include "arc_exception.h"
#include "arc_timer.h"
#include "board.h"

#include "arc_freertos_exceptions.h"

volatile unsigned int ulCriticalNesting = 999UL;
volatile unsigned int context_switch_reqflg;	/* task context switch request flag in exceptions and interrupts handling */

/* --------------------------------------------------------------------------*/
/**
 * @brief kernel tick interrupt handler of freertos
 */
/* ----------------------------------------------------------------------------*/
static void vKernelTick( void *ptr )
{
	/* clear timer interrupt */
	timer_int_clear(BOARD_OS_TIMER_ID);
	board_timer_update(configTICK_RATE_HZ);

	if (xTaskIncrementTick()) {
		portYIELD_FROM_ISR();	/* need to make task switch */
	}
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  setup freertos kernel tick
 */
/* ----------------------------------------------------------------------------*/
static void prvSetupTimerInterrupt(void)
{
	unsigned int cyc = configCPU_CLOCK_HZ / configTICK_RATE_HZ;

	int_disable(BOARD_OS_TIMER_INTNO); /* disable os timer interrupt */
	timer_stop(BOARD_OS_TIMER_ID);
	timer_start(BOARD_OS_TIMER_ID, TIMER_CTRL_IE | TIMER_CTRL_NH, cyc);

	int_handler_install(BOARD_OS_TIMER_INTNO, (INT_HANDLER)vKernelTick);
	int_enable(BOARD_OS_TIMER_INTNO);
}

/*
 * Setup the stack of a new task so it is ready to be placed under the
 * scheduler control.  The registers have to be placed on the stack in
 * the order that the port expects to find them.
 *
 * For ARC, task context switch is implemented with the help of SWI exception
 * It's not efficient but simple.
 *
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* To ensure asserts in tasks.c don't fail, although in this case the assert
	is not really required. */
	pxTopOfStack--;

	/* Setup the initial stack of the task.  The stack is set exactly as
	expected by the portRESTORE_CONTEXT() macro. */

	/* When the task starts is will expect to find the function parameter in
	R0. */
	*pxTopOfStack = ( StackType_t ) pvParameters;	/* R0 */

	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) pxCode;		/* function body */

	/* PC */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) start_r;	/* dispatch return address */

	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) portNO_CRITICAL_NESTING;
	return pxTopOfStack;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief  start the freertos scheduler, go to the first task
 *
 * @returns
 */
/* ----------------------------------------------------------------------------*/
BaseType_t xPortStartScheduler( void )
{
	/* Start the timer that generates the tick ISR. */
	prvSetupTimerInterrupt();
	start_dispatch();

	/* Should not get here! */
	return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief
 */
/* ----------------------------------------------------------------------------*/
void vPortEndScheduler( void )
{

}

/* --------------------------------------------------------------------------*/
/**
 * @brief  generate a task switch request in ISR
 */
/* ----------------------------------------------------------------------------*/
void vPortYieldFromIsr(void)
{
	unsigned int status32;

	status32 = cpu_lock_save();
	context_switch_reqflg = true;
	cpu_unlock_restore(status32);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief
 */
/* ----------------------------------------------------------------------------*/
void vPortYield(void)
{
	unsigned int status32;

	status32 = cpu_lock_save();
	dispatch();
	cpu_unlock_restore(status32);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief
 */
/* ----------------------------------------------------------------------------*/
void vPortEndTask(void)
{

#if ( INCLUDE_vTaskDelete == 1 )
	vTaskDelete(NULL); /* Delete task itself */
#endif

	while(1) { /* Yield to other task */
		vPortYield();
	}
}
