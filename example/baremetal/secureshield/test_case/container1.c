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

#include "background_container.h"
#include "container1.h"

TEST_CONTEXT container1_context;

int tst_func0(void)
{
	EMBARC_PRINTF("tst_func0 in container 1, no arguments\r\n");
	EMBARC_PRINTF("my container id is:%d\r\n",secureshield_container_id_self());
	EMBARC_PRINTF("the id of caller is:%d\r\n", secureshield_container_id_caller());
	return 0;
}

int tst_func1(int arg1)
{
	EMBARC_PRINTF("tst_func1 in container 1, arguments are:%d\r\n", arg1);
	container1_context.initialized = 1;
	if (secureshield_int_enable(INTNO_SWI1) != 0) {
		EMBARC_PRINTF("failed to enable INTNO_SWI1\r\n");
	}
	if (secureshield_int_sw_trigger(INTNO_SWI1) != 0) {
		EMBARC_PRINTF("failed to trigger soft interrupt1 \r\n");
	}
	EMBARC_PRINTF("container call from 1 to 2\r\n");
	EMBARC_PRINTF("ret is:%x\r\n", container_call(container2, tst_func3, 1, 2, 3));
	return 1;
}

int tst_func2(int arg1, int arg2)
{
	EMBARC_PRINTF("tst_func2 in container 1, arguments are:%d,%d\r\n", arg1, arg2);
	return 2;
}


void soft_interrupt1(void *p_exinf)
{
	EMBARC_PRINTF("soft interrupt 1 interrupt handler\r\n");
}

