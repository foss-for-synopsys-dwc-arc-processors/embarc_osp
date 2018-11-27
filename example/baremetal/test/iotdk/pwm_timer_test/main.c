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


DEV_GPIO_PTR gpio_4b2;

void pwmtimer0_int_handler(void *ptr)
{
	static uint8_t tmp0 =0;
	tmp0 = !tmp0;
	//EMBARC_PRINTF("this pwm timer ch0 int handler\r\n");
	//gpio_4b2->gpio_write(tmp0, 1);
}
void pwmtimer1_int_handler(void *ptr)
{
	static uint8_t tmp1 =0;
	tmp1 = !tmp1;
	//EMBARC_PRINTF("this pwm timer ch1 int handler\r\n");
	gpio_4b2->gpio_write(tmp1, 1);
}
void pwmtimer2_int_handler(void *ptr)
{
	static uint8_t tmp2 =0;
	tmp2 = !tmp2;
	//EMBARC_PRINTF("this pwm timer ch2 int handler\r\n");
	//gpio_4b2->gpio_write(tmp2, 1);
}
void pwmtimer3_int_handler(void *ptr)
{
	static uint8_t tmp3 =0;
	tmp3 = !tmp3;
	//EMBARC_PRINTF("this pwm timer ch3 int handler\r\n");
	//gpio_4b2->gpio_write(tmp3, 1);
}
void pwmtimer4_int_handler(void *ptr)
{
	static uint8_t tmp4 =0;
	tmp4 = !tmp4;
	//EMBARC_PRINTF("this pwm timer ch4 int handler\r\n");
	//gpio_4b2->gpio_write(tmp4, 1);
}
void pwmtimer5_int_handler(void *ptr)
{
	static uint8_t tmp5 =0;
	tmp5 = !tmp5;
	//EMBARC_PRINTF("this pwm timer ch5 int handler\r\n");
	//gpio_4b2->gpio_write(tmp5, 1);
}



int main(void)
{
	io_arduino_config(ARDUINO_PIN_3, ARDUINO_PWM, IO_PINMUX_ENABLE);//pwm timer ch0
	io_arduino_config(ARDUINO_PIN_5, ARDUINO_PWM, IO_PINMUX_ENABLE);//pwm timer ch1
	io_arduino_config(ARDUINO_PIN_6, ARDUINO_PWM, IO_PINMUX_ENABLE);//pwm timer ch2
	io_arduino_config(ARDUINO_PIN_9, ARDUINO_PWM, IO_PINMUX_ENABLE);//pwm timer ch3
	io_arduino_config(ARDUINO_PIN_10, ARDUINO_PWM, IO_PINMUX_ENABLE);//pwm timer ch4
	io_arduino_config(ARDUINO_PIN_11, ARDUINO_PWM, IO_PINMUX_ENABLE);//pwm timer ch5
	io_arduino_config(ARDUINO_PIN_0, ARDUINO_GPIO, IO_PINMUX_ENABLE);

	gpio_4b2 = gpio_get_dev(DFSS_GPIO_4B2_ID);
	gpio_4b2->gpio_open(1);
	gpio_4b2->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, (void *)(1));


	DEV_PWM_TIMER_CFG ch0_pwm_cfg;
	ch0_pwm_cfg.mode = DEV_PWM_TIMER_MODE_PWM;
	ch0_pwm_cfg.count_high = 10000;
	ch0_pwm_cfg.count_low = 40000;
	ch0_pwm_cfg.isr_hander = NULL;

	DEV_PWM_TIMER_CFG ch1_pwm_cfg;
	ch1_pwm_cfg.mode = DEV_PWM_TIMER_MODE_TIMER;
	ch1_pwm_cfg.count_low = 10000;
	ch1_pwm_cfg.isr_hander = pwmtimer1_int_handler;

	DEV_PWM_TIMER_CFG ch2_pwm_cfg;
	ch2_pwm_cfg.mode = DEV_PWM_TIMER_MODE_TIMER;
	ch2_pwm_cfg.count_low = 20000;
	ch2_pwm_cfg.isr_hander = pwmtimer2_int_handler;

	DEV_PWM_TIMER_CFG ch3_pwm_cfg;
	ch3_pwm_cfg.mode = DEV_PWM_TIMER_MODE_TIMER;
	ch3_pwm_cfg.count_low = 30000;
	ch3_pwm_cfg.isr_hander = pwmtimer3_int_handler;

	DEV_PWM_TIMER_CFG ch4_pwm_cfg;
	ch4_pwm_cfg.mode = DEV_PWM_TIMER_MODE_TIMER;
	ch4_pwm_cfg.count_low = 40000;
	ch4_pwm_cfg.isr_hander = pwmtimer4_int_handler;

	DEV_PWM_TIMER_CFG ch5_pwm_cfg;
	ch5_pwm_cfg.mode = DEV_PWM_TIMER_MODE_TIMER;
	ch5_pwm_cfg.count_low = 50000;
	ch5_pwm_cfg.isr_hander = pwmtimer5_int_handler;

	DEV_PWM_TIMER_PTR pwm_timer_test = pwm_timer_get_dev(DW_PWM_TIMER_0_ID);
	pwm_timer_test->pwm_timer_open();

	pwm_timer_test->pwm_timer_write(0, DEV_PWM_TIMER_MODE_PWM, 100000, 80);


	pwm_timer_test->pwm_timer_write(1, DEV_PWM_TIMER_MODE_TIMER, 100000, 0);
	pwm_timer_test->pwm_timer_control(1, PWM_TIMER_CMD_SET_ISR, (void *)(pwmtimer1_int_handler));
	pwm_timer_test->pwm_timer_control(1, PWM_TIMER_CMD_ENA_ISR, (void *)(0));

	pwm_timer_test->pwm_timer_control(2, PWM_TIMER_CMD_SET_CFG, (void *)(&ch2_pwm_cfg));
	pwm_timer_test->pwm_timer_control(3, PWM_TIMER_CMD_SET_CFG, (void *)(&ch3_pwm_cfg));
	pwm_timer_test->pwm_timer_control(4, PWM_TIMER_CMD_SET_CFG, (void *)(&ch4_pwm_cfg));
	pwm_timer_test->pwm_timer_control(5, PWM_TIMER_CMD_SET_CFG, (void *)(&ch5_pwm_cfg));

	DEV_PWM_TIMER_CFG ch_pwm_cfg;
	pwm_timer_test->pwm_timer_control(0, PWM_TIMER_CMD_GET_CFG, (void *)(&ch_pwm_cfg));
	EMBARC_PRINTF("pwmtimer1_int_handler addr = %x\r\n", (uint32_t)(pwmtimer1_int_handler));
	EMBARC_PRINTF("cfg: ch = %d; mode = %d; count_low = %d, count_high = %d, isr_hander_addr = %x\r\n",
		0, ch_pwm_cfg.mode, ch_pwm_cfg.count_low, ch_pwm_cfg.count_high, (uint32_t)(ch_pwm_cfg.isr_hander));
	while (1) {
		board_delay_ms(500, 1);
		pwm_timer_test->pwm_timer_control(0, PWM_TIMER_CMD_DIS_CH, (void *)(0));
		board_delay_ms(500, 1);
		pwm_timer_test->pwm_timer_control(0, PWM_TIMER_CMD_ENA_CH, (void *)(0));
	}
	return E_SYS;
}