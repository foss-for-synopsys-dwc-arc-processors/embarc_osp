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
#include "arc.h"
#include "arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "dw_gpio.h"
#include "axc003_gpio.h"
#include "axs_hardware.h"


static DEV_GPIO *leda_port;
static DEV_GPIO *ledb_port;
static DEV_GPIO *button_port;
static DEV_GPIO *seven_seg_port;

#define AXC003_GPIO_CHECK_EXP_NORTN(EXPR)		CHECK_EXP_NOERCD(EXPR, error_exit)

/** axc003 on board gpio init */
void axc003_gpio_init(void)
{
	DEV_GPIO * port_ptr;

	port_ptr = gpio_get_dev(CPU_DW_GPIO_PORT_A);
	port_ptr->gpio_open(CPU_POARTA_DIR);

	port_ptr = gpio_get_dev(CPU_DW_GPIO_PORT_B);
	port_ptr->gpio_open(CPU_PORTB_DIR);

	leda_port = gpio_get_dev(AXC003_LEDA_PORT);
	ledb_port = gpio_get_dev(AXC003_LEDB_PORT);
	button_port = gpio_get_dev(AXC003_BUTTON_PORT);
	seven_seg_port = gpio_get_dev(AXC003_7SEG_PORT);

	seven_seg_write(10);
	leda_write(0xAA, 0xAA);
	ledb_write(0x55, 0x55);
}

/** write 1 to light on led bit, else light off led */
void leda_write(uint32_t led_val, uint32_t mask)
{
	AXC003_GPIO_CHECK_EXP_NORTN(leda_port != NULL);

 	led_val &= mask;
	led_val = (led_val & 0x3) | ((led_val & 0xC) << 0x3) | ((led_val & 0x30) << 0x6) |
		((led_val & 0xC0) << 9);

	mask = (mask & 0x3) | ((mask & 0xC) << 0x3) | ((mask & 0x30) << 0x6) |
		((mask & 0xC0) << 9);

	leda_port->gpio_write(led_val, mask);

error_exit:
	return;
}



/** write 1 to light on led bit, else light off led */
void ledb_write(uint32_t led_val, uint32_t mask)
{
	AXC003_GPIO_CHECK_EXP_NORTN(ledb_port != NULL);

	led_val &= mask;
	led_val = led_val << AXC003_LEDB_OFFSET;
	mask = (mask << AXC003_LEDB_OFFSET) & AXC003_LEDB_MASK;
	ledb_port->gpio_write(led_val, mask);

error_exit:
	return;
}

/** Press down bit set to 1 else 0 */
uint32_t button_read(uint32_t mask)
{
	uint32_t value;

	AXC003_GPIO_CHECK_EXP_NORTN(button_port != NULL);

	mask = (mask << AXC003_BUTTON_OFFSET) & AXC003_BUTTON_MASK;
	button_port->gpio_read(&value, mask);
	value = (~value) & mask;

	return (value >> AXC003_BUTTON_OFFSET);

error_exit:
	return 0;
}

void button_install_isr(uint32_t offset, void (*isr_ptr)(void *ptr))
{

/* note: CPU card's creg ctrl's gpio int source to INTNO_ICTL_MB,
default is A12 which is int request from main board, int request from SW2504-SW2507 can not
go to INTNO_ICTL_MB */
	DEV_GPIO_BIT_ISR bit_isr;
	DEV_GPIO_INT_CFG int_cfg;
	uint32_t mask;

	if (offset < AXC003_BUTTON_SW2504_OFFSET || offset > AXC003_BUTTON_SW2507_OFFSET) {
		return;
	}
	AXC003_GPIO_CHECK_EXP_NORTN(button_port != NULL);

	mask = (0x1 << offset);

	int_cfg.int_bit_mask = mask;
	int_cfg.int_bit_type = GPIO_INT_BITS_EDGE_TRIG(mask);
	int_cfg.int_bit_polarity = GPIO_INT_BITS_POL_FALL_EDGE(mask);
	int_cfg.int_bit_debounce = GPIO_INT_BITS_ENA_DEBOUNCE(mask);
	button_port->gpio_control(GPIO_CMD_SET_BIT_INT_CFG, (void *)(&int_cfg));
	bit_isr.int_bit_ofs = offset;
	bit_isr.int_bit_handler = isr_ptr;
	button_port->gpio_control(GPIO_CMD_SET_BIT_ISR, (void *)(&bit_isr));
	button_port->gpio_control(GPIO_CMD_ENA_BIT_INT, (void *)mask);

error_exit:
	return;
}

static const uint8_t num_to_7seg_table[10] = {
	0x3f, // 0
	0x06, // 1
	0x5B, // 2
	0x4F, // 3
	0x66, // 4
	0x6D, // 5
	0x7D, // 6
	0x07, // 7
	0x7F, // 8
	0x6F // 9
};

void seven_seg_write(uint32_t val)
{
	uint32_t digit;

	AXC003_GPIO_CHECK_EXP_NORTN(seven_seg_port != NULL);

	if (val > 99) {
		return; // only can show  val < 99, two 7-seg display.
	}

	digit = num_to_7seg_table[val / 10] |  (num_to_7seg_table[val % 10] << 8);

	seven_seg_port->gpio_write((digit << AXC003_7SEG_OFFSET), AXC003_7SEG_MASK);

error_exit:
	return;
}
