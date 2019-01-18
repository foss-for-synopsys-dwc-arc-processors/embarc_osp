/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

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
 * Includes
 */

#include "rsi_driver.h"
#include "embARC.h"



/*===================================================*/
/**
 * @fn           int32_t rsi_timer_start(uint8_t timer_no, uint8_t mode,uint8_t type,uint32_t duration,void (* rsi_timer_expiry_handler)())
 * @brief        Starts and configures timer
 * @param[in]    timer_node, timer node to be configured.
 * @param[in]    mode , mode of the timer
 *               0 - Micro seconds mode
 *               1 - Milli seconds mode
 * @param[in]    type, type of  the timer
 *               0 - single shot type
 *               1 - periodic type
 * @param[in]    duration, timer duration
 * @param[in]    rsi_timer_expiry_handler() ,call back function to handle timer interrupt
 * @param[out]   none
 * @return       0 - success
 *               !0 - Failure
 * @description  This HAL API should contain the code to initialize the timer and start the timer
 *
 */
//TimerHandle_t rsi_timer;
int32_t rsi_timer_start(uint8_t timer_node, uint8_t mode, uint8_t type, uint32_t duration, void (* rsi_timer_expiry_handler)(void))
{
	// ! Initialise the timer
// 	uint32_t int_no;
// 	if(timer_node != TIMER_0){
// 		return -1;
// 	} else {
// 		int_no = INTNO_TIMER0 ;
// 	}
// 	timer_present(timer_node);
// 	timer_init();
// 	//! register the call back
// 	int_handler_install(int_no, rsi_timer_expiry_handler);
// 	//! Start timer
// 	timer_start(timer_node, mode, duration);
	return 0;
}


/*===================================================*/
/**
 * @fn           int32_t rsi_timer_stop(uint8_t timer_no)
 * @brief        Stops timer
 * @param[in]    timer_node, timer node to stop
 * @param[out]   none
 * @return       0 - success
 *               !0 - Failure
 * @description  This HAL API should contain the code to stop the timer
 *
 */

int32_t rsi_timer_stop(uint8_t timer_node)
{
	//! Stop the timer
	// timer_stop(timer_node);
	return 0;
}

/*===================================================*/
/**
 * @fn           uint32_t rsi_timer_read(uint8_t timer_node)
 * @brief        read timer
 * @param[in]    timer_node, timer node to read
 * @param[out]   none
 * @return       timer value
 * @description  This HAL API should contain API to  read the timer
 *
 */

uint32_t rsi_timer_read(uint8_t timer_node)
{
	volatile uint32_t timer_val = 0;

	//! read the timer and return timer value
	// timer_current(timer_node, (void *)(&timer_val));
	return timer_val;
}

/*===================================================*/
/**
 * @fn           void rsi_delay_us(uint32_t delay)
 * @brief        create delay in micro seconds
 * @param[in]    delay_us, timer delay in micro seconds
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to create delay in micro seconds
 *
 */
void rsi_delay_us(uint32_t delay_us)
{
    //! call the API for delay in micro seconds
	return;
}

/*===================================================*/
/**
 * @fn           void rsi_delay_ms(uint32_t delay)
 * @brief        create delay in micro seconds
 * @param[in]    delay, timer delay in micro seconds
 * @param[out]   none
 * @return       none
 * @description  This HAL API should contain the code to create delay in micro seconds
 *
 */
void rsi_delay_ms(uint32_t delay_ms)
{
	//! call the API for delay in milli seconds

#ifdef OS_FREERTOS
	vTaskDelay(delay_ms);
#else
	board_delay_ms(delay_ms, OSP_DELAY_OS_COMPAT_ENABLE);
#endif
	return;
}
