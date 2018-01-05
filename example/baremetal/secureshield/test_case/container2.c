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
 * \date 2016-12-01
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */

/**
 * \file
 * \ingroup	EMBARC_APP_BAREMETAL_SECURESHIELD_TEST_CASE
 * \brief	secureshield test case example container2 source file
 */

#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_assert.h"

#include "container1.h"
#include "container2.h"
#include "container4.h"

/* container2 is secure and linked with other normal containers
   so do not call normal world's code, as AUX_SEC_KP will be modified
   If you want to do this,  save AUX_SEC_KP first and retore it after call */

int tst_func3(int arg1, int arg2, int arg3)
{
	int ret;
	ret = container_call(container3, tst_func4, 1, 2, 3, 4);
	ret = container_call(container4, tst_func_sec1);
	/* secure container can directly access the system resources */
	_arc_aux_read(AUX_TIMER0_CNT);
	arc_int_enable(INTNO_SWI2);
	arc_int_sw_trigger(INTNO_SWI2);
	arc_int_sw_trigger(INTNO_SWI1);

	return ret;
}


int tst_func5(int arg1, int arg2, int arg3, int arg4, int arg5)
{
	/* container 2 is secure, it can directly access all system resources, no exception raised */
	/* the area pointed by challenge1 belongs to container1 */
	container1_context.initialized = 0;
	return 5;
}


void soft_interrupt2(void *p_exinf)
{
	/* if secure container is not really in secure state, _arc_lr_reg will raise an exception */
	_arc_aux_read(AUX_IRQ_CTRL);
}
