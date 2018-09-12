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

#include "embARC.h"
#include "embARC_debug.h"

static void smic_adc_int_handler(void *ptr);
static void smic_adc_ch4_callback(void *ptr);

ADC_DEFINE(adc_test, ADC_INT_NUM, ADC_CRTL_BASE, smic_adc_int_handler);


static void smic_adc_int_handler(void *ptr)
{
	smic_adc_int_isr(adc_test, ptr);
}

static void smic_adc_ch4_callback(void *ptr)
{
	EMBARC_PRINTF("\x1b[2k\x1b\x45");//\x1b[18;1H
	EMBARC_PRINTF("this is adc ch4 callback, ad value = %d\r\n", (uint32_t)ptr);
}


int main(void)
{
	uint16_t adc_value[16];
	SMIC_ADC_CALLBACK_BIT ch4_callback;
	ch4_callback.channel = 4;
	ch4_callback.isr = smic_adc_ch4_callback;
	arduino_pin_mux(ARDUINO_A0, ARDUINO_FUNC_2ND);
	arduino_pin_mux(ARDUINO_A1, ARDUINO_FUNC_2ND);
	arduino_pin_mux(ARDUINO_A2, ARDUINO_FUNC_2ND);
	arduino_pin_mux(ARDUINO_A3, ARDUINO_FUNC_2ND);
	arduino_pin_mux(ARDUINO_A4, ARDUINO_FUNC_3RD);
	arduino_pin_mux(ARDUINO_A5, ARDUINO_FUNC_3RD);
	smic_adc_open(adc_test);
	smic_adc_control(adc_test, SMIC_ADC_SET_CALLBACK, (void *)(&ch4_callback));
	adc_test->adc_callback[4] = smic_adc_ch4_callback;
	EMBARC_PRINTF("\r\n\r\n");
	while (1) {
		//adc_value = ADC_get(15);
		for(uint8_t i = 0; i < 8; i++) {
			smic_adc_read_int(adc_test, i, &(adc_value[i]));
		}
		for(uint8_t i = 8; i < 16; i++) {
			smic_adc_read_polling(adc_test, i, &(adc_value[i]));
		}
		EMBARC_PRINTF("0: %4d, 1: %4d, 2: %4d, 3: %4d\r\n", adc_value[0], adc_value[1], adc_value[2], adc_value[3]);
		EMBARC_PRINTF("4: %4d, 5: %4d, 6: %4d, 7: %4d\r\n", adc_value[4], adc_value[5], adc_value[6], adc_value[7]);
		EMBARC_PRINTF("8: %4d, 9: %4d, A: %4d, B: %4d\r\n", adc_value[8], adc_value[9], adc_value[10], adc_value[11]);
		EMBARC_PRINTF("C: %4d, D: %4d, E: %4d, F: %4d\r\n", adc_value[12], adc_value[13], adc_value[14], adc_value[15]);
		board_delay_ms(100, 1);
		EMBARC_PRINTF("\x1b[2k\x1b\x45");//\x1b[18;1H
		EMBARC_PRINTF("\x1b[2k\x1b\x45");//\x1b[18;1H
		EMBARC_PRINTF("\x1b[2k\x1b\x45");//\x1b[18;1H
		EMBARC_PRINTF("\x1b[2k\x1b\x45");//\x1b[18;1H
	}

	return E_SYS;
}