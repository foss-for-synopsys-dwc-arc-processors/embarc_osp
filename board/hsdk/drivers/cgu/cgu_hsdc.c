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

#include <stdint.h>
#include <stdbool.h>
#include "arc_builtin.h"
#include "arc_exception.h"
#include "cgu_hsdc.h"
#include "embARC_debug.h"

typedef struct
{
	uint32_t pd;      //PowerDown
	uint32_t bypass;  //Bypass PLL
	uint32_t band;    //Band select
	uint32_t od;      //Output divider
	uint32_t r;       //Input divider
	uint32_t f;       //Feedback divider
} CGU_HSDC_PLL_CTRL_T;


const CGU_REGISTER_PLL_SETTING_T cgu_register_pll_setting_table[CGU_HSDC_PLL_FREQ_NONE] = {
// freq  	bs 	F 	R 	OD 	byp 	pd
	{33, 	0, 	0, 	0, 	0, 	1, 	0},
	{100, 	0, 	11, 	0, 	3, 	0, 	0},
	{133, 	0, 	31, 	1, 	3, 	0, 	0},
	{150, 	0, 	17, 	0, 	3, 	0, 	0},
	{167, 	0, 	39, 	1, 	3, 	0, 	0},
	{200, 	0, 	47, 	1, 	3, 	0, 	0},
	{233, 	0, 	27, 	1, 	2, 	0, 	0},
	{250, 	0, 	29, 	1, 	2, 	0, 	0},
	{267, 	0, 	31, 	1, 	2, 	0, 	0},
	{300, 	0, 	35, 	1, 	2, 	0, 	0},
	{333, 	0, 	39, 	1, 	2, 	0, 	0},
	{350, 	0, 	41, 	1, 	2, 	0, 	0},
	{367, 	0, 	43, 	1, 	2, 	0, 	0},
	{400, 	0, 	47, 	1, 	2, 	0, 	0},
	{433, 	0, 	12, 	0, 	1, 	0, 	0},
	{450, 	1, 	26, 	0, 	2, 	0, 	0},
	{467, 	0, 	13, 	0, 	1, 	0, 	0},
	{500, 	0, 	14, 	0, 	1, 	0, 	0},
	{533, 	0, 	15, 	0, 	1, 	0, 	0},
	{550, 	0, 	32, 	1, 	1, 	0, 	0},
	{567, 	0, 	33, 	1, 	1, 	0, 	0},
	{600, 	0, 	17, 	0, 	1, 	0, 	0},
	{633, 	0, 	37, 	1, 	1, 	0, 	0},
	{650, 	0, 	38, 	1, 	1, 	0, 	0},
	{667, 	0, 	39, 	1, 	1, 	0, 	0},
	{700, 	0, 	20, 	0, 	1, 	0, 	0},
	{733, 	0, 	43, 	1, 	1, 	0, 	0},
	{750, 	0, 	44, 	1, 	1, 	0, 	0},
	{767, 	0, 	45, 	1, 	1, 	0, 	0},
	{800, 	0, 	23, 	0, 	1, 	0, 	0},
	{833, 	0, 	24, 	1, 	0, 	0, 	0},
	{867, 	0, 	25, 	1, 	0, 	0, 	0},
	{900, 	0, 	26, 	1, 	0, 	0, 	0},
	{933, 	0, 	27, 	1, 	0, 	0, 	0},
	{967, 	0, 	28, 	1, 	0, 	0, 	0},
	{1000, 	0, 	29, 	1, 	0, 	0, 	0},
};

