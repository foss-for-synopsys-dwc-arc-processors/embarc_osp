/*******************************************************************************
 MRF24WG Stub Functions

  Summary: This module contains prototypes for stub functions.

  Description: None
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

#ifndef __WF_STUBS_H
#define __WF_STUBS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

#include "wf_dev_hal.h"

extern uint8_t g_mrf24wgInit;
//== I/O Stub Function Protypes ========================================
//   Located in wf_io_stub.c
extern void WF_GpioInit(void);
extern void WF_GpioWrite(uint32_t value, uint32_t mask);
extern uint32_t WF_GpioRead(uint32_t mask);
extern void WF_IOSetChipSelect(uint8_t level);
extern void WF_GpioSetHibernate(uint8_t level);
extern void WF_GpioSetReset(uint8_t level);

//== MRF24WG SPI Stub Function Protypes ========================================
//   Located in wf_spi_stub.c
extern void WF_SpiInit(void);

//== MRF24WG External Interrupt Stub Function Protypes =========================
//   Located in wf_eint_stub.c
extern void WF_EintInit(void);
extern uint32_t WF_EintRead(void);
extern void WF_EintEnable(void);
extern void WF_EintDisable(void);
extern bool WF_isEintDisabled(void);
extern bool WF_isEintPending(void);
extern void WF_EintHandler(void);

//== MRF24WG 1ms Timer Stub Function Protypes ==================================
//   Located in wf_timer_stub.c
extern void WF_TimerInit(void);
extern uint32_t WF_TimerRead(void);
extern void WF_Delay_MS(uint32_t ms);

//== MRF24WG Event Handler Stub Function Protypes ==============================
// Located in wf_event_stub.c
extern void WF_ProcessEvent(DEV_WNIC *mrf24g_wnic, uint8_t eventType, uint32_t eventData);
extern void  RxPacketCheck(DEV_WNIC *mrf24g_wnic);

#define MRF24W_RST_SET(level) 	WF_GpioWrite( ((level)<<WF_GPIO_RST_PIN_OFS), WF_GPIO_RST_PIN_MASK)
#define MRF24W_WP_SET(level) 	WF_GpioWrite( ((level)<<WF_GPIO_WP_PIN_OFS), WF_GPIO_WP_PIN_MASK)
#define MRF24W_HIB_SET(level) 	WF_GpioWrite( ((level)<<WF_GPIO_HIB_PIN_OFS), WF_GPIO_HIB_PIN_MASK)
#define MRF24W_INT_READ()	WF_EintRead()

#ifdef __cplusplus
}
#endif


#endif /* __WF_STUBS_H */

