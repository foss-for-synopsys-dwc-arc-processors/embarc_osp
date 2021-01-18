/*
 * Copyright (c) 2021, Synopsys, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
*********************************************************************************************************
*
*                                           ARC Port
*
* Filename  : os_cpu_c.c
* Version   : V2.93.00
*********************************************************************************************************
* For       : Synopsys ARC
* Toolchain : GNU C Compiler & MWDT Compiler
*********************************************************************************************************
* Note(s)   : Hardware FP is not supported.
*********************************************************************************************************
*/

#define  OS_CPU_GLOBALS

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <ucos_ii.h>
#include "embARC.h"

/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/

#if OS_TMR_EN > 0u
static  INT16U  OSTmrCtr;
#endif


/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                            (BEGINNING)
*
* Description: This function is called by OSInit() at the beginning of OSInit().
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/

static void prvSetupTimerInterrupt(void);

#if OS_CPU_HOOKS_EN > 0u
void  OSInitHookBegin (void)
{
#if OS_TMR_EN > 0u
    OSTmrCtr = 0u;
#endif
    prvSetupTimerInterrupt();
}
#endif


/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                               (END)
*
* Description: This function is called by OSInit() at the end of OSInit().
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSInitHookEnd (void)
{

}
#endif


/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
*
* Description: This function is called when a task is created.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTaskCreateHook (OS_TCB  *p_tcb)
{
#if OS_APP_HOOKS_EN > 0u
    App_TaskCreateHook(p_tcb);
#else
    (void)ptcb;                                /* Prevent compiler warning                             */
#endif
}
#endif


/*
*********************************************************************************************************
*                                           TASK DELETION HOOK
*
* Description: This function is called when a task is deleted.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTaskDelHook (OS_TCB  *p_tcb)
{
#if OS_APP_HOOKS_EN > 0u
    App_TaskDelHook(p_tcb);
#else
    (void)ptcb;                                /* Prevent compiler warning                             */
#endif
}
#endif


/*
*********************************************************************************************************
*                                             IDLE TASK HOOK
*
* Description: This function is called by the idle task.  This hook has been added to allow you to do
*              such things as STOP the CPU to conserve power.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTaskIdleHook (void)
{
#if OS_APP_HOOKS_EN > 0u
    App_TaskIdleHook();
#endif
}
#endif


/*
*********************************************************************************************************
*                                            TASK RETURN HOOK
*
* Description: This function is called if a task accidentally returns.  In other words, a task should
*              either be an infinite loop or delete itself when done.
*
* Arguments  : ptcb      is a pointer to the task control block of the task that is returning.
*
* Note(s)    : none
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTaskReturnHook (OS_TCB  *p_tcb)
{
#if OS_APP_HOOKS_EN > 0u
    App_TaskReturnHook(p_tcb);
#else
    (void)ptcb;
#endif
}
#endif


/*
*********************************************************************************************************
*                                           STATISTIC TASK HOOK
*
* Description: This function is called every second by uC/OS-II's statistics task.  This allows your
*              application to add functionality to the statistics task.
*
* Arguments  : none
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTaskStatHook (void)
{
#if OS_APP_HOOKS_EN > 0u
    App_TaskStatHook();
#endif
}
#endif

#if ARC_FEATURE_STACK_CHECK
#define ARC_INIT_STATUS ((1 << AUX_STATUS_BIT_SC) | AUX_STATUS_MASK_IE | ((-1 - INT_PRI_MIN) << 1) | STATUS32_RESET_VALUE)
#else
#define ARC_INIT_STATUS (AUX_STATUS_MASK_IE | ((-1 - INT_PRI_MIN) << 1) | STATUS32_RESET_VALUE)
#endif

extern void start_r(void);
extern void OS_TaskReturn(void);

uint32_t g_context_switch_reqflg;
uint32_t g_exc_nest_count;

struct init_stack_frame {
    OS_STK pc;
    OS_STK blink;
    OS_STK task;
    OS_STK status32;
    OS_STK r0;
};

OS_STK  *OSTaskStkInit (void   (*task)(void *p_arg),
                        void    *p_arg,
                        OS_STK  *p_tos,
                        INT16U  opt)
{
    struct init_stack_frame *stack_frame;

    OS_STK *p_stk;
    (void) opt;                                /* 'opt' is not used, prevent warning */

    p_stk = (OS_STK *)((OS_STK)(p_tos) & 0xFFFFFFFCu); /* Load stack pointer and align it to 4-bytes*/

    p_stk -= sizeof(struct init_stack_frame) / sizeof(OS_STK);

    stack_frame = (struct init_stack_frame *)p_stk;

    stack_frame->pc = (uint32_t)start_r;
    stack_frame->blink = (uint32_t)OS_TaskReturn;
    stack_frame->task = (uint32_t)task;
    stack_frame->status32 = ARC_INIT_STATUS;
    stack_frame->r0 = (uint32_t)p_arg;

    return p_stk;
}

