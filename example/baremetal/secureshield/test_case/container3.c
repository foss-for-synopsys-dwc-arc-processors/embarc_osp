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
#include "embARC_assert.h"

#include "container3.h"

TEST_CONTEXT container3_context;

int tst_func4(int arg1, int arg2, int arg3, int arg4)
{
	EMBARC_PRINTF("tst_func4 in container 3, arguments are:%d,%d,%d,%d\r\n",
		arg1, arg2, arg3, arg4);
	/* container 3 initializes its own context */
	container3_context.initialized = 1;
	if (secureshield_int_enable(INTNO_SWI3) != 0) {
		EMBARC_PRINTF("failed to enable INTNO_SWI3\r\n");
	}
	if (secureshield_int_pri_set(INTNO_SWI3, INT_PRI_MIN) != 0) {
		EMBARC_PRINTF("failed to enable INTNO_SWIT3\r\n");
	}
	if (secureshield_int_pri_get(INTNO_SWI3) != INT_PRI_MIN) {
		EMBARC_PRINTF("interrupt pri ops error\r\n");
	}
	return 4;
}

int tst_func6(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6)
{
	EMBARC_PRINTF("tst_func6 in container 3, arguments are:%d,%d,%d,%d,%d,%d\r\n",
		arg1, arg2, arg3, arg4, arg5, arg6);
	/*
    	 * here timer interrupt belonging to background container will come out,
	 * test the case of interrupt handling with container switch
	 */
	board_delay_ms(100, 0);
	return 6;
}


void soft_interrupt3(void *p_exinf)
{
	EMBARC_PRINTF("soft interrupt 3 interrupt handler\r\n");
}