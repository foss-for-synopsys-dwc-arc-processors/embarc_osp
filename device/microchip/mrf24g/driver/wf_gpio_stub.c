/*******************************************************************************
 MRF24WG GPIO Stub Functions

  Summary: Functions to control MRF24WG RESET and HIBERNATE pins needed by the
           Universal Driver.  Functions in this module should not be called
           directly by the application code.
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

#define WFGPIO_CHECK_EXP_NORTN(EXPR)     CHECK_EXP_NOERCD(EXPR, error_exit)

/*****************************************************************************
  Function:
    void WF_GpioInit(void);

  Summary:
    Configures GPIO pins used for RESET and HIBERNATE as outputs

  Description:
    Called by Universal Driver during initialization.

    This function should configure port pins used to control MRF24WG RESET and
    HIBERNATE pins as outputs.  Always set the level first, then set the port as
    an output to avoid a glitch.  This function initially sets the HIBERNATE
    pin high (in hibernate mode) and sets the RESET line low (in reset).

 Parameters:
    None

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_GpioInit(void)
{
    uint32_t mask, direction;
    WF_OPS *p_wf_ops = get_wf_ops();

    WFGPIO_CHECK_EXP_NORTN(p_wf_ops);

    mask      = (WF_GPIO_RST_PIN_MASK) | \
                (WF_GPIO_WP_PIN_MASK)  | \
                (WF_GPIO_HIB_PIN_MASK) ;
    direction = (GPIO_DIR_OUTPUT << WF_GPIO_RST_PIN_OFS) | \
                (GPIO_DIR_OUTPUT << WF_GPIO_WP_PIN_OFS)  | \
                (GPIO_DIR_OUTPUT << WF_GPIO_HIB_PIN_OFS) ;

    p_wf_ops->gpio_ops->gpio_open(mask, direction);

    MRF24W_WP_SET(LOW_LEVEL);

    // Set level on HIBERNATE I/O to high (MRF24WG in hibernate) and then
    // configure as an output.
    MRF24W_HIB_SET(HIGH_LEVEL);

    // Set level on RESET I/O to low (MRF24WG in reset) and then
    // configure as an output.
    MRF24W_RST_SET(LOW_LEVEL);

error_exit:
    return;
}

void WF_GpioWrite(uint32_t value, uint32_t mask)
{
    WF_OPS *p_wf_ops = get_wf_ops();

    WFGPIO_CHECK_EXP_NORTN(p_wf_ops);

    p_wf_ops->gpio_ops->gpio_write(mask, value);

error_exit:
    return;
}

uint32_t WF_GpioRead(uint32_t mask)
{
    uint32_t value = 0;
    WF_OPS *p_wf_ops = get_wf_ops();

    WFGPIO_CHECK_EXP_NORTN(p_wf_ops);

    p_wf_ops->gpio_ops->gpio_read(mask, &value);

error_exit:
    return value;
}

/*****************************************************************************
  Function:
    void WF_GpioSetReset(uint8_t level);

  Summary:
    Puts the MRF24WG into and out of reset.

  Description:
    Called by Universal Driver during initialization to take the MRF24WG out of
    reset.

    Sets the RESET line high or low.

 Parameters:
    level -- WF_HIGH or WF_LOW

  Returns:
    None

  Remarks:
     Should not be called directly from application code.
*****************************************************************************/
void WF_GpioSetReset(uint8_t level)
{
    MRF24W_RST_SET(level);
}

/*****************************************************************************
  Function:
    void WF_GpioSetHibernate(uint8_t level);

  Summary:
    Puts the MRF24WG into and out of hibernate.

  Description:
    Called by Universal Driver during initialization to take the MRF24WG out of
    reset.

    Sets the HIBERNATE line high or low.

 Parameters:
    level -- WF_HIGH or WF_LOW

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_GpioSetHibernate(uint8_t level)
{
    MRF24W_HIB_SET(level);
}