/*
* Polling method
*/
static int32_t cgu_wait_for_lock(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_PLL_T pll)
{
#define TIMEOUT         (32*1024)
	uint32_t cnt;
	volatile uint32_t *ptr;

	//Point to first element of reg-struct
	ptr  = (volatile uint32_t *)&cgu_regs->CGU_ARC_PLL_STATUS;
	ptr += 4 * pll;

	//Wait for unlock
	cnt = TIMEOUT;

	while (cnt > 0) {
		if ( (*ptr & CGU_HSDC_STS_PLL_LOCK) != CGU_HSDC_STS_PLL_LOCK) {
			break;  //it is unlocked after reprogramming
		}
		_arc_nop();
		cnt--;
	}

	//Wait for lock
	cnt = TIMEOUT;
	while (cnt > 0) {
		if ( (*ptr & CGU_HSDC_STS_PLL_LOCK) == CGU_HSDC_STS_PLL_LOCK) {
			return 0; //ok
		}
		_arc_nop();
		cnt--;
	}

	return -1; //nok
}

volatile int cgu_pll_locked = 0;

static void cgu_pll_lock_handler(void *ptr)
{
	DBG("@@ PLL-locked\r\n");
	cgu_pll_locked++;
}

volatile int cgu_pll_unlocked = 0;

static void cgu_pll_unlock_handler(void *ptr)
{
	DBG("@@ PLL-unlocked\r\n");
	cgu_pll_unlocked++;
}

volatile int cgu_pll_error = 0;

static void cgu_pll_error_handler(void *ptr)
{
	DBG("@@ PLL-error\r\n");
	cgu_pll_error++;
}

static uint32_t cgu_pll_ctrl_reg(CGU_HSDC_PLL_CTRL_T *ctrl)
{
	return (((uint32_t)(ctrl->pd) << CGU_HSDC_CTRL_PD_POS) |
		((uint32_t)(ctrl->bypass) << CGU_HSDC_CTRL_BYPASS_POS) |
		((uint32_t)(ctrl->band) << CGU_HSDC_CTRL_BS_POS) |
		((uint32_t)(ctrl->od) << CGU_HSDC_CTRL_OD_POS) |
		((uint32_t)(ctrl->r) << CGU_HSDC_CTRL_R_POS) |
		((uint32_t)(ctrl->f) << CGU_HSDC_CTRL_F_POS));
}

static void cgu_get_ctrl_reg(uint32_t reg, CGU_HSDC_PLL_CTRL_T *ctrl)
{
	ctrl->pd = ((reg & CGU_HSDC_CTRL_PD) >> CGU_HSDC_CTRL_PD_POS);
	ctrl->bypass = ((reg & CGU_HSDC_CTRL_BYPASS) >> CGU_HSDC_CTRL_BYPASS_POS);
	ctrl->band = ((reg & CGU_HSDC_CTRL_BS) >> CGU_HSDC_CTRL_BS_POS);
	ctrl->od = ((reg & CGU_HSDC_CTRL_OD) >> CGU_HSDC_CTRL_OD_POS);
	ctrl->r = ((reg & CGU_HSDC_CTRL_R) >> CGU_HSDC_CTRL_R_POS);
	ctrl->f = ((reg & CGU_HSDC_CTRL_F) >> CGU_HSDC_CTRL_F_POS);
}

uint32_t cgu_pll_calc_freq(int32_t ref_freq, CGU_HSDC_PLL_CTRL_T *ctrl)
{
	uint32_t freq;

	if (ctrl->pd) {
		freq = 0;
	} else {
		if (ctrl->bypass) {
			freq =  ref_freq;
		} else {
			freq = (ref_freq / (ctrl->r + 1)) * (2 * (ctrl->f + 1)) / ( 1 << ctrl->od);
		}
	 }

	 return freq;
}

void cgu_hsdc_init(CGU_HSDC_STRUCT_PTR cgu_regs)
{

}

