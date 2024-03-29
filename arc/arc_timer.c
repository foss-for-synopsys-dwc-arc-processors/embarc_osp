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

/**
 * @file
 * @ingroup	ARC_HAL_MISC_TIMER
 * @brief  implementation of internal timer related functions
 * @todo RTC support should be improved if RTC is enabled
 */
#include "arc/arc_timer.h"
#include "arc/arc_exception.h"

#define LPS_PREC 8

volatile uint64_t gl_loops_per_jiffy = 1;
volatile uint32_t gl_count = 1;

/**
 * @brief  check whether the specific timer present
 * @param[in] no timer number
 * @return 1 present, 0 not present
 */
int32_t arc_timer_present(const uint32_t no)
{
	uint32_t bcr = arc_aux_read(AUX_BCR_TIMERS);

	switch (no) {
	case TIMER_0:
		bcr = (bcr >> 8) & 1;
		break;
	case TIMER_1:
		bcr = (bcr >> 9) & 1;
		break;
	case TIMER_RTC:
		bcr = (bcr >> 10) & 1;
		break;
	default:
		bcr = 0;
		/* illegal argument so return false */
		break;
	}

	return (int)bcr;
}

/**
 * @brief  start the specific timer
 * @param[in] no	timer number
 * @param[in] mode	timer mode
 * @param[in] val	timer limit value (not for RTC)
 * @return 0 success, -1 failure
 */
