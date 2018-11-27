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
/**
 * \defgroup	DEVICE_HAL_PWM_TIMER		PWM_TIMER Device HAL Interface
 * \ingroup	DEVICE_HAL_DEF
 * \brief	Definitions for pwm_timer device hardware layer (\ref dev_pwm_timer.h)
 * \details	Provide unified APIs for pwm_timer driver to implement.
 *  Here is a diagram for the pwm_timer interface.
 *
 *  \htmlonly
 *  <div class="imagebox">
 *      <div style="width: 600px">
 *          <img src="pic/dev_pwm_timer_hal.jpg" alt="PWM_TIMER Device HAL Interface Diagram"/>
 *          <p>PWM_TIMER Device HAL Interface Diagram</p>
 *      </div>
 *  </div>
 *  \endhtmlonly
 *
 * ### Reference Links
 *     * [PWM Port](https://en.wikipedia.org/wiki/Pulse-width_modulation)
 *
 * @{
 *
 * \file
 * \brief	pwm_timer device hardware layer definitions
 * \details	Provide common definitions for pwm_timer device,
 * 	then software developer can develop pwm_timer driver
 * 	following this definitions, and the applications
 * 	can directly call this definition to realize functions
 *
 */

#ifndef _DEVICE_HAL_PWM_TIMER_H_
#define _DEVICE_HAL_PWM_TIMER_H_

#include "dev_common.h"

/**
 * \defgroup	DEVICE_HAL_PWM_TIMER_CTRLCMD		PWM_TIMER Device Control Commands
 * \ingroup	DEVICE_HAL_PWM_TIMER
 * \brief	Definitions for pwm_timer control command, used in \ref dev_pwm_timer::pwm_timer_control "PWM_TIMER IO Control"
 * \details	These commands defined here can be used in user code directly.
 * - Parameters Usage
 *   - For passing parameters like integer, just use uint32_t/int32_t to directly pass values
 *   - For passing parameters for a structure, please use pointer to pass values
 *   - For getting some data, please use pointer to store the return data
 * - Common Return Values
 *   - \ref E_OK,	Control device successfully
 *   - \ref E_CLSED,	Device is not opened
 *   - \ref E_OBJ,	Device object is not valid or not exists
 *   - \ref E_PAR,	Parameter is not valid for current control command
 *   - \ref E_SYS,	Control device failed, due to hardware issues such as device is disabled
 *   - \ref E_CTX,	Control device failed, due to different reasons like in transfer state
 *   - \ref E_NOSPT,	Control command is not supported or not valid
 * @{
 */

#define PWM_TIMER_CMD_SET_CFG			DEV_SET_SYSCMD(0)

#define PWM_TIMER_CMD_GET_CFG			DEV_SET_SYSCMD(1)

#define PWM_TIMER_CMD_ENA_CH			DEV_SET_SYSCMD(2)

#define PWM_TIMER_CMD_DIS_CH			DEV_SET_SYSCMD(3)

#define PWM_TIMER_CMD_SET_ISR			DEV_SET_SYSCMD(4)

#define PWM_TIMER_CMD_GET_ISR			DEV_SET_SYSCMD(5)

#define PWM_TIMER_CMD_DIS_ISR			DEV_SET_SYSCMD(6)

#define PWM_TIMER_CMD_ENA_ISR			DEV_SET_SYSCMD(7)

/** PWM_TIMER interrupt handler or Interrupt Service Routine(ISR) */
typedef void (*DEV_PWM_TIMER_HANDLER) (void *ptr);

/** interrupt handler for each pwm_timer channel */
typedef struct dev_pwm_timer_ch_isr {
	uint32_t int_ch_num;		/*!< pwm_timer channel num*/
	DEV_PWM_TIMER_HANDLER int_ch_handler;		/*!< interrupt handler */
} DEV_PWM_TIMER_CH_ISR, * DEV_PWM_TIMER_CH_ISR_PTR;

/**
 * PWM_TIMER Device Work Mode Types Enum
 */
typedef enum dev_pwm_timer_mode {
	DEV_PWM_TIMER_MODE_CLOSE = 0,	/*!< mode close */
	DEV_PWM_TIMER_MODE_TIMER = 1,	/*!< mode timer */
	DEV_PWM_TIMER_MODE_PWM = 2		/*!< mode pwm */
} DEV_PWM_TIMER_MODE, *DEV_PWM_TIMER_MODE_PTR;


/**
 * PWM_TIMER Device Config Struct
 */
typedef struct dev_pwm_timer_cfg {
	DEV_PWM_TIMER_MODE mode;		/*!< work mode*/
	uint16_t count_low;				/*!< count for timer or pwm period low*/
	uint16_t count_high;			/*!< count for pwm period high*/
	DEV_PWM_TIMER_HANDLER isr_hander;		/*!< channel ch isr hander*/
} DEV_PWM_TIMER_CFG, *DEV_PWM_TIMER_CFG_PTR;



typedef struct dev_pwm_timer_info {
	void *pwm_timer_ctrl;
	uint32_t opn_cnt;
	//uint32_t status;
	//uint32_t mode;
} DEV_PWM_TIMER_INFO, * DEV_PWM_TIMER_INFO_PTR;

typedef struct dev_pwm_timer {
	DEV_PWM_TIMER_INFO pwm_timer_info;				/*!< PWM_TIMER device information */
	int32_t (*pwm_timer_open) (void);				/*!< Open pwm_timer device */
	int32_t (*pwm_timer_close) (void);				/*!< Close pwm_timer device */
	int32_t (*pwm_timer_control) (uint32_t ch, uint32_t cmd, void *param2);	/*!< Control pwm_timer device */
	int32_t (*pwm_timer_write) (uint32_t ch, uint32_t mode, uint32_t freq, uint32_t dc);	/*!< Send data by pwm_timer device(blocked) */
	int32_t (*pwm_timer_read) (uint32_t ch, uint32_t *mode, uint32_t *freq, uint32_t *dc);	/*!< Read data from pwm_timer device(blocked) */
} DEV_PWM_TIMER, * DEV_PWM_TIMER_PTR;



#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief	get an \ref dev_pwm_timer "pwm_timer device" by pwm_timer device id.
 * 	For how to use pwm_timer device hal refer to \ref dev_pwm_timer "Functions in pwm_timer device structure"
 * \param[in]	pwm_timer_id	id of pwm_timer, defined by user
 * \retval	!NULL	pointer to an \ref dev_pwm_timer "pwm_timer device structure"
 * \retval	NULL	failed to find the pwm_timer device by \ref pwm_timer_id
 * \note	need to implemented by user in user code
 */
extern DEV_PWM_TIMER_PTR pwm_timer_get_dev(int32_t pwm_timer_id);

#ifdef __cplusplus
}
#endif

#endif /* _DEVICE_HAL_PWM_TIMER_H_ */