int32_t cgu_hsdc_set_pll(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_PLL_CONFIG_T *config)
{
	if (config->pll < CGU_HSDC_PLL_NONE) {
		volatile uint32_t    *ptr;
		CGU_HSDC_PLL_CTRL_T  ctrl;

		//Point to first element of reg-struct
		ptr  = (volatile uint32_t *)&cgu_regs->CGU_ARC_PLL_CTRL;
		//Calculate offset 0x00 / 0x10  / 0x20 /
		ptr += (4 * config->pll);

		//Get values from table
		ctrl.pd = cgu_register_pll_setting_table[config->freq].pd;
		ctrl.bypass = cgu_register_pll_setting_table[config->freq].bypass;
		ctrl.band = cgu_register_pll_setting_table[config->freq].BS;
		ctrl.od = cgu_register_pll_setting_table[config->freq].OD;
		ctrl.r = cgu_register_pll_setting_table[config->freq].R;
		ctrl.f  = cgu_register_pll_setting_table[config->freq].F;

		if (config->intnum != 0) {
			//use interrupt driver method
			cgu_pll_locked    = 0;
			cgu_pll_unlocked  = 0;
			cgu_pll_error     = 0;

			int_handler_install(config->intnum, cgu_pll_lock_handler);    //Lock
			int_handler_install(config->intnum+1, cgu_pll_unlock_handler);  //Unlock
			int_handler_install(config->intnum+2, cgu_pll_error_handler);  //Lockerror

			*ptr = cgu_pll_ctrl_reg(&ctrl);
			_arc_sync();

			//Interrupt method: wait for interrupt
			while ((cgu_pll_locked == 0) && (cgu_pll_error == 0));

			return 0;
		} else {
			//use polling method: bootloader
			*ptr = cgu_pll_ctrl_reg(&ctrl);
			_arc_sync();

			return cgu_wait_for_lock(cgu_regs, config->pll);
		}
	}

	return -1;
}

uint32_t cgu_hsdc_compare_pll(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_PLL_CONFIG_T *config)
{
	if (config->pll < CGU_HSDC_PLL_NONE) {
		volatile uint32_t    *ptr;
		uint32_t             new_freq;
		uint32_t             cur_freq;
		CGU_HSDC_PLL_CTRL_T  new_ctrl;
		CGU_HSDC_PLL_CTRL_T  cur_ctrl;

		//Point to first element of reg-struct
		ptr  = (volatile uint32_t *)&cgu_regs->CGU_ARC_PLL_CTRL;
		//Calculate offset 0x00 / 0x10  / 0x20 / ...
		ptr += (4*config->pll);

		//Get current values
		cgu_get_ctrl_reg(*ptr, &cur_ctrl);

		//Calculate current frequency
		cur_freq =  cgu_pll_calc_freq(33000, &cur_ctrl);
	 //DBG("Curent PLL config:\r\n");
	 //DBG(" - pd     %d\r\n",cur_ctrl.pd);
	 //DBG(" - bypass %d\r\n",cur_ctrl.bypass);
	 //DBG(" - band   %d\r\n",cur_ctrl.band);
	 //DBG(" - od     %d\r\n",cur_ctrl.od);
	 //DBG(" - r      %d\r\n",cur_ctrl.r);
	 //DBG(" - f      %d\r\n",cur_ctrl.f);
	 //DBG(" - freq   %d\r\n",cur_freq);

	//Get new values from table
		new_ctrl.pd = cgu_register_pll_setting_table[config->freq].pd;
		new_ctrl.bypass = cgu_register_pll_setting_table[config->freq].bypass;
		new_ctrl.band = cgu_register_pll_setting_table[config->freq].BS;
		new_ctrl.od = cgu_register_pll_setting_table[config->freq].OD;
		new_ctrl.r = cgu_register_pll_setting_table[config->freq].R;
		new_ctrl.f = cgu_register_pll_setting_table[config->freq].F;
		new_freq = cgu_pll_calc_freq (33000,&new_ctrl);

	 //DBG("New PLL config:\r\n");
	 //DBG(" - pd     %d\r\n",new_ctrl.pd);
	 //DBG(" - bypass %d\r\n",new_ctrl.bypass);
	 //DBG(" - band   %d\r\n",new_ctrl.band);
	 //DBG(" - od     %d\r\n",new_ctrl.od);
	 //DBG(" - r      %d\r\n",new_ctrl.r);
	 //DBG(" - f      %d\r\n",new_ctrl.f);
	 //DBG(" - freq   %d\r\n",new_freq);


		if ((new_ctrl.pd == cur_ctrl.pd) &&
			(new_ctrl.bypass == cur_ctrl.bypass) &&
			(new_ctrl.band == cur_ctrl.band) &&
			(new_ctrl.od == cur_ctrl.od) &&
			(new_ctrl.r == cur_ctrl.r) &&
			(new_ctrl.f == cur_ctrl.f)) {
			//DBG("New PLL freq = current PLL config\r\n");
			return 0; // new PLL = cur PLL
		} else{
			if (new_freq < cur_freq) {
				//DBG("New PLL freq < current PLL config\r\n");
				return 1; // new PLL < cur PLL
			} else {
				//DBG("New PLL freq > current PLL config\r\n");
				return 2; // new PLL > cur PLL
			}
			return 1; // new PLL > cur PLL
		}
	}

	return 3; //NOK
}

