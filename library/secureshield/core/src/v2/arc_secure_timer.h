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
 * \version 2017.03
 * \date 2017-01-05
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */
#ifndef _ARC_HAL_SECURE_TIMER_H_
#define _ARC_HAL_SECURE_TIMER_H_
#include "embARC_toolchain.h"

/**
 * \name arc internal timers names
 * @{
 */
#define SECURE_TIMER_0		0 	/*!< macro name for arc internal secure timer 0 */
#define SECURE_TIMER_1		1 	/*!< macro name for arc internal secure timer 1 */

/** @} */


/**
 * \name bit definition of timer CTRL reg
 * @{
 */
#define TIMER_CTRL_IE		(1 << 0)	/*!< Interrupt when count reaches limit */
#define TIMER_CTRL_NH		(1 << 1)	/*!< Count only when CPU NOT halted */
#define TIMER_CTRL_W		(1 << 2)	/*!< watchdog enable */
#define TIMER_CTRL_IP		(1 << 3)	/*!< interrupt pending */

/** @} */

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t secure_timer_present(const uint32_t no);
extern int32_t secure_timer_start(const uint32_t no, const uint32_t mode, const uint32_t val);
extern int32_t secure_timer_stop(const uint32_t no);
extern int32_t secure_timer_current(const uint32_t no, void* val);
extern int32_t secure_timer_int_clear(const uint32_t no);
extern void secure_timer_init(void);

#ifdef __cplusplus
}
#endif

#endif	/* _ARC_HAL_SECURE_TIMER_H_ */