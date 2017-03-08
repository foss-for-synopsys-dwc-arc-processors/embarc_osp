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
 * \file temperature-sensor.c
 * \brief iic temperature sensor driver for contiki
 */
#include "contiki.h"
#include "temperature-sensor.h"
#include "dev_common.h"
#include "dev_iic.h"
#include "dw_iic.h"
#include <stdio.h>

// I2C address of PmodTmp2 can be selected via jumpers: 0x48, 0x49, 0x4A, 0x4B
// Default (all jumpers removed) is 0x4B
#define I2C_SLAVE_ADDRESS  0x4B

const struct sensors_sensor temperature_sensor;

static int value(int type)
{
	DEV_IIC_PTR temp_sensor;
	uint8_t data[2];
	int val = 0;

	temp_sensor = iic_get_dev(DW_IIC_0_ID);
	if (temp_sensor->iic_read(data, 2) == 2) {
		val = (data[0] << 8) + (data[1] & 0xF8);
		val  >>= 3;
	}
	//conversion (val = val*0.0625) in Degrees
	return val;
}

static int configure(int type, int c)
{
	uint32_t val;
	DEV_IIC_PTR temp_sensor;
	int32_t ercd = E_OBJ;

	temp_sensor = iic_get_dev(DW_IIC_0_ID);
	switch (type) {
		case SENSORS_ACTIVE:
			if(c) { /* open */
				val = I2C_SLAVE_ADDRESS;
				ercd = temp_sensor->iic_open(DEV_MASTER_MODE, IIC_SPEED_FAST);
				if (ercd == E_OK || ercd == E_OPNED) {
					ercd = temp_sensor->iic_control(IIC_CMD_MST_SET_TAR_ADDR, (void *)(val));
				}
			} else { /* close */
			 	ercd = temp_sensor->iic_close();
			}
			break;
		default:
			break;
	}

	if (ercd == E_OK) {
		return 1;
	}
	return 0;
}

static int status(int type)
{
	DEV_IIC_PTR temp_sensor;
	temp_sensor = iic_get_dev(DW_IIC_0_ID);

	switch (type) {
		case SENSORS_ACTIVE:
			if (temp_sensor->iic_info.opn_cnt > 0) {
				return 1;
			}
			break;
		case SENSORS_READY:
			if (temp_sensor->iic_info.status & DEV_ENABLED) {
				return 1;
			}
			break;
		default:
			break;
	}
	return 0;
}


SENSORS_SENSOR(temperature_sensor, TEMPERATURE_SENSOR,
		value, configure, status);
