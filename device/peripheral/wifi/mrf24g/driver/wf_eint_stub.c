/*******************************************************************************
 MRF24WG External Interrupt Stub Functions

  Summary: Functions to control MRF24WG External Interrupt.

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
#include "board.h"

void _WFInterrupt(void *ptr);

#define WFINT_CHECK_EXP_NORTN(EXPR)     CHECK_EXP_NOERCD(EXPR, error_exit)
/*****************************************************************************
  Function:
    void WF_EintInit(void);

  Summary:
    Configure host processor external interrupt.  This line is asserted low by
    MRF24WG.

  Description:
    Called by Universal Driver during initialization.

 Parameters:
    None

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_EintInit(void)
{
    /** set int pin as input */
    uint32_t direction;
    WF_OPS *p_wf_ops = get_wf_ops();

    WFINT_CHECK_EXP_NORTN(p_wf_ops);

    direction = (GPIO_DIR_INPUT << WF_GPIO_INT_PIN_OFS);

    p_wf_ops->gpio_ops->gpio_open(WF_GPIO_INT_PIN_MASK, direction);
    p_wf_ops->gpio_ops->gpio_eint_control(WF_EINT_DISABLE);
    p_wf_ops->gpio_ops->gpio_eint_install(_WFInterrupt);

error_exit:
    return;
}

uint32_t WF_EintRead(void)
{
    uint32_t val = 0;
    WF_OPS *p_wf_ops = get_wf_ops();

    WFINT_CHECK_EXP_NORTN(p_wf_ops);

    p_wf_ops->gpio_ops->gpio_read(WF_GPIO_INT_PIN_MASK, &val);

error_exit:
    return val;
}

/*****************************************************************************
  Function:
    void WF_EintEnable(void);

  Summary:
    Enables the MRF24WG external interrupt

  Description:
    Called by Universal Driver during normal operations to enable the MRF24WG
    external interrupt.

    Note: When using level-triggered interrupts it is possible that host MCU
          could miss a falling edge; this can occur because during normal
          operations as the Universal Driver disables the external interrupt for
          short periods.  If the MRF24WG asserts the interrupt line while the
          interrupt is disabled the falling edge won't be detected.  So, a
          check must be made to determine if an interrupt is pending; if so, the
          interrupt must be forced.

          This is not an issue for level-triggered interrupts.

 Parameters:
    None

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_EintEnable(void)
{
    WF_OPS *p_wf_ops = get_wf_ops();

    WFINT_CHECK_EXP_NORTN(p_wf_ops);

    p_wf_ops->gpio_ops->gpio_eint_control(WF_EINT_ENABLE);

error_exit:
    return;
}

/*****************************************************************************
  Function:
    void WF_EintDisable(void);

  Summary:
    Disables the MRF24WG external interrupt

  Description:
    Called by Universal Driver during normal operations to disable the MRF24WG
    external interrupt.

 Parameters:
    None

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_EintDisable(void)
{
    WF_OPS *p_wf_ops = get_wf_ops();

    WFINT_CHECK_EXP_NORTN(p_wf_ops);

    p_wf_ops->gpio_ops->gpio_eint_control(WF_EINT_DISABLE);

error_exit:
    return;
}

/*****************************************************************************
  Function:
    bool WF_isEintDisabled(void);

  Summary:
    Determines if the external interrupt is disabled

  Description:
    Called by Universal Driver during normal operations to check if the current
    state of the external interrupt is disabled.

 Parameters:
    None

  Returns:
    True if interrupt is disabled, else False

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
bool WF_isEintDisabled(void)
{
    bool flag = true;
    int32_t int_state;
    WF_OPS *p_wf_ops = get_wf_ops();

    WFINT_CHECK_EXP_NORTN(p_wf_ops);

    int_state = p_wf_ops->gpio_ops->gpio_eint_is_enabled();
    flag = ((int_state==WF_EINT_DISABLE) ? true : false);

error_exit:
    return flag;
}

/*****************************************************************************
  Function:
    bool WF_isEintPending(void);

  Summary:
    Determines if the external interrupt is pending.

  Description:
    Called by Universal Driver during normal operations to check if an external
    interrupt is pending.

 Parameters:
    None

  Returns:
    True if interrupt is pending, else False

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
bool WF_isEintPending(void)
{
    uint32_t int_pin_val;

    int_pin_val = MRF24W_INT_READ();

    // if interrupt line is low, but interrupt request has not occurred
    return ( (int_pin_val==0)? true : false);
}


/*****************************************************************************
  Function:
    void _WFInterrupt(void);

  Summary:
    MRF24WG external interrupt handler

  Description:
    This interrupt handler should:
        1) ensure the interrupt is disabled upon exit (Universal Driver will reenable it)
        1) clear the interrupt
        2) call WFEintHandler()

 Parameters:
    None

  Returns:
    None

  Remarks:
    None
*****************************************************************************/
void _WFInterrupt(void *ptr)
{
    WF_EintDisable();
    /** go to interrupt handler */
    WF_EintHandler();           // call Univeral Driver handler function
}


