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

#include "arc.h"
#include "arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC_debug.h"

#include "smic_adc.h"

#define SMIC_ADC_CHECK_EXP_NORTN(EXPR)		CHECK_EXP_NOERCD(EXPR, error_exit)
#define SMIC_ADC_CHECK_EXP(EXPR, ERROR_CODE)	CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

int32_t smic_adc_open(SMIC_ADC_DEF_PTR obj)
{
	int32_t ercd = E_OK;

	SMIC_ADC_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_ADC_CHECK_EXP(obj->adc_open_cnt == 0, E_OPNED);

	obj->adc_open_cnt++;
	obj->adc_reg->ADCCON = 0x16;
	//enable ADC;
	//single mode;
	//Fadc = 144/2^6 = 2.25Mhz, Sampling Rate = Fadc/16 = 140625Hz
	int_handler_install(obj->adc_intno, obj->adc_handle);
	int_disable(obj->adc_intno);

error_exit:
	return ercd;
}

int32_t smic_adc_close(SMIC_ADC_DEF_PTR obj)
{
	int32_t ercd = E_OK;

	SMIC_ADC_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_ADC_CHECK_EXP(obj->adc_open_cnt != 0, E_CLSED);

	obj->adc_reg->ADCCON = 0x00;
	obj->adc_reg->ADCIMSC = 0x00;
	obj->adc_reg->ADCSCAN = 0x00;

	for (uint8_t i = 0; i < obj->adc_channel_max; i++) {
		obj->adc_callback[i] = NULL;
	}

	obj->adc_open_cnt = 0;

error_exit:
	return ercd;
}

int32_t smic_adc_read_polling(SMIC_ADC_DEF_PTR obj, uint8_t channel, uint16_t *val)
{
	int32_t ercd = E_OK;
	uint16_t adc_value;

	SMIC_ADC_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_ADC_CHECK_EXP(obj->adc_open_cnt != 0, E_CLSED);
	SMIC_ADC_CHECK_EXP(channel >=0 && channel < obj->adc_channel_max, E_PAR);

	obj->adc_reg->ADCIMSC |= 1 << channel; //enable interrupt
	obj->adc_reg->ADCSCAN |= (0x10000 | (1 << channel));//adc channel enable, adc start

	while (!(obj->adc_reg->ADCRIS& (1 << channel)));//wait for ADC conversion end

	if (channel < 8) {
		adc_value = obj->adc_reg->ADCD0_7[channel];
	} else {
		adc_value = obj->adc_reg->ADCD8_15[channel-8];
	}

	obj->adc_reg->ADCICLR |= (1 << channel);//clear interrupt flag

	obj->adc_reg->ADCIMSC &= ~(uint32_t)(1 << channel); //disable interrupt
	obj->adc_reg->ADCSCAN &= ~(uint32_t)(1 << channel);//adc channel disable
	*val = adc_value & 0xFFF;

error_exit:
	return ercd;
}

int32_t smic_adc_read_int(SMIC_ADC_DEF_PTR obj, uint8_t channel, uint16_t *val)
{
	int32_t ercd = E_OK;
	uint16_t adc_value;

	SMIC_ADC_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_ADC_CHECK_EXP(obj->adc_open_cnt != 0, E_CLSED);
	SMIC_ADC_CHECK_EXP(channel >=0 && channel < obj->adc_channel_max, E_PAR);

	obj->adc_ready_flag &= ~((uint32_t)(1 << channel));

	int_enable(obj->adc_intno);

	obj->adc_reg->ADCIMSC |= 1 << channel; //enable interrupt
	obj->adc_reg->ADCSCAN |= (0x10000 | (1 << channel));//adc channel enable, adc start

	while (!(obj->adc_ready_flag & (1 << channel)));

	adc_value = obj->adc_data[channel];

	int_disable(obj->adc_intno);

	obj->adc_ready_flag &= ~((uint32_t)(1 << channel));
	obj->adc_reg->ADCIMSC &= ~(uint32_t)(1 << channel); //disable interrupt
	obj->adc_reg->ADCSCAN &= ~(uint32_t)(1 << channel);//adc channel disable
	*val = adc_value & 0xFFF;

error_exit:
	return ercd;
}

int32_t smic_adc_control(SMIC_ADC_DEF_PTR obj, uint32_t ctrl_cmd, void *param)
{
	int32_t ercd = E_OK;

	SMIC_ADC_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_ADC_CHECK_EXP(obj->adc_open_cnt != 0, E_CLSED);
	SMIC_ADC_CHECK_EXP(param != NULL, E_PAR);

	if (ctrl_cmd != SMIC_ADC_SET_MODE && ctrl_cmd != SMIC_ADC_GET_MODE &&
	    ctrl_cmd != SMIC_ADC_SET_CALLBACK && ctrl_cmd != SMIC_ADC_GET_CALLBACK) {
		ercd = E_PAR;
		goto error_exit;
	}

	switch (ctrl_cmd) {
		case SMIC_ADC_SET_MODE:
			SMIC_ADC_CHECK_EXP(*((SMIC_ADC_MODE *)param) != ADC_MODE_CONTINUOUS && \
			                   *((SMIC_ADC_MODE *)param) != ADC_MODE_SINGLE, E_PAR);
			obj->adc_reg->ADCCON |= (*((SMIC_ADC_MODE *)param) << 3);
			obj->adc_mode = *((SMIC_ADC_MODE *)param);
			break;

		case SMIC_ADC_GET_MODE:
			obj->adc_mode = (SMIC_ADC_MODE)((obj->adc_reg->ADCCON & (1 << 3)) >> 3);
			*((SMIC_ADC_MODE *)param) = obj->adc_mode;
			break;

		case SMIC_ADC_SET_CALLBACK:
			SMIC_ADC_CHECK_EXP(((SMIC_ADC_CALLBACK_BIT *)param)->channel >=0 && \
			((SMIC_ADC_CALLBACK_BIT *)param)->channel < obj->adc_channel_max, E_PAR);
			obj->adc_callback[((SMIC_ADC_CALLBACK_BIT *)param)->channel] \
			= ((SMIC_ADC_CALLBACK_BIT *)param)->isr;
			break;

		case SMIC_ADC_GET_CALLBACK:
			SMIC_ADC_CHECK_EXP(((SMIC_ADC_CALLBACK_BIT *)param)->channel >=0 && \
			((SMIC_ADC_CALLBACK_BIT *)param)->channel < obj->adc_channel_max, E_PAR);
			((SMIC_ADC_CALLBACK_BIT *)param)->isr = \
			obj->adc_callback[((SMIC_ADC_CALLBACK_BIT *)param)->channel];
			break;

		default:
			break;
	}


error_exit:
	return ercd;
}

int32_t smic_adc_int_isr(SMIC_ADC_DEF_PTR obj, void *ptr)
{
	int32_t ercd = E_OK;
	SMIC_ADC_CHECK_EXP(obj != NULL, E_OBJ);

	for (uint8_t i = 0; i < obj->adc_channel_max; i++) {
		if (obj->adc_reg->ADCMIS & (1 << i)) {

			if (i < 8) {
				obj->adc_data[i] = obj->adc_reg->ADCD0_7[i];
			} else {
				obj->adc_data[i] = obj->adc_reg->ADCD8_15[i-8];
			}

			obj->adc_ready_flag |= (1 << i);
			obj->adc_reg->ADCICLR |= (1 << i);//clear interrupt flag

			if (obj->adc_callback[i] != NULL) {
				obj->adc_callback[i]((void *) (uint32_t)obj->adc_data[i]);
			}
		}
	}

error_exit:
	return ercd;
}
