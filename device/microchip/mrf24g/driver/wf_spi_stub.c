/*******************************************************************************
 MRF24WG SPI Stub Functions

  Summary: Functions to control MRF24WG RESET and HIBERNATE pins need by the
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

#define WFSPI_CHECK_EXP_NORTN(EXPR)     CHECK_EXP_NOERCD(EXPR, error_exit)

/*****************************************************************************
  Function:
    void WF_SpiInit(void);

  Summary:
    Initializes SPI controller used to communicate with MRF24WG

  Description:
    Called by Universal Driver during initialization to initialize SPI interface.

 Parameters:
    None

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_SpiInit(void)
{
    WF_OPS *p_wf_ops = get_wf_ops();

    WFSPI_CHECK_EXP_NORTN(p_wf_ops);

    p_wf_ops->spi_ops->spi_open(p_wf_ops->spi_ops->working_freq, p_wf_ops->spi_ops->clk_mode);

error_exit:
    return;
}

/*****************************************************************************
  Function:
    void WF_SpiEnableChipSelect(void);

  Summary:
    Selects the MRF24WG SPI by setting the CS line low.

  Description:
    Called by Universal Driver when preparing to transmit data to the MRF24WG.

 Parameters:
    None

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_SpiEnableChipSelect(void)
{
    WF_OPS *p_wf_ops = get_wf_ops();

    WFSPI_CHECK_EXP_NORTN(p_wf_ops);

    p_wf_ops->spi_ops->spi_cs(WF_CHIP_SELECTED);

error_exit:
    return;
}

/*****************************************************************************
  Function:
    void WF_SpiDisableChipSelect(void);

  Summary:
    Deselects the MRF24WG SPI by setting CS high.

  Description:
    Called by Universal Driver after completing an SPI transaction.

 Parameters:
    None

  Returns:
    None

  Remarks:
    Should not be called directly from application code.
*****************************************************************************/
void WF_SpiDisableChipSelect(void)
{
    WF_OPS *p_wf_ops = get_wf_ops();

    WFSPI_CHECK_EXP_NORTN(p_wf_ops);

    p_wf_ops->spi_ops->spi_cs(WF_CHIP_DESELECTED);

error_exit:
    return;
}