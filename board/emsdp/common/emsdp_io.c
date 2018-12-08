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
#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "emsdp_io.h"
#include "emsdp/emsdp.h"

static DEV_GPIO *led_port;
static DEV_GPIO *switch_port;

#define EMSDP_GPIO_CHECK_EXP_NORTN(EXPR)		CHECK_EXP_NOERCD(EXPR, error_exit)

/** emsdp on board gpio init */
void emsdp_io_init(void)
{
	emsdp_led_init();
	emsdp_switch_init();
}

/** emsdp on-board led init, led default off */
void emsdp_led_init(void)
{
	led_port = gpio_get_dev(EMSDP_LED_PORT);

	EMSDP_GPIO_CHECK_EXP_NORTN(led_port != NULL);

	if (led_port->gpio_open(EMSDP_LED_DIR) == E_OPNED) {
		led_port->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, (void *)(EMSDP_LED_MASK));
		led_port->gpio_control(GPIO_CMD_DIS_BIT_INT, (void *)(EMSDP_LED_MASK));
	}

	led_write(0, BOARD_LED_MASK);

error_exit:
	return;
}

/** emsdp on-board switch init */
void emsdp_switch_init(void)
{
	switch_port = gpio_get_dev(EMSDP_SWITCH_PORT);

	EMSDP_GPIO_CHECK_EXP_NORTN(switch_port != NULL);

	if (switch_port->gpio_open(EMSDP_SWITCH_DIR) == E_OPNED) {
		switch_port->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT, (void *)(EMSDP_SWITCH_MASK));
		switch_port->gpio_control(GPIO_CMD_DIS_BIT_INT, (void *)(EMSDP_SWITCH_MASK));
	}

error_exit:
	return;
}

/** write 1 to light on led bit, else light off led */
void led_write(uint32_t led_val, uint32_t mask)
{
	EMSDP_GPIO_CHECK_EXP_NORTN(led_port != NULL);

	led_val = (~led_val) & mask;
	led_val = led_val << EMSDP_LED_OFFSET;
	mask = (mask << EMSDP_LED_OFFSET) & EMSDP_LED_MASK;
	led_port->gpio_write(led_val, mask);

error_exit:
	return;
}

/** toggle masked led */
void led_toggle(uint32_t mask)
{
	EMSDP_GPIO_CHECK_EXP_NORTN(led_port != NULL);
	led_port->gpio_control(GPIO_CMD_TOGGLE_BITS, CONV2VOID(mask));

error_exit:
	return;
}

/** read led value, on for 1, off for 0 */
uint32_t led_read(uint32_t mask)
{
	uint32_t value;

	EMSDP_GPIO_CHECK_EXP_NORTN(led_port != NULL);

	mask = (mask << EMSDP_LED_OFFSET) & EMSDP_LED_MASK;
	led_port->gpio_read(&value, mask);
	value = (~value) & EMSDP_LED_MASK;

	return (value >> EMSDP_LED_OFFSET);

error_exit:
	return 0;
}

/** turn on switch return 1, else 0 */
uint32_t switch_read(uint32_t mask)
{
	uint32_t value;

	EMSDP_GPIO_CHECK_EXP_NORTN(switch_port != NULL);

	mask = (mask << EMSDP_SWITCH_OFFSET) & EMSDP_SWITCH_MASK;
	switch_port->gpio_read(&value, mask);
	value = (~value) & mask;

	return (value >> EMSDP_SWITCH_OFFSET);

error_exit:
	return 0;
}