uint32_t cgu_hsdc_get_pll_freq(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_PLL_FREQ_T freq)
{
	if (freq < CGU_HSDC_PLL_FREQ_NONE) {
		return (cgu_register_pll_setting_table[freq].freq);
	}

	return 0;
}

//return 1 when PLL is in bypass
uint32_t cgu_hsdc_is_pll_in_bypass(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_PLL_T pll)
{
	volatile uint32_t *ptr;

	ptr  = (volatile uint32_t *)&cgu_regs->CGU_ARC_PLL_CTRL;

	//Calculate offset 0x00 / 0x10 / 0x20 / ...
	ptr += ((uint32_t)(pll) * 4);

	return ((*ptr & CGU_HSDC_CTRL_BYPASS) >> CGU_HSDC_CTRL_BYPASS_POS);
}

uint32_t cgu_hsdc_measure_pll(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_PLL_T pll, uint32_t fref)
{
	if (pll < CGU_HSDC_PLL_NONE) {
		uint32_t reg;
		uint32_t rcnt;
		uint32_t fcnt;
		volatile uint32_t *ptr;

		ptr  = (volatile uint32_t *)&cgu_regs->CGU_ARC_PLL_FMEAS;

		//Calculate offset 0x08 / 0x18 / 0x28 / ...
		ptr += ((uint32_t)(pll) * 4);

		// NOTE: The measuement logic inside the IC may not be properly reset after a software reset
		//       Hence, the first measurement after a software reset may be incorrect. To hide this
		//       artefact from the user we first do one dummy measurement inside the diver function
		//start dummy measurement
		*ptr = CGU_HSDC_FMEAS_START;
		_arc_sync();

		//wait till done bit is "1"
		reg = *ptr;
		while ( (reg & CGU_HSDC_FMEAS_DONE) != CGU_HSDC_FMEAS_DONE) {
			reg = *ptr;
		}

		//start measurement
		*ptr = CGU_HSDC_FMEAS_START;
		_arc_sync();

		//wait till done bit is "1"
		reg = *ptr;
		while ( (reg & CGU_HSDC_FMEAS_DONE) != CGU_HSDC_FMEAS_DONE) {
			reg = *ptr;
		}

		//measurement done, skip most sign bits
		reg  = (*ptr & (CGU_HSDC_FMEAS_RCNT | CGU_HSDC_FMEAS_FCNT));

		//stop measurement
		*ptr = 0;
		_arc_sync();

		rcnt = ((reg & CGU_HSDC_FMEAS_RCNT) >> 0);
		fcnt = ((reg & CGU_HSDC_FMEAS_FCNT) >> 15);
		fcnt *= (uint32_t)fref;
		reg  = (fcnt / (1000*rcnt));

		return reg; //freq in khz
	}

	return 0; //no freq
}

