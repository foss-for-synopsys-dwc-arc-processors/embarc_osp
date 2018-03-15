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
#include "arc_secure_timer.h"
#include "arc.h"
#include "arc_builtin.h"

/**
 * \brief  check whether the specific secure timer present
 * \param[in] no timer number
 * \return 1 present, 0 not present
 */
int32_t secure_timer_present(const uint32_t no)
{
	uint32_t bcr = _arc_aux_read(AUX_BCR_TIMERS);
	switch (no) {
		case SECURE_TIMER_0:
			bcr = (bcr >> 11) & 1;
			break;
		case SECURE_TIMER_1:
			bcr = (bcr >> 12) & 1;
			break;
		default:
			bcr = 0;
			/* illegal argument so return false */
			break;
	}

	return (int)bcr;
}

/**
 * \brief  start the specific secure timer
 * \param[in] no	timer number
 * \param[in] mode	timer mode
 * \param[in] val	timer limit value (not for RTC)
 * \return 0 success, -1 failure
 */
int32_t secure_timer_start(const uint32_t no, const uint32_t mode, const uint32_t val)
{
	if (secure_timer_present(no) == 0) {
		return -1;
	}

	switch (no) {
		case SECURE_TIMER_0:
			_arc_aux_write(AUX_SECURE_TIMER0_CTRL, 0);
			_arc_aux_write(AUX_SECURE_TIMER0_LIMIT, val);
			_arc_aux_write(AUX_SECURE_TIMER0_CTRL, mode);
			_arc_aux_write(AUX_SECURE_TIMER0_CNT, 0);
			break;
		case SECURE_TIMER_1:
			_arc_aux_write(AUX_SECURE_TIMER1_CTRL, 0);
			_arc_aux_write(AUX_SECURE_TIMER1_LIMIT, val);
			_arc_aux_write(AUX_SECURE_TIMER1_CTRL, mode);
			_arc_aux_write(AUX_SECURE_TIMER1_CNT, 0);
			break;
		default:
			return -1;
	}

	return 0;
}

/**
 * \brief  stop and clear the specific secure timer
 *
 * \param[in] no timer number
 * \return 0 success, -1 failure
 */
int32_t secure_timer_stop(const uint32_t no)
{
	if (secure_timer_present(no) == 0) {
		return -1;
	}

	switch (no) {
		case SECURE_TIMER_0 :
			_arc_aux_write(AUX_SECURE_TIMER0_CTRL, 0);
			_arc_aux_write(AUX_SECURE_TIMER0_LIMIT,0);
			_arc_aux_write(AUX_SECURE_TIMER0_CNT, 0);
			break;
		case SECURE_TIMER_1:
			_arc_aux_write(AUX_SECURE_TIMER1_CTRL, 0);
			_arc_aux_write(AUX_SECURE_TIMER1_LIMIT,0);
			_arc_aux_write(AUX_SECURE_TIMER1_CNT, 0);
			break;
		default:
			return -1;
	}

	return 0;
}

/**
 * \brief  get secure timer current tick
 *
 * \param[in] no timer number
 * \param[out] val, timer value
 * \return 0 success, -1 failure
 */
int32_t secure_timer_current(const uint32_t no, void *val)
{
	if (secure_timer_present(no) == 0) {
		return -1;
	}

	switch (no) {
		case SECURE_TIMER_0 :
			*((uint32_t *)val) = _arc_aux_read(AUX_SECURE_TIMER0_CNT);
			break;
		case SECURE_TIMER_1 :
			*((uint32_t *)val) = _arc_aux_read(AUX_SECURE_TIMER1_CNT);
			break;
		default :
			return -1;
	}

	return 0;
}

/**
 * \brief  clear the interrupt pending bit of timer
 *
 * \param[in] no timer number
 * \return 0 success, -1 failure
 */
int32_t secure_timer_int_clear(const uint32_t no)
{
	uint32_t val;

	if (secure_timer_present(no) == 0) {
		return -1;
	}

	switch (no) {
		case SECURE_TIMER_0 :
			val = _arc_aux_read(AUX_SECURE_TIMER0_CTRL);
			val &= ~TIMER_CTRL_IP;
			_arc_aux_write(AUX_SECURE_TIMER0_CTRL, val);
			break;
		case SECURE_TIMER_1 :
			val = _arc_aux_read(AUX_SECURE_TIMER1_CTRL);
			val &= ~TIMER_CTRL_IP;
			_arc_aux_write(AUX_SECURE_TIMER1_CTRL, val);
			break;
		default :
			return -1;
	}

	return 0;
}

/**
 * \brief  init internal secure timer
 */
void secure_timer_init(void)
{
	secure_timer_stop(SECURE_TIMER_0);
	secure_timer_stop(SECURE_TIMER_1);
}



int32_t timer_present(const uint32_t no) EMBARC_LINKTO(secure_timer_present);
int32_t timer_start(const uint32_t no, const uint32_t mode, const uint32_t val) EMBARC_LINKTO(secure_timer_start);
int32_t timer_stop(const uint32_t no) EMBARC_LINKTO(secure_timer_stop);
int32_t timer_current(const uint32_t no, void *val) EMBARC_LINKTO(secure_timer_current);
int32_t timer_int_clear(const uint32_t no) EMBARC_LINKTO(secure_timer_int_clear);
void timer_init(void) EMBARC_LINKTO(secure_timer_init);