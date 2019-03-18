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

#ifndef _ADT7420_H_
#define _ADT7420_H_

#include "dev_iic.h"

/**
 * I2C address of PmodTmp2 (ADT7420) can be selected via jumpers: 0x48, 0x49, 0x4A, 0x4B
 * Default (all jumpers removed) is 0x4B
 */
#define ADT7420_A0_PIN		1	/*!< I2C Serial Bus Address Selection Pin */
#define ADT7420_A1_PIN		1	/*!< I2C Serial Bus Address Selection Pin */
#define ADT7420_IIC_ADDRESS	(0x48 + (ADT7420_A1_PIN << 1) + ADT7420_A0_PIN)

#define TEMP_I2C_SLAVE_ADDRESS	ADT7420_IIC_ADDRESS

#ifdef __cplusplus
extern "C" {
#endif

/* options for mode parameter of temp_sensor_mode() */
typedef enum {
	ADT7420_OP_MODE_CONT_CONV	= 0,
	ADT7420_OP_MODE_ONE_SHOT	= 1,
	ADT7420_OP_MODE_1_SPS		= 2,
	ADT7420_OP_MODE_SHUTDOWN	= 3
} ADT7420_OP_MODE;

/* options for resolution parameter of temp_sensor_resolution() */
typedef enum {
	ADT7420_RESOLUTION_13BIT,
	ADT7420_RESOLUTION_16BIT
} ADT7420_RESOLUTION;

/* temperature sensor object type*/
typedef struct {
	uint32_t i2c_id;
	uint32_t slvaddr;

	int32_t  resolution;
	int32_t  op_mode;
} ADT7420_DEF, *ADT7420_DEF_PTR;

#define ADT7420_DEFINE(NAME, I2C_ID, SLAVE_ADDRESS) \
	ADT7420_DEF __ ## NAME = { \
			.i2c_id = I2C_ID, \
			.slvaddr = SLAVE_ADDRESS, \
	}; \
	ADT7420_DEF_PTR NAME = &__ ## NAME

extern int32_t adt7420_sensor_init(ADT7420_DEF_PTR obj);
extern int32_t adt7420_sensor_deinit(ADT7420_DEF_PTR obj);
extern int32_t adt7420_sensor_mode(ADT7420_DEF_PTR obj, int32_t mode);
extern int32_t adt7420_sensor_resolution(ADT7420_DEF_PTR obj, int32_t resolution);
extern int32_t adt7420_sensor_read(ADT7420_DEF_PTR obj, float *val);

#ifdef __cplusplus
}
#endif

#endif /* _ADT7420_H_ */