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

extern uint32_t _f_sdata;

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
 * scheduler control. The registers have to be placed on the stack in
 * the order that the port expects to find them.
 *
 * For ARC, task context switch is implemented with the help of TRAP exception
 *
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) portINITIAL_STATUS32;


	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0;		/* SEC_STAT */

	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) pxCode;		/* function body, eret */


	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00000000;	/* R31, blink */

	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x11111111;	/* R11 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x10101010;	/* R10 */
#ifndef ARC_FEATURE_RF16
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x09090909;	/* R9 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x08080808;	/* R8 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x07070707;	/* R7 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x06060606;	/* R6 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x05050505;	/* R5 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x04040404;	/* R4 */
#endif
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x03030303;	/* R3 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x02020202;	/* R2 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x01010101;	/* R1 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) pvParameters;	/* R0 */

#if ARC_FEATURE_CODE_DENSITY
	/* code density related regs */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00000000;	/* JLI_BASE*/
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00000000;	/* LDI_BASE*/
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00000000;	/* EI_BASE*/
#endif

	/* LP related regs */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00000000;	/* LP_COUNT */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00000000;	/* LP_START */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00000000;	/* LP_END */


	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x12121212;	/* R12 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) &_f_sdata;	/* gp */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00000000;	/* fp */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00000000;	/* ilink */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x30303030;	/* R30 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00000000;	/* BTA */
/** CALLEE saved regs */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x13131313;	/* R13 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x14141414;	/* R14 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x15151515;	/* R15 */
#ifndef ARC_FEATURE_RF16
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x16161616;	/* R16 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x17171717;	/* R17 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x18181818;	/* R18 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x19191919;	/* R19 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x20202020;	/* R20 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x21212121;	/* R21 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x22222222;	/* R22 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x23232323;	/* R23 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x24242424;	/* R24 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x25252525;	/* R25 */
#endif

#if ARC_FEATURE_FPU_DSP_CONTEXT

#if ARC_FEATURE_DSP || ARC_FEATURE_FPU || ARC_FEATURE_MPU_OPTION_NUM > 6
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x58585858;	/* R58 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x59595959;	/* R58 */
#endif

#if ARC_FEATURE_FPU
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) _arc_aux_read(AUX_FPU_CTRL);	/* AUX_FPU_CTRL */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) _arc_aux_read(AUX_FPU_STATUS);	/* AUX_FPU_STATUS */

#if ARC_FEATURE_FPU_DA
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) _arc_aux_read(AUX_FPU_DPFP1L);	/* AUX_FPU_DPFP1L */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) _arc_aux_read(AUX_FPU_DPFP1H);	/* AUX_FPU_DPFP1H */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) _arc_aux_read(AUX_FPU_DPFP2L);	/* AUX_FPU_DPFP2L */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) _arc_aux_read(AUX_FPU_DPFP2H);	/* AUX_FPU_DPFP2H */
#endif

#endif /* ARC_FEATURE_FPU */

#if ARC_FEATURE_DSP
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) _arc_aux_read(AUX_DSP_CTRL);	/* AUX_DSP_CTRL */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) _arc_aux_read(AUX_ACC0_LO);	/* AUX_ACC0_LO */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) _arc_aux_read(AUX_ACC0_GLO);	/* AUX_ACC0_GLO */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) _arc_aux_read(AUX_ACC0_HI);	/* AUX_ACC0_HI */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) _arc_aux_read(AUX_ACC0_GHI);	/* AUX_ACC0_GHI */

#if defined(ARC_FEATURE_DSP_COMPLEX)
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) _arc_aux_read(AUX_DSP_BFLY0);	/* AUX_DSP_BFLY0 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) _arc_aux_read(AUX_DSP_FFT_CTRL);/* AUX_DSP_FFT_CTRL */
#endif

#endif /* ARC_FEATURE_DSP */

#endif /* ARC_FEATURE_FPU_DSP_CONTEXT */

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
	Asm("trap_s 1");
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
