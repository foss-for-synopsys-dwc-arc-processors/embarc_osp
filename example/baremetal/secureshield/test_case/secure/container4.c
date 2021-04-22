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
#undef LIB_SECURESHIELD_OVERRIDES
#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_assert.h"

#include "container3.h"
#include "container4.h"
#include "secureshield_sys.h"
#include "secureshield_int.h"

#ifndef BOARD_EMSDP
void timer1_interrupt1(void *p_exinf)
{
	uint32_t val;
	val = arc_aux_read(AUX_TIMER1_CTRL);
	val &= ~TIMER_CTRL_IP;
	arc_aux_write(AUX_TIMER1_CTRL, val);
	//secure_int_sw_trigger(INTNO_SWI3);
}
#endif

int tst_func_sec1(void)
{
#ifndef BOARD_EMSDP
	arc_aux_write(AUX_TIMER1_CTRL, 0);
	arc_aux_write(AUX_TIMER1_LIMIT, 5000);
	arc_aux_write(AUX_TIMER1_CTRL, TIMER_CTRL_IE|TIMER_CTRL_NH);
	arc_aux_write(AUX_TIMER1_CNT, 0);
	secure_int_handler_install(INTNO_TIMER1, timer1_interrupt1);
	secure_int_pri_set(INTNO_TIMER1, INT_PRI_MIN);
	secure_int_enable(INTNO_TIMER1);
#endif
	secure_int_sw_trigger(INTNO_SWI3);

	return 7;
}

int tst_func_sec2(void)
{
	return 8;
}