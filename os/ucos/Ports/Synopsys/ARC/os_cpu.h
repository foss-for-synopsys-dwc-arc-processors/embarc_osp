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

#ifndef  OS_CPU_H
#define  OS_CPU_H

#ifdef   OS_CPU_GLOBALS
#define  OS_CPU_EXT
#else
#define  OS_CPU_EXT  extern
#endif


/*
*********************************************************************************************************
*                                     EXTERNAL C LANGUAGE LINKAGE
*
* Note(s) : (1) C++ compilers MUST 'extern'ally declare ALL C function prototypes & variable/object
*               declarations for correct C language linkage.
*********************************************************************************************************
*/

#ifdef __cplusplus
extern  "C" {                                    /* See Note #1.                                       */
#endif


/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/

typedef  unsigned  char        BOOLEAN;          /* Unsigned  8 bit boolean or logical                 */
typedef  unsigned  char        INT8U;            /* Unsigned  8 bit quantity                           */
typedef  signed    char        INT8S;            /* Signed    8 bit quantity                           */
typedef  unsigned  short       INT16U;           /* Unsigned 16 bit quantity                           */
typedef  signed    short       INT16S;           /* Signed   16 bit quantity                           */
typedef  unsigned  int         INT32U;           /* Unsigned 32 bit quantity                           */
typedef  signed    int         INT32S;           /* Signed   32 bit quantity                           */
typedef  unsigned  long  long  INT64U;           /* 64-bit unsigned integer                            */
typedef  signed    long  long  INT64S;           /* 64-bit   signed integer                            */
typedef  float                 FP32;             /* Single precision floating point                    */
typedef  double                FP64;             /* Double precision floating point                    */

typedef  unsigned  int         OS_STK;           /* Each stack entry is 32-bit wide                    */
typedef  unsigned  int         OS_CPU_SR;        /* Define size of Machine status register (32 bits)   */


/*
*********************************************************************************************************
*                                               ARC
*                                      Critical Section Management
*
* Method #1:  Disable/Enable interrupts using simple instructions.  After critical section, interrupts
*             will be enabled even if they were disabled before entering the critical section.
*             NOT IMPLEMENTED
*
* Method #2:  Disable/Enable interrupts by preserving the state of interrupts.  In other words, if
*             interrupts were disabled before entering the critical section, they will be disabled when
*             leaving the critical section.
*             NOT IMPLEMENTED
*
* Method #3:  Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
*             would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
*             disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
*             disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
*             into the CPU's status register.
*********************************************************************************************************
*/

#define  OS_CRITICAL_METHOD   3u

#if OS_CRITICAL_METHOD == 3u
#define  OS_ENTER_CRITICAL()  do { cpu_sr = OS_CPU_SR_Save();} while (0)
#define  OS_EXIT_CRITICAL()   do { OS_CPU_SR_Restore(cpu_sr);} while (0)
#endif


/*
*********************************************************************************************************
*                                        ARC Miscellaneous
*********************************************************************************************************
*/

#define  OS_STK_GROWTH        1u                  /* Stack grows from HIGH to LOW memory               */

#define  OS_TASK_SW()         OSCtxSw()


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#if OS_CRITICAL_METHOD == 3u                      /* See OS_CPU_A.S                                    */
OS_CPU_SR  OS_CPU_SR_Save   (void);
void       OS_CPU_SR_Restore(OS_CPU_SR  cpu_sr);
#endif

void       OSCtxSw          (void);
void       OSIntCtxSw       (void);
void       OSStartHighRdy   (void);

void       ucos_exc_init    (void);


/*
*********************************************************************************************************
*                                   EXTERNAL C LANGUAGE LINKAGE END
*********************************************************************************************************
*/

#ifdef __cplusplus
}                                                 /* End of 'extern'al C lang linkage.                 */
#endif


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif
