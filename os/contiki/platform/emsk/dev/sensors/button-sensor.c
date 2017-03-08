/*
 * Copyright (c) 2014, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
/*
 * Modified for port to ARC processor
 * by Wayne Ren wei.ren@synopsys.com, Synopsys, Inc.
 */

/**
 * \file button-sensor.c
 * \brief button sensor driver for contiki
 */
#include "contiki.h"
#include "lib/sensors.h"
#include "dev_common.h"
#include "dev_gpio.h"
#include "arc_exception.h"
#include "board.h"
#include "button-sensor.h"

/* note: emsk_button_init(in emsk_gpio.c) must be called before
	i.e., button port must be opened and button bits are configured
	as input
*/

const struct sensors_sensor button_sensor1;
const struct sensors_sensor button_sensor2;
const struct sensors_sensor button_sensor3;

static struct timer debouncetimer1;
static struct timer debouncetimer2;
static struct timer debouncetimer3;

#define BUTTON_SENSOR1_OFS	0
#define BUTTON_SENSOR2_OFS	1
#define BUTTON_SENSOR3_OFS	2

#define BUTTON_SENSOR_ID	EMSK_GPIO_PORT_A

static void button_int_cfg(DEV_GPIO *port, uint32_t bit_ofs, DEV_GPIO_HANDLER handler)
{
	uint32_t mask = (1<<bit_ofs);
	DEV_GPIO_BIT_ISR bit_isr;
	DEV_GPIO_INT_CFG int_cfg;

	port->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT, (void *)(mask));
	int_cfg.int_bit_mask = mask;
	int_cfg.int_bit_type = GPIO_INT_BITS_EDGE_TRIG(mask);
	int_cfg.int_bit_polarity = GPIO_INT_BITS_POL_FALL_EDGE(mask);
	int_cfg.int_bit_debounce = GPIO_INT_BITS_ENA_DEBOUNCE(mask);
	port->gpio_control(GPIO_CMD_SET_BIT_INT_CFG, (void *)(&int_cfg));
	bit_isr.int_bit_ofs = bit_ofs;
	bit_isr.int_bit_handler = handler;
	port->gpio_control(GPIO_CMD_SET_BIT_ISR, (void *)(&bit_isr));
}

static void button1_isr(void *ptr)
{

	if(timer_expired(&debouncetimer1)) {
		timer_set(&debouncetimer1, CLOCK_SECOND / 4);
		sensors_changed(&button_sensor1);
	}

}

static int value1(int type)
{
	return button_read((1<<BUTTON_SENSOR1_OFS));
}

static int configure1(int type, int c)
{
	DEV_GPIO_PTR port;
	uint32_t mask;

	port = gpio_get_dev(BUTTON_SENSOR_ID);
	mask = (1<<BUTTON_SENSOR1_OFS);
	port->gpio_open(0x0);

	switch (type) {
		case SENSORS_ACTIVE:
			if (c) {
				timer_set(&debouncetimer1, 0);
				button_int_cfg(port, BUTTON_SENSOR1_OFS, button1_isr);
				port->gpio_control(GPIO_CMD_ENA_BIT_INT, (void *)mask);
			} else {
				port->gpio_control(GPIO_CMD_DIS_BIT_INT, (void *)mask);
			}
			return 1;
		default:
			break;
	}
	return 0;
}

static int status(int type)
{
	DEV_GPIO_PTR port;
	port = gpio_get_dev(BUTTON_SENSOR_ID);

	switch (type) {
		case SENSORS_ACTIVE:
		case SENSORS_READY:
			return (int_enabled(INTNO_GPIO) && (port->gpio_info.opn_cnt > 0));
  	}
	return 0;
}

SENSORS_SENSOR(button_sensor1, BUTTON_SENSOR1,
		value1, configure1, status);

static void button2_isr(void *ptr)
{

	if(timer_expired(&debouncetimer2)) {
		timer_set(&debouncetimer2, CLOCK_SECOND / 4);
		sensors_changed(&button_sensor2);
	}

}

static int value2(int type)
{
	return button_read((1<<BUTTON_SENSOR2_OFS));
}

static int configure2(int type, int c)
{
	DEV_GPIO_PTR port;
	uint32_t mask;

	port = gpio_get_dev(BUTTON_SENSOR_ID);
	mask = (1<<BUTTON_SENSOR2_OFS);
	port->gpio_open(0x0);

	switch (type) {
		case SENSORS_ACTIVE:
			if (c) {
				timer_set(&debouncetimer2, 0);
				button_int_cfg(port, BUTTON_SENSOR2_OFS, button2_isr);
				port->gpio_control(GPIO_CMD_ENA_BIT_INT, (void *)mask);
			} else {
				port->gpio_control(GPIO_CMD_DIS_BIT_INT, (void *)mask);
			}
			return 1;
		default:
			break;
	}
	return 0;
}

SENSORS_SENSOR(button_sensor2, BUTTON_SENSOR2,
		value2, configure2, status);

static void button3_isr(void *ptr)
{

	if(timer_expired(&debouncetimer3)) {
		timer_set(&debouncetimer3, CLOCK_SECOND / 4);
		sensors_changed(&button_sensor3);
	}

}

static int value3(int type)
{
	return button_read((1<<BUTTON_SENSOR3_OFS));
}

static int configure3(int type, int c)
{
	DEV_GPIO_PTR port;
	uint32_t mask;

	port = gpio_get_dev(DW_GPIO_PORT_A);
	mask = (1<<BUTTON_SENSOR3_OFS);
	port->gpio_open(0x0);

	switch (type) {
		case SENSORS_ACTIVE:
			if (c) {
				timer_set(&debouncetimer3, 0);
				button_int_cfg(port, BUTTON_SENSOR3_OFS, button3_isr);
				port->gpio_control(GPIO_CMD_ENA_BIT_INT, (void *)mask);
			} else {
				port->gpio_control(GPIO_CMD_DIS_BIT_INT, (void *)mask);
			}
			return 1;
		default:
			break;
	}
	return 0;
}

SENSORS_SENSOR(button_sensor3, BUTTON_SENSOR3,
		value3, configure3, status);