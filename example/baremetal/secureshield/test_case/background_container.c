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
 * \version 2017.03
 * \date 2016-12-01
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */
/**
 * \defgroup	EMBARC_APP_BAREMETAL_SECURESHIELD_TEST_CASE	embARC Secureshield Test Cases Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_BAREMETAL
 * \brief	embARC Secureshield Test Cases example
 *
 * \details
 * ### Extra Required Tools
 *     - Designware nSIM Tool
 *
 * ### Extra Required Peripherals
 *
 * ### Design Concept
 *     This example is designed to test the secureshield's API functionality.
 *
 * ### Usage Manual
 *     Run this example you will see the following output.
 *
 *     ![ScreenShot of secureshield example](pic/images/example/emsk/baremetal_secureshield.jpg)
 *
 *
 * ### Extra Comments
 *
 */

/**
 * \file
 * \ingroup	EMBARC_APP_BAREMETAL_SECURESHIELD_TEST_CASE
 * \brief	secureshield test case example background container source file
 */

/**
 * \addtogroup	EMBARC_APP_BAREMETAL_SECURESHIELD_TEST_CASE
 * @{
 */

/* embARC HAL */
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

	EMBARC_PRINTF("timer0 interrupt prioirty is:%d\r\n", secureshield_int_pri_get(INTNO_TIMER0));

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

/** @} */
