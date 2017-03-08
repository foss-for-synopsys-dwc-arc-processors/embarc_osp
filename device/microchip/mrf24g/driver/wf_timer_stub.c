/*******************************************************************************
 MRF24WG Universal Driver Timer Stub Functions

  Summary: This module contains Timer functions needed by the Universal Driver.

*******************************************************************************/

/* MRF24WG0M Universal Driver
*
* Copyright (c) 2012-2013, Microchip <www.microchip.com>
* Contact Microchip for the latest version.
*
* This program is free software; distributed under the terms of BSD
* license:
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1.    Redistributions of source code must retain the above copyright notice, this
*        list of conditions and the following disclaimer.
* 2.    Redistributions in binary form must reproduce the above copyright notice,
*        this list of conditions and the following disclaimer in the documentation
*        and/or other materials provided with the distribution.
* 3.    Neither the name(s) of the above-listed copyright holder(s) nor the names
*        of its contributors may be used to endorse or promote products derived
*        from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*
 * Modified for port to ARC processor
 * by Huaqi Fang huaqi.fang@synopsys.com, Synopsys, Inc.
 */

//==============================================================================
//                                  INCLUDES
//==============================================================================
#include "wf_dev_hal.h"

#include "./inc/shared/wf_universal_driver.h"

#define WFTIMER_CHECK_EXP_NORTN(EXPR)     CHECK_EXP_NOERCD(EXPR, error_exit)

/*****************************************************************************
  Function:
    uint32_t WF_TimerInit(void);

  Summary:
    Configures a timer that increments a 32-bit counter every 1ms.

  Description:
    Called by Universal Driver during initialization to start a 1ms timer that
    will be used for various reasons.

 Parameters:
    None

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_TimerInit(void)
{
    WF_OPS *p_wf_ops = get_wf_ops();

    WFTIMER_CHECK_EXP_NORTN(p_wf_ops);

    p_wf_ops->timer_ops->timer_init(WF_TIMER_FREQ_1K);

error_exit:
    return;
}

/*****************************************************************************
  Function:
    uint32_t WF_TimerRead();

  Summary:
    Called by the Universal Driver for various timing operations.

  Description:
    Returns the current value of the 1ms timer.

 Parameters:
    None

  Returns:
    Current value of 1ms timer

  Remarks:
    None
*****************************************************************************/
uint32_t WF_TimerRead(void)
{
    WF_OPS *p_wf_ops = get_wf_ops();

    WFTIMER_CHECK_EXP_NORTN(p_wf_ops);

    return p_wf_ops->timer_ops->timer_read_ms();

error_exit:
    return 0;
}


void WF_Delay_MS(uint32_t ms)
{
    WF_OPS *p_wf_ops = get_wf_ops();

    WFTIMER_CHECK_EXP_NORTN(p_wf_ops);

    return p_wf_ops->timer_ops->timer_delay(ms);

error_exit:
    return;
}