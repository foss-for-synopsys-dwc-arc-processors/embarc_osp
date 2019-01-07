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
/* embARC HAL */
#include "embARC.h"
#include "embARC_debug.h"

#include "arc_xy.h"

#define DATA_SIZE 1024

static ARC_X_CCM uint32_t a[DATA_SIZE];
static ARC_Y_CCM uint32_t b[DATA_SIZE];
static ARC_X_CCM uint32_t c[DATA_SIZE];

static uint32_t a1[DATA_SIZE];
static uint32_t b1[DATA_SIZE];
static uint32_t c1[DATA_SIZE];



void xy_uint32_t_add(uint32_t *src1, uint32_t *src2, uint32_t *dst, uint32_t num)
{
	uint32_t i = 0;
	_arc_agu_ap_set(0, src1);
	_arc_agu_ap_set(1, src2);
	_arc_agu_ap_set(2, dst);

	_arc_agu_mod_set(0, 0, ARC_AGU_DT_i32, ARC_AGU_AD_INC_i32(1));
	_arc_agu_mod_set(1, 1, ARC_AGU_DT_i32, ARC_AGU_AD_INC_i32(1));
	_arc_agu_mod_set(2, 2, ARC_AGU_DT_i32, ARC_AGU_AD_INC_i32(1));

	for (i = 0; i < num; i++) {
#if defined(__MW__)
		Asm("add %agu_u2, %agu_u1, %agu_u0");
#else
		Asm("add %r34, %r32, %r33");
#endif
	}

}

void uint32_t_add(uint32_t *src1, uint32_t *src2, uint32_t *dst, uint32_t num)
{
	uint32_t i = 0;

	for (i = 0; i < num; i++) {
		*dst = *src1 + *src2;
		src1++;
		src2++;
		dst++;
	}
}

int main(void)
{
	uint32_t i;
	uint32_t start, end;

	/* data initialization */
	for (i = 0; i < DATA_SIZE; i++) {
		a[i] = i;
		b[i] = DATA_SIZE - i;
		c[i] = 0;

		a1[i] = i;
		b1[i] = DATA_SIZE - i;
		c1[i] = 0;
	}

	timer_current(TIMER_0, &start);
	xy_uint32_t_add(a, b, c, DATA_SIZE);
	timer_current(TIMER_0, &end);


	if (end > start) {
		end = end - start;
	} else {
		end += _arc_aux_read(AUX_TIMER0_LIMIT) - start;
	}

	EMBARC_PRINTF("add with xy ccm for %d numbers : %d cycles\r\n", DATA_SIZE, end);


	timer_current(TIMER_0, &start);
	uint32_t_add(a1, b1, c1, DATA_SIZE);
	timer_current(TIMER_0, &end);


	if (end > start) {
		end = end - start;
	} else {
		end += _arc_aux_read(AUX_TIMER0_LIMIT) - start;
	}

	EMBARC_PRINTF("add for %d numbers : %d cycles\r\n", DATA_SIZE, end);
}
