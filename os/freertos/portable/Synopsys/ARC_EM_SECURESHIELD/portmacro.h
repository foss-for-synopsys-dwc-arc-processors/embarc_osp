/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
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

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR	char
#define portFLOAT	float
#define portDOUBLE	double
#define portLONG	long
#define portSHORT	short
#define portSTACK_TYPE	unsigned int
#define portBASE_TYPE	portLONG

#ifndef Inline
#define Inline	static __inline__
#endif
#ifndef Asm
#define Asm	__asm__ volatile
#endif

/*
 *  normal constants
 */
#ifndef NULL
#define NULL		0			/* invalid pointer */
#endif /* NULL */

#ifndef true
#define true		1			/* true */
#endif /* true */

#ifndef false
#define false		0			/* false */
#endif /* false */

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef unsigned int TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif

#define portNO_CRITICAL_NESTING	0x0
#define portSTACK_GROWTH	( -1 )
#define portTICK_PERIOD_MS	( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT	8
#define portNOP()		Asm( "nop_s" );
#define IPM_ENABLE_ALL		1

#define portYIELD_FROM_ISR()		vPortYieldFromIsr()
#define portYIELD()			vPortYield()


#define portINITIAL_STATUS32 (0x80000000 | STATUS32_RESET_VALUE | (((INT_PRI_MAX - INT_PRI_MIN) << 1) & 0x1e))

/* Critical section management. */
#define portDISABLE_INTERRUPTS()		\
{						\
	Asm("clri");				\
	Asm("":::"memory");			\
}						\

#define portENABLE_INTERRUPTS()			\
{						\
	Asm("":::"memory");			\
	Asm("seti");				\
}						\

extern volatile unsigned int ulCriticalNesting;

#define portENTER_CRITICAL()			\
{						\
	portDISABLE_INTERRUPTS()		\
	ulCriticalNesting++;			\
}


#define portEXIT_CRITICAL()						\
{									\
	if (ulCriticalNesting > portNO_CRITICAL_NESTING)		\
	{								\
		ulCriticalNesting--;					\
		if (ulCriticalNesting == portNO_CRITICAL_NESTING)	\
		{							\
			portENABLE_INTERRUPTS()				\
		}							\
	}								\
}


#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() do {} while (0)	/* we use the timer */
#define portALT_GET_RUN_TIME_COUNTER_VALUE(dest) (dest = xTickCount)

#if defined(__MW__)
extern void task_end_hook(void *pxTCB);
#define portCLEAN_UP_TCB( pxTCB )	task_end_hook((void *)pxTCB)
#else
#define portCLEAN_UP_TCB( pxTCB )	( void ) pxTCB
#endif

void vPortYield(void);
void vPortYieldFromIsr(void);

#ifdef __cplusplus
}
#endif

#endif	/* PORTMACRO_H */