int32_t arc_timer_start(const uint32_t no, const uint32_t mode, const uint32_t val)
{
	switch (no) {
	case TIMER_0:
		arc_aux_write(AUX_TIMER0_CTRL, 0);
		arc_aux_write(AUX_TIMER0_LIMIT, val);
		arc_aux_write(AUX_TIMER0_CTRL, mode);
		arc_aux_write(AUX_TIMER0_CNT, 0);
		break;
	case TIMER_1:
		arc_aux_write(AUX_TIMER1_CTRL, 0);
		arc_aux_write(AUX_TIMER1_LIMIT, val);
		arc_aux_write(AUX_TIMER1_CTRL, mode);
		arc_aux_write(AUX_TIMER1_CNT, 0);
		break;
	case TIMER_RTC:
		arc_aux_write(AUX_RTC_CTRL, mode);
		break;
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief  stop and clear the specific timer
 *
 * @param[in] no timer number
 * @return 0 success, -1 failure
 */
int32_t arc_timer_stop(const uint32_t no)
{
	switch (no) {
	case TIMER_0:
		arc_aux_write(AUX_TIMER0_CTRL, 0);
		arc_aux_write(AUX_TIMER0_LIMIT, 0);
		arc_aux_write(AUX_TIMER0_CNT, 0);
		break;
	case TIMER_1:
		arc_aux_write(AUX_TIMER1_CTRL, 0);
		arc_aux_write(AUX_TIMER1_LIMIT, 0);
		arc_aux_write(AUX_TIMER1_CNT, 0);
		break;
	case TIMER_RTC:
		arc_aux_write(AUX_RTC_CTRL, TIMER_RTC_CLEAR);
		break;
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief  get timer current tick
 *
 * @param[in] no timer number
 * @param[out] val, timer value
 * @return 0 success, -1 failure
 */
int32_t arc_timer_current(const uint32_t no, void *val)
{
	switch (no) {
	case TIMER_0:
		*((uint32_t *)val) = arc_aux_read(AUX_TIMER0_CNT);
		break;
	case TIMER_1:
		*((uint32_t *)val) = arc_aux_read(AUX_TIMER1_CNT);
		break;
	case TIMER_RTC:
		*((uint64_t *)val) = arc_aux_read(AUX_RTC_LOW);
		break;
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief  clear the interrupt pending bit of timer
 *
 * @param[in] no timer number
 * @return 0 success, -1 failure
 */
int32_t arc_timer_int_clear(const uint32_t no)
{
	uint32_t val;

	switch (no) {
	case TIMER_0:
		val = arc_aux_read(AUX_TIMER0_CTRL);
		val &= ~TIMER_CTRL_IP;
		arc_aux_write(AUX_TIMER0_CTRL, val);
		break;
	case TIMER_1:
		val = arc_aux_read(AUX_TIMER1_CTRL);
		val &= ~TIMER_CTRL_IP;
		arc_aux_write(AUX_TIMER1_CTRL, val);
		break;
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief  init internal timer
 */
void arc_timer_init(void)
{
	if (arc_timer_present(TIMER_0)) {
		arc_timer_stop(TIMER_0);
	}

	if (arc_timer_present(TIMER_1)) {
		arc_timer_stop(TIMER_1);
	}

	if (arc_timer_present(TIMER_RTC)) {
		arc_timer_stop(TIMER_RTC);
	}
}

#if defined(ARC_FEATURE_SEC_TIMER1_PRESENT) || defined(ARC_FEATURE_SEC_TIMER0_PRESENT)
/**
 * @brief  check whether the specific secure timer present
 * @param[in] no timer number
 * @return 1 present, 0 not present
 */
int32_t arc_secure_timer_present(const uint32_t no)
{
	uint32_t bcr = arc_aux_read(AUX_BCR_TIMERS);

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
 * @brief  start the specific secure timer
 * @param[in] no	timer number
 * @param[in] mode	timer mode
 * @param[in] val	timer limit value (not for RTC)
 * @return 0 success, -1 failure
 */
int32_t arc_secure_timer_start(const uint32_t no, const uint32_t mode, const uint32_t val)
{
	switch (no) {
	case SECURE_TIMER_0:
		arc_aux_write(AUX_SECURE_TIMER0_CTRL, 0);
		arc_aux_write(AUX_SECURE_TIMER0_LIMIT, val);
		arc_aux_write(AUX_SECURE_TIMER0_CTRL, mode);
		arc_aux_write(AUX_SECURE_TIMER0_CNT, 0);
		break;
	case SECURE_TIMER_1:
		arc_aux_write(AUX_SECURE_TIMER1_CTRL, 0);
		arc_aux_write(AUX_SECURE_TIMER1_LIMIT, val);
		arc_aux_write(AUX_SECURE_TIMER1_CTRL, mode);
		arc_aux_write(AUX_SECURE_TIMER1_CNT, 0);
		break;
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief  stop and clear the specific secure timer
 *
 * @param[in] no timer number
 * @return 0 success, -1 failure
 */
int32_t arc_secure_timer_stop(const uint32_t no)
{
	switch (no) {
	case SECURE_TIMER_0:
		arc_aux_write(AUX_SECURE_TIMER0_CTRL, 0);
		arc_aux_write(AUX_SECURE_TIMER0_LIMIT, 0);
		arc_aux_write(AUX_SECURE_TIMER0_CNT, 0);
		break;
	case SECURE_TIMER_1:
		arc_aux_write(AUX_SECURE_TIMER1_CTRL, 0);
		arc_aux_write(AUX_SECURE_TIMER1_LIMIT, 0);
		arc_aux_write(AUX_SECURE_TIMER1_CNT, 0);
		break;
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief  get secure timer current tick
 *
 * @param[in] no timer number
 * @param[out] val, timer value
 * @return 0 success, -1 failure
 */
int32_t arc_secure_timer_current(const uint32_t no, void *val)
{
	switch (no) {
	case SECURE_TIMER_0:
		*((uint32_t *)val) = arc_aux_read(AUX_SECURE_TIMER0_CNT);
		break;
	case SECURE_TIMER_1:
		*((uint32_t *)val) = arc_aux_read(AUX_SECURE_TIMER1_CNT);
		break;
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief  clear the interrupt pending bit of timer
 *
 * @param[in] no timer number
 * @return 0 success, -1 failure
 */
int32_t arc_secure_timer_int_clear(const uint32_t no)
{
	uint32_t val;

	switch (no) {
	case SECURE_TIMER_0:
		val = arc_aux_read(AUX_SECURE_TIMER0_CTRL);
		val &= ~TIMER_CTRL_IP;
		arc_aux_write(AUX_SECURE_TIMER0_CTRL, val);
		break;
	case SECURE_TIMER_1:
		val = arc_aux_read(AUX_SECURE_TIMER1_CTRL);
		val &= ~TIMER_CTRL_IP;
		arc_aux_write(AUX_SECURE_TIMER1_CTRL, val);
		break;
	default:
		return -1;
	}

	return 0;
}

/**
 * @brief  init internal secure timer
 */
void arc_secure_timer_init(void)
{
	if (arc_secure_timer_present(SECURE_TIMER_0)) {
		arc_secure_timer_stop(SECURE_TIMER_0);
	}

	if (arc_secure_timer_present(SECURE_TIMER_1)) {
		arc_secure_timer_stop(SECURE_TIMER_1);
	}
}
#endif /* ARC_FEATURE_SEC_TIMER1_PRESENT && ARC_FEATURE_SEC_TIMER0_PRESENT */

/**
 * @brief  provide US delay function
 *
 * @param[in] usecs US to delay
 */
void arc_delay_us(uint32_t usecs)
{
	if (usecs == 0) {
		return;
	}

	usecs = usecs * gl_loops_per_jiffy / gl_count;

	__asm__ __volatile__ (
		"	.align 4				\n"
		"	mov %%lp_count, %0			\n"
		"	lp  1f					\n"
		"	nop					\n"
		"1:						\n"
		:
		: "r" (usecs)
		: "lp_count");
}

/**
 * @brief  calibrate delay
 *
 * @param[in] board cpu clock
 * @return loops_per_jiffy
 */
uint64_t arc_timer_calibrate_delay(uint32_t cpu_clock)
{
	unsigned long loopbit;
	int lps_precision = LPS_PREC;
	volatile uint64_t loops_per_jiffy;
	uint32_t timer0_limit;
	uint32_t status;

	gl_loops_per_jiffy = 1;
	gl_count = 1;

	cpu_clock /= 1000;

	status = cpu_lock_save();

	timer0_limit = arc_aux_read(AUX_TIMER0_LIMIT);
	arc_aux_write(AUX_TIMER0_LIMIT, 0xFFFFFFFF);

	loops_per_jiffy = (1 << 4);
	while ((loops_per_jiffy <<= 1) != 0) {

		arc_aux_write(AUX_TIMER0_CNT, 0);
		arc_delay_us(loops_per_jiffy);
		if (arc_aux_read(AUX_TIMER0_CNT) > cpu_clock) {
			break;
		}
	}

	loops_per_jiffy >>= 1;
	loopbit = loops_per_jiffy;
	while (lps_precision-- && (loopbit >>= 1)) {

		loops_per_jiffy |= loopbit;
		arc_aux_write(AUX_TIMER0_CNT, 0);
		arc_delay_us(loops_per_jiffy);
		if (arc_aux_read(AUX_TIMER0_CNT) > cpu_clock) {
			loops_per_jiffy &= ~loopbit;
		}
	}

	gl_loops_per_jiffy = loops_per_jiffy;
	gl_count = 1000;

	arc_aux_write(AUX_TIMER0_CNT, 0);
	arc_aux_write(AUX_TIMER0_LIMIT, timer0_limit);
	cpu_unlock_restore(status);

	return loops_per_jiffy;
}