int32_t cgu_hsdc_set_idiv(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_IDIV_T idiv, uint8_t n, uint8_t div)
{
	volatile uint32_t *ptr;
	int32_t m;

	switch (idiv) {
		case CGU_HSDC_IDIV_ARC:
			m  = IDIV_ARC_NONE; //hs38
			ptr= (volatile uint32_t *)&cgu_regs->CGU_ARC_IDIV;
			break;
		case CGU_HSDC_IDIV_SYS:
			m  = IDIV_SYS_NONE; //apb, axi, .... ebi
			ptr= (volatile uint32_t *)&cgu_regs->CGU_SYS_IDIV_APB;
			break;
		case CGU_HSDC_IDIV_TUNNEL:
			m  = IDIV_TUNNEL_NONE; //tunnel_cl
			ptr  = (volatile uint32_t *)&cgu_regs->CGU_TUN_IDIV;
			break;
		case CGU_HSDC_IDIV_HDMI:
			m  = IDIV_HDMI_NONE; //hdmi_clk
			ptr  = (volatile uint32_t *)&cgu_regs->CGU_HDMI_IDIV_APB;
			break;
		case CGU_HSDC_IDIV_I2S:
			m  = IDIV_I2S_NONE; //i2s-tx, rx
			ptr  = (volatile uint32_t *)&cgu_regs->CGU_I2S_IDIV_TX;
			break;
		default:
			m = 0;
			break;
	}

	if (n < m) {
		//Calculate offset 0x00 / 0x10 / 0x20 / ...
		ptr += (uint32_t)(n) * 4;  //offset
		*ptr = (uint8_t)div;       //divider
		_arc_sync();
		return 0;
	}

	return -1; //nok
}

uint8_t cgu_hsdc_get_idiv(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_IDIV_T idiv, uint8_t n)
{
	volatile uint32_t *ptr;
	int32_t m;

	switch (idiv) {
		case CGU_HSDC_IDIV_ARC:
			m  = IDIV_ARC_NONE; //hs38
			ptr= (volatile uint32_t *)&cgu_regs->CGU_ARC_IDIV;
			break;
		case CGU_HSDC_IDIV_SYS:
			m  = IDIV_SYS_NONE; //apb, axi, .... ebi
			ptr= (volatile uint32_t *)&cgu_regs->CGU_SYS_IDIV_APB;
			break;
		case CGU_HSDC_IDIV_TUNNEL:
			m  = IDIV_TUNNEL_NONE; //tunnel_cl
			ptr  = (volatile uint32_t *)&cgu_regs->CGU_TUN_IDIV;
			break;
		case CGU_HSDC_IDIV_HDMI:
			m  = IDIV_HDMI_NONE; //hdmi_clk
			ptr  = (volatile uint32_t *)&cgu_regs->CGU_HDMI_IDIV_APB;
			break;
		case CGU_HSDC_IDIV_I2S:
			m  = IDIV_I2S_NONE; //i2s-tx, rx
			ptr  = (volatile uint32_t *)&cgu_regs->CGU_I2S_IDIV_TX;
			break;
		default:
			m = 0;
			break;
	}

	if (n < m) {
		//Calculate offset 0x00 / 0x10 / 0x20 / ...
		ptr += (uint32_t)(n) * 4;  //offset
		return (*ptr & 0xFF);
	}

	return 0;
}

