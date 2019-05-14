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
#ifndef _SMIC_ADC_H_
#define _SMIC_ADC_H_

#include "arc_exception.h"

#define ADC_CRTL_BASE		0xF0007000
#define ADC_CHANNEL_MAX		16
#define ADC_INT_NUM 		97

#define SMIC_ADC_SET_MODE	0
#define SMIC_ADC_GET_MODE	1
#define SMIC_ADC_SET_CALLBACK	2
#define SMIC_ADC_GET_CALLBACK	3

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct smic_adc_reg {
	//ADC control register
	uint32_t ADCCON;//(0x00)
	//ADC scan control register
	uint32_t ADCSCAN;//(0x04)
	//ADC channel 0-7 data register
	uint32_t ADCD0_7[8];//(0x08)
	//ADC interrupt enable register
	uint32_t ADCIMSC;//(0x28)
	//ADC raw interrput status register
	uint32_t ADCRIS;//(0x2C)
	//ADC mask interrupt status register
	uint32_t ADCMIS;//(0x30)
	//ADC interrupt flag clear register
	uint32_t ADCICLR;//(0x34)
	//RESERVED
	uint32_t RESERVED[2];//(0x38 0x3C)
	//ADC channel 8 data register
	uint32_t ADCD8_15[8];//(0x40)
} SMIC_ADC_REG, *SMIC_ADC_REG_PTR;

typedef enum {
	ADC_MODE_SINGLE	= 0,
	ADC_MODE_CONTINUOUS = 1,
} SMIC_ADC_MODE;


typedef void (*SMIC_ADC_CALLBACK)(void *ptr);

typedef struct {
	uint8_t channel;
	SMIC_ADC_CALLBACK isr;
} SMIC_ADC_CALLBACK_BIT;

typedef volatile struct {
	SMIC_ADC_REG_PTR adc_reg;
	SMIC_ADC_MODE adc_mode;
	uint8_t adc_channel_max;
	uint8_t adc_open_cnt;
	uint8_t adc_intno;
	uint16_t adc_ready_flag;
	uint16_t adc_data[ADC_CHANNEL_MAX];
	SMIC_ADC_CALLBACK adc_callback[ADC_CHANNEL_MAX];
	INT_HANDLER adc_handle;
} SMIC_ADC_DEF, *SMIC_ADC_DEF_PTR;


#define ADC_DEFINE(NAME, INT_NUM, REG_BASE, HANDLE) \
	SMIC_ADC_DEF __ ## NAME = { \
		.adc_mode = ADC_MODE_SINGLE, \
		.adc_channel_max = ADC_CHANNEL_MAX, \
		.adc_open_cnt = 0, \
		.adc_intno = INT_NUM, \
		.adc_handle = HANDLE, \
		.adc_reg = (SMIC_ADC_REG_PTR)REG_BASE, \
	}; \
	SMIC_ADC_DEF_PTR NAME = &__ ## NAME

extern int32_t smic_adc_open(SMIC_ADC_DEF_PTR obj);
extern int32_t smic_adc_close(SMIC_ADC_DEF_PTR obj);
extern int32_t smic_adc_read_polling(SMIC_ADC_DEF_PTR obj, uint8_t channel, uint16_t *val);
extern int32_t smic_adc_read_int(SMIC_ADC_DEF_PTR obj, uint8_t channel, uint16_t *val);
extern int32_t smic_adc_control(SMIC_ADC_DEF_PTR obj, uint32_t ctrl_cmd, void *param);
extern int32_t smic_adc_int_isr(SMIC_ADC_DEF_PTR obj, void *ptr);
#ifdef __cplusplus
}
#endif

#endif /* _SMIC_ADC_H_ */
