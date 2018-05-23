/**
 * @file     rsi_timer.c
 * @version  0.1
 * @date     15 Aug 2015
 *
 *  Copyright(C) Redpine Signals 2015
 *  All rights reserved by Redpine Signals.
 *
 *  @section License
 *  This program should be used on your own responsibility.
 *  Redpine Signals assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains the timer wrappers related functionality
 *
 *  @section Description  This file contains the timer wrappers related functionality
 *
 *
 */
/**
 * Include files
 * */
#include "rsi_driver.h"
#include "rsi_timer.h"
#include "rsi_hal.h"


volatile uint32_t  timer_counter = 0;


/*==============================================*/
/**
 * @fn          void rsi_timer_expiry_interrupt_handler()
 * @brief       timer interrupt handler which is called for every milli second
 * @param[in]   none
 * @param[out]  none
 * @return      void
 *
 *
 * @section description
 * This function is called for every milli second and increments the timer counter
 *
 *
 */
void rsi_timer_expiry_interrupt_handler()
{
	timer_counter++;
#ifdef SPI_INTERFACE
#ifdef RSI_HWTIMER
	rsi_spiTimer2++;
	rsi_spiTimer1++;
	rsi_spiTimer3++;
#endif
#endif

}


/*==============================================*/
/**
 * @fn           uint32_t rsi_timer_read_counter()
 * @brief        This function returns the timer counter value
 * @param[in]    none
 * @param[out]   none
 * @return       uint32_t
 *               timer counter value
 *
 *
 * @section description
 * This function returns the timer counter value
 *
 *
 */
uint32_t rsi_timer_read_counter()
{
	return timer_counter;
}







/*==============================================*/
/**
 * @fn            void rsi_init_timer(rsi_timer_instance_t *rsi_timer, uint32_t duration)
 * @brief         This function initialises the timer instance with the expiry time
 * @param[in]     rsi_timer, timer instance
 * @param[in]     duration, duration in milli seconds
 * @return        none
 *
 * @section description
 *  This function initialises the timer instance with the expiry time
 *
 *
 */
void rsi_init_timer(rsi_timer_instance_t *rsi_timer, uint32_t duration)
{
	*rsi_timer  =  rsi_timer_read_counter() + duration;
}






/*==============================================*/
/**
 * @fn           int32_t rsi_timer_expired(rsi_timer_instance_t *timer)
 * @brief        This function is used to check whether the timer instance is expired or not
 * @param[in]    rsi_timer, timer instance
 * @param[out]   none
 * @return       int32_t
 *               1 - if timer is expired
 *               0 - if timer is not expired
 *
 *
 * @section description
 * This function is used to check whether the timer instance is expired or not
 *
 *
 */

int32_t rsi_timer_expired(rsi_timer_instance_t *timer)
{
	long left = ((*timer )- rsi_timer_read_counter());

	if(left < 0)
		return 1;
	else
		return 0;

}









/*==============================================*/
/**
 * @fn           uint32_t rsi_timer_left(rsi_timer_instance_t *timer)
 * @brief        This function is used get the remaining time
 * @param[in]    rsi_timer, timer instance
 * @param[out]   none
 * @return       int32_t
 *               >0 - time left to expire
 *               0 - if timer is expired
 *
 *
 * @section description
 *  This function is used get the remaining time for timer expiry
 *
 *
 */
uint32_t rsi_timer_left(rsi_timer_instance_t *timer)
{
	long left = ((*timer )- rsi_timer_read_counter());
	return (left < 0) ? 0 : left;
}