#if ARC_FEATURE_STACK_CHECK
void set_hw_stack_check(OS_TCB *from, OS_TCB *to)
{
    if (to != NULL) {
#if ARC_FEATURE_SEC_PRESENT
        arc_aux_write(AUX_S_KSTACK_TOP, (uint32_t)(to->task_stack_base));
        arc_aux_write(AUX_S_KSTACK_BASE, (uint32_t)(to->task_stack_base + to->stack_size);
#else
        arc_aux_write(AUX_KSTACK_TOP, (uint32_t)(to->task_stack_base));
        arc_aux_write(AUX_KSTACK_BASE, (uint32_t)(to->task_stack_base + to->stack_size));
#endif
    }
}
#endif

/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
*
* Description: This function is called when a task switch is performed.  This allows you to perform other
*              operations during a context switch.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                 will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                 task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if (OS_CPU_HOOKS_EN > 0u) && (OS_TASK_SW_HOOK_EN > 0u)
void  OSTaskSwHook (void)
{
#if OS_APP_HOOKS_EN > 0u
    App_TaskSwHook();
#endif

    OS_TRACE_TASK_SWITCHED_IN(OSTCBHighRdy);
}
#endif


/*
*********************************************************************************************************
*                                           OS_TCBInit() HOOK
*
* Description: This function is called by OS_TCBInit() after setting up most of the TCB.
*
* Arguments  : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0u
void  OSTCBInitHook (OS_TCB  *p_tcb)
{
#if OS_APP_HOOKS_EN > 0u
    App_TCBInitHook(p_tcb);
#else
    (void)ptcb;                                /* Prevent compiler warning                             */
#endif
}
#endif


/*
*********************************************************************************************************
*                                               TICK HOOK
*
* Description: This function is called every tick.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if (OS_CPU_HOOKS_EN > 0u) && (OS_TIME_TICK_HOOK_EN > 0u)
void  OSTimeTickHook (void)
{
#if OS_APP_HOOKS_EN > 0u
    App_TimeTickHook();
#endif

#if OS_TMR_EN > 0u
    OSTmrCtr++;
    if (OSTmrCtr >= (OS_TICKS_PER_SEC / OS_TMR_CFG_TICKS_PER_SEC)) {
        OSTmrCtr = 0u;
        OSTmrSignal();
    }
#endif
}
#endif


/*
*********************************************************************************************************
*                                          SYS TICK HANDLER
*
* Description: Handle the system tick (SysTick) interrupt, which is used to generate the uC/OS-II tick
*              interrupt.
*
* Arguments  : None.
*
**********************************************************************************************************/


static void vKernelTick( void *ptr )
{
    /* clear timer interrupt */
    timer_int_clear(BOARD_OS_TIMER_ID);

    OSTimeTick();/* Call uC/OS-II's OSTimeTick()*/

}

static void prvSetupTimerInterrupt(void)
{
    unsigned int cyc = BOARD_CPU_CLOCK / OS_TICKS_PER_SEC;

    int_disable(BOARD_OS_TIMER_INTNO); /* disable os timer interrupt */
    timer_stop(BOARD_OS_TIMER_ID);
    timer_start(BOARD_OS_TIMER_ID, TIMER_CTRL_IE | TIMER_CTRL_NH, cyc);

    int_handler_install(BOARD_OS_TIMER_INTNO, (INT_HANDLER)vKernelTick);
    int_pri_set(BOARD_OS_TIMER_INTNO, INT_PRI_MIN + 1);
    int_enable(BOARD_OS_TIMER_INTNO);
}

#ifdef __GNU__
extern void gnu_printf_setup(void);
#endif

void ucos_exc_init(void)
{

#ifdef __GNU__
    gnu_printf_setup();
#endif

}
