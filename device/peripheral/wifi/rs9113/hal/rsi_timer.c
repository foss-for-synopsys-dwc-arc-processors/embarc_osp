/**
 * @file       rsi_hal_mcu_timer.c
 * @version    0.1
 * @date       15 Aug 2015
 *
 * Copyright(C) Redpine Signals 2015
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief HAL TIMER: Functions related to HAL timers
 *
 * @section Description
 * This file contains the list of functions for linux timers.
 *
 */


/**
 * Includes
 */
#ifdef LINUX_PLATFORM
#ifdef RSI_ZB_ENABLE
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>


void (*timer_func_handler_pntr)(void);


#ifdef __linux__

void timer_handler(int);

struct itimerval timervalue;
struct itimerval timervalue_read;

struct sigaction new_handler, old_handler;

void timer_sig_handler(int);

#define CLOCKID CLOCK_REALTIME


int start_timer(int uSec, void (*timer_func_handler)(void))
{
  timer_func_handler_pntr = timer_func_handler;

  //! Single Shot
  timervalue.it_interval.tv_sec = 0;
  timervalue.it_interval.tv_usec = 0;

  timervalue.it_value.tv_sec = uSec / 1000000;
  timervalue.it_value.tv_usec = 0 ;
  if(setitimer(ITIMER_REAL, &timervalue, NULL))
  {
    printf("\nsetitimer() error\n");
    return(1);
  }

  new_handler.sa_handler = &timer_sig_handler;
  new_handler.sa_flags = SA_NOMASK;
  if(sigaction(SIGALRM, &new_handler, &old_handler))
  {
    printf("\nsigaction() error\n");
    return(1);
  }

  return(0);
}

int read_time()
{
  getitimer(ITIMER_REAL, &timervalue_read);
  return timervalue_read.it_value.tv_usec;
}

void timer_sig_handler(int arg)
{
  timer_func_handler_pntr();
}


void stop_timer(void)
{
  timervalue.it_interval.tv_sec = 0;
  timervalue.it_interval.tv_usec = 0;
  timervalue.it_value.tv_sec = 0;
  timervalue.it_value.tv_usec = 0;
  setitimer(ITIMER_REAL, &timervalue, NULL);

  sigaction(SIGALRM, &old_handler, NULL);
}

/*===================================================*/
/**
 * @fn           void rsi_zigb_delay_ms(uint32_t delay)
 * @brief        create delay in micro seconds
 * @param[in]    delay, timer delay in micro seconds
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to create delay in micro seconds
 *
 */

#endif
#endif
#endif













