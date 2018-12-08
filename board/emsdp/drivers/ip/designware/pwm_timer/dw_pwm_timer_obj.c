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

#include "emsdp_hardware.h"
#include "dw_pwm_timer_obj.h"


#if (USE_DW_PWM_TIMER_0)
static DEV_PWM_TIMER	dw_pwm_timer_0;		/*!< designware pwm_timer object */
static DW_PWM_TIMER_CTRL dw_pwm_timer_0_ctrl;		/*!< designware pwm_timer group 0 ctrl */
static DEV_PWM_TIMER_HANDLER dw_pwm_timer_0_handler[DW_PWM_TIMER_0_CH_MAX_COUNT];	/*!< designware pwm_timer group 0 channels callback handler */

static DEV_PWM_TIMER_MODE dw_pwm_timer_0_mode[DW_PWM_TIMER_0_CH_MAX_COUNT]; /*!< designware pwm_timer group 0 channels mode */

static DW_PWM_TIMER_CH_ISR dw_pwm_timer_0_ch_isr = {
	DW_PWM_TIMER_0_CH_MAX_COUNT, dw_pwm_timer_0_handler
};


/** DesignWare PWM_TIMER 0 open */
static int32_t dw_pwm_timer_0_open (void)
{
	return dw_pwm_timer_open(&dw_pwm_timer_0);
}
/** DesignWare PWM_TIMER 0 close */
static int32_t dw_pwm_timer_0_close (void)
{
	return dw_pwm_timer_close(&dw_pwm_timer_0);
}
/** DesignWare PWM_TIMER 0 control */
static int32_t dw_pwm_timer_0_control (uint32_t ch, uint32_t cmd, void *param)
{
	return dw_pwm_timer_control(&dw_pwm_timer_0, ch, cmd, param);
}
/** DesignWare PWM_TIMER 0 write */
static int32_t dw_pwm_timer_0_write (uint32_t ch, uint32_t mode, uint32_t freq, uint32_t dc)
{
	return dw_pwm_timer_write(&dw_pwm_timer_0, ch, mode, freq, dc);
}
/** DesignWare PWM_TIMER 0 read */
static int32_t dw_pwm_timer_0_read (uint32_t ch, uint32_t *mode, uint32_t *freq, uint32_t *dc)
{
	return dw_pwm_timer_read(&dw_pwm_timer_0, ch, mode, freq, dc);
}
/** DesignWare PWM_TIMER 0 interrupt routine */
static void dw_pwm_timer_0_isr(void *ptr)
{
	dw_pwm_timer_isr_handler(&dw_pwm_timer_0, ptr);
}
/** Install DesignWare PWM_TIMER 0 to system */
static void dw_pwm_timer_0_install(void)
{
	uint32_t i;
	DEV_PWM_TIMER *dw_pwm_timer_ptr = &dw_pwm_timer_0;
	DEV_PWM_TIMER_INFO *dw_pwm_timer_info_ptr = &(dw_pwm_timer_0.pwm_timer_info);
	DW_PWM_TIMER_CTRL_PTR dw_pwm_timer_ctrl_ptr = &(dw_pwm_timer_0_ctrl);

	/* Info init */
	dw_pwm_timer_info_ptr->pwm_timer_ctrl = (void *)dw_pwm_timer_ctrl_ptr;
	dw_pwm_timer_info_ptr->opn_cnt = 0;

	dw_pwm_timer_ctrl_ptr->id = DW_PWM_TIMER_0_ID;
	dw_pwm_timer_ctrl_ptr->regs = (DW_PWM_TIMER_REG_PTR)(EMSDP_TIMERS0_BASE);
	dw_pwm_timer_ctrl_ptr->intno = EMSDP_TIMER0_INTR0;
	dw_pwm_timer_ctrl_ptr->clock = EMSDP_PWM_TIMER_CLOCK;
	dw_pwm_timer_ctrl_ptr->ch_num = DW_PWM_TIMER_0_CH_MAX_COUNT;
	dw_pwm_timer_ctrl_ptr->int_handler = dw_pwm_timer_0_isr;

	for (i=0; i < dw_pwm_timer_0_ch_isr.int_ch_max_cnt; i++) {
		dw_pwm_timer_0_ch_isr.int_ch_handler_ptr[i] = NULL;
	}
	dw_pwm_timer_ctrl_ptr->ch_isr = &(dw_pwm_timer_0_ch_isr);
	dw_pwm_timer_ctrl_ptr->mode = dw_pwm_timer_0_mode;


	/** PWM_TIMER dev init */
	dw_pwm_timer_ptr->pwm_timer_open = dw_pwm_timer_0_open;
	dw_pwm_timer_ptr->pwm_timer_close = dw_pwm_timer_0_close;
	dw_pwm_timer_ptr->pwm_timer_control = dw_pwm_timer_0_control;
	dw_pwm_timer_ptr->pwm_timer_write = dw_pwm_timer_0_write;
	dw_pwm_timer_ptr->pwm_timer_read = NULL;
}
#endif /* USE_DW_PWM_TIMER_0 */

#if (USE_DW_PWM_TIMER_1)
static DEV_PWM_TIMER	dw_pwm_timer_1;		/*!< designware pwm_timer object */
static DW_PWM_TIMER_CTRL dw_pwm_timer_1_ctrl;		/*!< designware pwm_timer group 1 ctrl */
static DEV_PWM_TIMER_HANDLER dw_pwm_timer_1_handler[DW_PWM_TIMER_1_CH_MAX_COUNT];	/*!< designware pwm_timer group 1 channels callback handler */

