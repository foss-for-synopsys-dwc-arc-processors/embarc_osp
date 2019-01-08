/*------------------------------------------
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

#include "container1.h"
#include "container2.h"
#include "container3.h"
#include "background_container.h"


static void tst_func7(int arg)
{
	EMBARC_PRINTF("test function 7 in background container:%d\r\n", arg);
	cpu_lock();
	int_sw_trigger(INTNO_SWI0);
	cpu_unlock();
}

void soft_interrupt0(void *p_exinf)
{
	EMBARC_PRINTF("soft interrupt 0 interrupt handler\r\n");
}

void default_interrupt_handler(void *p_exinf)
{
	EMBARC_PRINTF("default interrupt handler\r\n");
}



int main(void)
{
	int32_t val;
	EMBARC_PRINTF("interrupt operation tests\r\n");
	EMBARC_PRINTF("test cases for secureshield_int_disable\r\n");

	EMBARC_ASSERT(secureshield_int_disable(INTNO_TIMER0) == 0);
	EMBARC_ASSERT(secureshield_int_disable(INTNO_SWI1) != 0);

	EMBARC_PRINTF("test cases for secureshield_int_enable\r\n");
	EMBARC_ASSERT(secureshield_int_enable(INTNO_TIMER0) == 0);
	EMBARC_ASSERT(secureshield_int_enable(INTNO_SWI1) != 0);

	EMBARC_PRINTF("test cases for secureshield_int_enabled\r\n");
	EMBARC_ASSERT(secureshield_int_enabled(INTNO_TIMER0) > 0);
	EMBARC_ASSERT(secureshield_int_enabled(INTNO_SWI1) == -1);

	EMBARC_PRINTF("test cases for secureshield_int_prt_set/get\r\n");
	val = secureshield_int_pri_get(INTNO_TIMER0);
	EMBARC_ASSERT(val < 0);
	EMBARC_ASSERT(secureshield_int_pri_get(INTNO_SWI1) == 0);
	EMBARC_ASSERT(secureshield_int_pri_set(INTNO_TIMER0, val) == 0);
	EMBARC_ASSERT(secureshield_int_pri_set(INTNO_SWI1, val) < 0);

	EMBARC_PRINTF("test cases for secureshield_int_handler_install/get\r\n");
	val = (int32_t)secureshield_int_handler_get(INTNO_TIMER0);
	EMBARC_ASSERT(val != 0);
	EMBARC_ASSERT(secureshield_int_handler_get(INTNO_SWI1) == 0);
	EMBARC_ASSERT(secureshield_int_handler_install(INTNO_TIMER0, default_interrupt_handler) == 0);
	EMBARC_ASSERT(secureshield_int_handler_install(INTNO_SWI1, default_interrupt_handler) < 0);
	EMBARC_ASSERT(secureshield_int_handler_get(INTNO_TIMER0) == default_interrupt_handler);
	EMBARC_ASSERT(secureshield_int_handler_install(INTNO_TIMER0, (void *)val) == 0);

	EMBARC_PRINTF("test cases for secureshield_int_sw_trigger and probe\r\n");
	secureshield_cpu_lock();
	EMBARC_ASSERT(secureshield_int_sw_trigger(INTNO_TIMER0) == 0);
	EMBARC_ASSERT(secureshield_int_sw_trigger(INTNO_SWI1) < 0);
	EMBARC_ASSERT(secureshield_int_probe(INTNO_TIMER0) > 0);
	EMBARC_ASSERT(secureshield_int_probe(INTNO_SWI1) < 0);
	secureshield_cpu_unlock();

	EMBARC_PRINTF("test cases for secureshield_int_level_config");
	EMBARC_ASSERT(secureshield_int_level_config(INTNO_TIMER0, 0) == 0);
	EMBARC_ASSERT(secureshield_int_level_config(INTNO_TIMER0, 1) == 0);
	EMBARC_ASSERT(secureshield_int_level_config(INTNO_SWI1, 0) < 0);

	int_handler_install(INTNO_SWI0, soft_interrupt0);
	int_enable(INTNO_SWI0);

	EMBARC_PRINTF("container call from background to 1 \r\n");
	EMBARC_PRINTF("ret is:%x\r\n", container_call(container1, tst_func1, 1));

	container_call(container1, tst_func2, 1, 2);

	EMBARC_PRINTF("container call from background to 2\r\n");
	EMBARC_PRINTF("ret is:%x\r\n", container_call(container2, tst_func5, 1, 2, 3, 4, 5));

	EMBARC_PRINTF("container call from background to 3\r\n");
	EMBARC_PRINTF("ret is:%x\r\n", container_call(container3, tst_func6, 1, 2, 3, 4, 5, 6));

	EMBARC_PRINTF("container call from background to 4\r\n");
	EMBARC_PRINTF("ret is:%x\r\n", container_call(container4, tst_func_sec2));

	EMBARC_PRINTF("IRQ_ACT is:%x\r\n", secureshield_arc_lr_reg(AUX_IRQ_ACT));

	tst_func7(7);

	return E_SYS;
}