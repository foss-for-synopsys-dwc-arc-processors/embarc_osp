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

#ifndef _AD7991_H_
#define _AD7991_H_

#include "dev_iic.h"

/**
 * PMOD AD2 board (AD7991) has to choose its voltage reference form power supply, or external on board 2.048V Voltage Reference (ADR380) or VIN4 (Ch3)
 * please note that when choose external reference, only 3 channels are converting
 */
#define AD2_JUMPER_REF		0
#define AD2_JUMPER_VIN4		1
/* switch for AD2 board Jumper, choose from AD2_JUMPER_REF or AD2_JUMPER_VIN4*/
#define AD2_REF_JUMPER		AD2_JUMPER_REF

#define AD2_REF_VDD 		3300 	/*< power supply voltage is 3300mV*/
#if AD2_REF_JUMPER == AD2_JUMPER_REF
#define AD2_REF_EXTERNAL 	2048 	/*< The ADR380 voltage reference on AD2 board has 2048mV output*/
#else
#define AD2_REF_EXTERNAL 	1000 	/*< choose VIN4 (CH3) as reference, use 1000 to return a fraction voltage value of CH3 */
#endif

#define AD7991_CONFIG_CH0_EN		(1 << 4)
#define AD7991_CONFIG_CH1_EN		(1 << 5)
#define AD7991_CONFIG_CH2_EN		(1 << 6)
#define AD7991_CONFIG_CH3_EN		(1 << 7)
/**
 * I2C address : Pmod AD2 (AD7991) uses AD7991-0, has only one address which is 0x28
 */
#define AD7991_IIC_ADDRESS	0x28

#define ADC_I2C_SLAVE_ADDRESS	AD7991_IIC_ADDRESS

#ifdef __cplusplus
extern "C" {
#endif

/** options for reference source parameter of ad7991_ref_select() */
typedef enum ad7991_ref_source {
	AD7991_REF_VDD,
	AD7991_REF_EXTERNAL
} AD7991_REF_SOURCE;

/** AD7991 object type */
typedef struct {
	uint32_t dev_id;
	uint32_t slvaddr;

	int8_t  channel_num;
	int8_t  reg_config;
	int32_t  ref_source;
	int32_t  ref_voltage_mv;
} AD7991_DEF, *AD7991_DEF_PTR;

#define AD7991_DEFINE(NAME, DEVICE_ID, SLAVE_ADDRESS) \
	AD7991_DEF __ ## NAME = { \
			.dev_id = DEVICE_ID, \
			.slvaddr = SLAVE_ADDRESS, \
	}; \
	AD7991_DEF_PTR NAME = &__ ## NAME

extern int32_t ad7991_adc_init(AD7991_DEF_PTR obj);
extern int32_t ad7991_adc_deinit(AD7991_DEF_PTR obj);
extern int32_t ad7991_ref_select(AD7991_DEF_PTR obj, int32_t source);
extern int32_t ad7991_adc_channel(AD7991_DEF_PTR obj, int8_t channel);
extern int32_t ad7991_adc_read(AD7991_DEF_PTR obj, float *val);

#ifdef __cplusplus
}
#endif

#endif /* _AD7991_H_ */