static DEV_PWM_TIMER_MODE dw_pwm_timer_1_mode[DW_PWM_TIMER_1_CH_MAX_COUNT]; /*!< designware pwm_timer group 1 channels mode */

static DW_PWM_TIMER_CH_ISR dw_pwm_timer_1_ch_isr = {
	DW_PWM_TIMER_1_CH_MAX_COUNT, dw_pwm_timer_1_handler
};


/** DesignWare PWM_TIMER 1 open */
static int32_t dw_pwm_timer_1_open (void)
{
	return dw_pwm_timer_open(&dw_pwm_timer_1);
}
/** DesignWare PWM_TIMER 1 close */
static int32_t dw_pwm_timer_1_close (void)
{
	return dw_pwm_timer_close(&dw_pwm_timer_1);
}
/** DesignWare PWM_TIMER 1 control */
static int32_t dw_pwm_timer_1_control (uint32_t ch, uint32_t cmd, void *param)
{
	return dw_pwm_timer_control(&dw_pwm_timer_1, ch, cmd, param);
}
/** DesignWare PWM_TIMER 1 write */
static int32_t dw_pwm_timer_1_write (uint32_t ch, uint32_t mode, uint32_t freq, uint32_t dc)
{
	return dw_pwm_timer_write(&dw_pwm_timer_1, ch, mode, freq, dc);
}
/** DesignWare PWM_TIMER 1 read */
static int32_t dw_pwm_timer_1_read (uint32_t ch, uint32_t *mode, uint32_t *freq, uint32_t *dc)
{
	return dw_pwm_timer_read(&dw_pwm_timer_1, ch, mode, freq, dc);
}
/** DesignWare PWM_TIMER 1 interrupt routine */
static void dw_pwm_timer_1_isr(void *ptr)
{
	dw_pwm_timer_isr_handler(&dw_pwm_timer_1, ptr);
}
/** Install DesignWare PWM_TIMER 1 to system */
static void dw_pwm_timer_1_install(void)
{
	uint32_t i;
	DEV_PWM_TIMER *dw_pwm_timer_ptr = &dw_pwm_timer_1;
	DEV_PWM_TIMER_INFO *dw_pwm_timer_info_ptr = &(dw_pwm_timer_1.pwm_timer_info);
	DW_PWM_TIMER_CTRL_PTR dw_pwm_timer_ctrl_ptr = &(dw_pwm_timer_1_ctrl);

	/* Info init */
	dw_pwm_timer_info_ptr->pwm_timer_ctrl = (void *)dw_pwm_timer_ctrl_ptr;
	dw_pwm_timer_info_ptr->opn_cnt = 0;

	dw_pwm_timer_ctrl_ptr->id = DW_PWM_TIMER_1_ID;
	dw_pwm_timer_ctrl_ptr->regs = (DW_PWM_TIMER_REG_PTR)(EMSDP_TIMERS1_BASE);
	dw_pwm_timer_ctrl_ptr->intno = EMSDP_TIMER1_INTR0;
	dw_pwm_timer_ctrl_ptr->clock = EMSDP_PWM_TIMER_CLOCK;
	dw_pwm_timer_ctrl_ptr->ch_num = DW_PWM_TIMER_1_CH_MAX_COUNT;
	dw_pwm_timer_ctrl_ptr->int_handler = dw_pwm_timer_1_isr;

	for (i=0; i < dw_pwm_timer_1_ch_isr.int_ch_max_cnt; i++) {
		dw_pwm_timer_1_ch_isr.int_ch_handler_ptr[i] = NULL;
	}
	dw_pwm_timer_ctrl_ptr->ch_isr = &(dw_pwm_timer_1_ch_isr);
	dw_pwm_timer_ctrl_ptr->mode = dw_pwm_timer_1_mode;


	/** PWM_TIMER dev init */
	dw_pwm_timer_ptr->pwm_timer_open = dw_pwm_timer_1_open;
	dw_pwm_timer_ptr->pwm_timer_close = dw_pwm_timer_1_close;
	dw_pwm_timer_ptr->pwm_timer_control = dw_pwm_timer_1_control;
	dw_pwm_timer_ptr->pwm_timer_write = dw_pwm_timer_1_write;
	dw_pwm_timer_ptr->pwm_timer_read = NULL;
}
#endif /* USE_DW_PWM_TIMER_1 */


/**
 * \brief	install all pwm_timer objects
 * \note	\b MUST be called during system init
 */
void dw_pwm_timer_all_install(void)
{
#if (USE_DW_PWM_TIMER_0)
	dw_pwm_timer_0_install();
#endif
#if (USE_DW_PWM_TIMER_1)
	dw_pwm_timer_1_install();
#endif
}


/** get one designware device structure */
DEV_PWM_TIMER_PTR pwm_timer_get_dev(int32_t pwm_timer_id)
{
	static uint32_t install_flag = 0;

	/* intall device objects */
	if (install_flag == 0) {
		install_flag = 1;
		dw_pwm_timer_all_install();
	}

	switch (pwm_timer_id) {
#if (USE_DW_PWM_TIMER_0)

		case DW_PWM_TIMER_0_ID:
			return &dw_pwm_timer_0;
			break;
#endif
#if (USE_DW_PWM_TIMER_1)

		case DW_PWM_TIMER_1_ID:
			return &dw_pwm_timer_1;
			break;
#endif
		default:
			break;
	}

	return NULL;
}