uint32_t cgu_hsdc_measure_clk(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_FMEAS_T fmeas, uint8_t n, uint32_t fref)
{
	uint32_t reg = 0;
	uint32_t rcnt;
	uint32_t fcnt;
	volatile uint32_t *ptr;
	int32_t m;

	switch (fmeas) {
		default:
		case CGU_HSDC_FMEAS_ARC:
			m  = IDIV_ARC_NONE; //hs38
			ptr  = (volatile uint32_t *)&cgu_regs->CGU_ARC_FMEAS;
			break;
		case CGU_HSDC_FMEAS_SYS:
			m  = IDIV_SYS_NONE; //apb, axi, .... gpio
			ptr  = (volatile uint32_t *)&cgu_regs->CGU_SYS_FMEAS_APB;
			break;
		case CGU_HSDC_FMEAS_TUNNEL:
			m  = IDIV_TUNNEL_NONE; //tunnel_cl
			ptr  = (volatile uint32_t *)&cgu_regs->CGU_TUN_FMEAS;
			break;
		case CGU_HSDC_FMEAS_HDMI:
			m  = IDIV_HDMI_NONE; //hdmi_clk
			ptr  = (volatile uint32_t *)&cgu_regs->CGU_HDMI_FMEAS_APB;
			break;
		case CGU_HSDC_FMEAS_I2S:
			m  = IDIV_I2S_NONE; //i2s-tx, rx
			ptr  = (volatile uint32_t *)&cgu_regs->CGU_I2S_FMEAS_TX;
			break;
	}

	if (n < m) {
		//Calculate offset 0x00 / 0x10 / 0x20 / ...
		ptr += (uint32_t)(n) * 4;

		// NOTE: The measuement logic inside the IC may not be properly reset after a software reset
		//       Hence, the first measurement after a software reset may be incorrect. To hide this
		//       artefact from the user we first do one dummy measurement inside the diver function

		//start dummy measurement
		*ptr = CGU_HSDC_FMEAS_START;
		_arc_sync();

		//wait till done bit is "1"
		reg = *ptr;
		while ( (reg & CGU_HSDC_FMEAS_DONE) != CGU_HSDC_FMEAS_DONE) {
			reg = *ptr;
		}

		//start measurement
		*ptr = CGU_HSDC_FMEAS_START;
		_arc_sync();

		//wait till done bit is "1"
		reg = *ptr;
		while ( (reg & CGU_HSDC_FMEAS_DONE) != CGU_HSDC_FMEAS_DONE) {
			reg = *ptr;
		}

		//measurement done, skip most sign bits
		reg  = (*ptr & (CGU_HSDC_FMEAS_RCNT | CGU_HSDC_FMEAS_FCNT));

		//stop measurement
		*ptr = 0;
		_arc_sync();

		rcnt = ((reg & CGU_HSDC_FMEAS_RCNT) >> 0);
		fcnt = ((reg & CGU_HSDC_FMEAS_FCNT) >> 15);
		fcnt *= (uint32_t)fref;
		reg  = (fcnt / (1000*rcnt));
	}

	return (reg);
}

void cgu_hsdc_sw_reset_ctrl(CGU_HSDC_STRUCT_PTR cgu_regs, CGU_HSDC_RSTCTRL_T ctrl, uint32_t rst)
{
	if (ctrl < CGU_HSDC_RSTCTRL_NONE) {
		volatile uint32_t *ptr;

		ptr  = (volatile uint32_t *)&cgu_regs->CGU_ARC_RST_CTRL;

		//Calculate offset 0x00 / 0x20 / 0x40 / ...
		ptr += (uint32_t)(ctrl) * 8;

		*ptr = rst;
		_arc_sync();
	}
}

void cgu_hsdc_sw_reset(CGU_HSDC_STRUCT_PTR cgu_regs, uint32_t delay)
{
	uint32_t reg;

	reg = cgu_regs->CGU_IP_SW_RESET;
	reg &= ~(CGU_HSDC_SW_RESET_DELAY_MASK); //clr
	reg |= (delay << CGU_HSDC_SW_RESET_DELAY_POS);
	cgu_regs->CGU_IP_SW_RESET = (reg | CGU_HSDC_SW_RESET);
	_arc_sync();

	//wait till reset bit is back to 0
	reg = cgu_regs->CGU_IP_SW_RESET;
	while ( (reg & CGU_HSDC_SW_RESET) == CGU_HSDC_SW_RESET) {
		reg = cgu_regs->CGU_IP_SW_RESET;
	}
}