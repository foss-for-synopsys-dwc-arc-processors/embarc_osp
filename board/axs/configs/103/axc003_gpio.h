/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

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
#ifndef _AXC003_GPIO_H_
#define _AXC003_GPIO_H_

#include "../../drivers/ip/designware/gpio/dw_gpio_obj.h"


/*
 * CPU GPIO PORT A description
 * bit 0 : LED2501
 * bit 1 : LED2502
 * bit 2-4 : reserved
 * bit 5 : LED2503
 * bit 6 : LED2504
 * bit 7-9 : reserved
 * bit 10 : LED2505
 * bit 11 : LED2506
 * bit 12 : MB ICTL in
 * bit 13-14 : reserved
 * bit 15 : LED2507
 * bit 16 : LED2508
 * bit 17-19 : reserved
 * bit 20-23 : SW2504-SW2507
 * bit 24-31 : reserved
 */
#define CPU_POARTA_DIR 0x18C63
/*
 * CPU GPIO PORT B description
 * bit 0-7 : JP1201 - JP1207
 * bit 8-15 : LED0-7 on cpu card
 * bit 16-31 : 2 7-segments display
 */
#define CPU_PORTB_DIR 0xFFFFFFF0


/**
 * \name	AXC003 LED Port A Configuration
 * @{
 */
#define AXC003_LEDA_MASK	CPU_POARTA_DIR
#define AXC003_LEDA_PORT	(CPU_DW_GPIO_PORT_A)
#define AXC003_LEDA_DIR		CPU_POARTA_DIR
/** @} end of name */

/**
 * \name	AXC003 LED Port B Configuration
 * @{
 */
#define AXC003_LEDB_OFFSET	(8)
#define AXC003_LEDB_MASK	((0xff) << AXC003_LEDB_OFFSET)
#define AXC003_LEDB_PORT	(CPU_DW_GPIO_PORT_B)
#define AXC003_LEDB_DIR		((0xff) << AXC003_LEDB_OFFSET)
/** @} end of name */


/**
 * \name	AXC003 7-segment Port Configuration
 * @{
 */
#define AXC003_7SEG_OFFSET	(16)
#define AXC003_7SEG_MASK	((0xffff) << AXC003_7SEG_OFFSET)
#define AXC003_7SEG_PORT	(CPU_DW_GPIO_PORT_B)
#define AXC003_7SEG_DIR		((0xffff) << AXC003_7SEG_OFFSET)
/** @} end of name */

/**
 * \name	AXC003 Button Port Configuration
 * @{
 */
#define AXC003_BUTTON_OFFSET	(20)
#define AXC003_BUTTON_MASK	((0xf) << AXC003_BUTTON_OFFSET)
#define AXC003_BUTTON_PORT	(CPU_DW_GPIO_PORT_A)
#define AXC003_BUTTON_DIR	((0x0) << AXC003_BUTTON_OFFSET)

#define AXC003_BUTTON_SW2504_OFFSET	(20)
#define AXC003_BUTTON_SW2505_OFFSET	(22)
#define AXC003_BUTTON_SW2506_OFFSET	(21)
#define AXC003_BUTTON_SW2507_OFFSET	(23)
/** @} end of name */

#ifdef __cplusplus
extern "C" {
#endif

extern void axc003_gpio_init(void);
extern void leda_write(uint32_t led_val, uint32_t mask);
extern void ledb_write(uint32_t led_val, uint32_t mask);
extern void seven_seg_write(uint32_t val);
extern uint32_t button_read(uint32_t mask);
extern void button_install_isr(uint32_t offset, void (*isr_ptr)(void *ptr));

#ifdef __cplusplus
}
#endif

#endif /* _AXC003_GPIO_H_ */
