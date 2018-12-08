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
#ifndef _DW_PWM_TIMER_OBJ_H_
#define _DW_PWM_TIMER_OBJ_H_

#include "dw_pwm_timer.h"

#define USE_DW_PWM_TIMER_0		1	/*!< enable use DesignWare PWM TIMER */
#define USE_DW_PWM_TIMER_1		1	/*!< enable use DesignWare PWM TIMER */



#define DW_PWM_TIMER_0_ID				0
#define DW_PWM_TIMER_1_ID				1

#define DW_PWM_TIMER_0_CH_MAX_COUNT		6
#define DW_PWM_TIMER_1_CH_MAX_COUNT		6

#ifdef __cplusplus
extern "C" {
#endif

extern DEV_PWM_TIMER_PTR pwm_timer_get_dev(int32_t pwm_timer_id);

#ifdef __cplusplus
}
#endif

#endif /* _DW_PWM_TIMER_OBJ_H_ */