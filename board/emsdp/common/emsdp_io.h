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
#ifndef _EMSDP_GPIO_H_
#define _EMSDP_GPIO_H_

#include "dev_gpio.h"

/**
 * EMSDP LED Port Configuration
 */
#define EMSDP_LED_OFFSET	(0)
#define EMSDP_LED_MASK		((0xff) << EMSDP_LED_OFFSET)
#define EMSDP_LED_PORT		(DEV_GPIO_PORT_B)
#define EMSDP_LED_DIR		((0xff) << EMSDP_LED_OFFSET)

/**
 * EMSDP Switch Port Configuration
 */
#define EMSDP_SWITCH_OFFSET	(0)
#define EMSDP_SWITCH_MASK	((0xf) << EMSDP_SWITCH_OFFSET)
#define EMSDP_SWITCH_PORT	(DEV_GPIO_PORT_A)
#define EMSDP_SWITCH_DIR	((0x0) << EMSDP_SWITCH_OFFSET)

#ifdef __cplusplus
extern "C" {
#endif

extern void emsdp_led_init(void);
extern void emsdp_switch_init(void);
extern void emsdp_io_init(void);
extern void led_write(uint32_t led_val, uint32_t mask);
extern uint32_t led_read(uint32_t mask);
extern void led_toggle(uint32_t mask);
extern uint32_t switch_read(uint32_t mask);

#ifdef __cplusplus
}
#endif

#endif /* _EMSDP_GPIO_H_ */
