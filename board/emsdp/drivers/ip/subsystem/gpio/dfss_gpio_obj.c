/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

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
#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "board.h"

#include "dfss_gpio_obj.h"
#include "ip/subsystem/gpio/ss_gpio.h"


#if (USE_DFSS_GPIO_0)

#define DFSS_GPIO_0_LEN	21
static void gpio_int_cb0(void *param);
static DEV_GPIO		gpio_0;
static DEV_GPIO_HANDLER gpio_int_handlers0[DFSS_GPIO_0_LEN];
static SS_GPIO_DEV_CONTEXT gpio_context0 = {
	AR_IO_GPIO0_SWPORTA_DR,
	(DFSS_GPIO_0_ID-DFSS_GPIO_BASE_ID), DFSS_GPIO_0_LEN, IO_GPIO0_INT_INTR_FLAG,
	0, gpio_int_cb0, gpio_int_handlers0, 0
};


static void gpio_int_cb0(void *param)
{
	ss_gpio_int_cb(&gpio_context0, param);
}

static int32_t dfss_gpio_0_open(uint32_t dir)
{
	return ss_gpio_open(&gpio_context0, dir);
}

static int32_t dfss_gpio_0_close(void)
{
	return ss_gpio_close(&gpio_context0);
}

static int32_t dfss_gpio_0_control(uint32_t ctrl_cmd, void *param)
{
	return ss_gpio_control(&gpio_context0, ctrl_cmd, param);
}

static int32_t dfss_gpio_0_write(uint32_t val, uint32_t mask)
{
	return ss_gpio_write(&gpio_context0, val, mask);
}

static int32_t dfss_gpio_0_read(uint32_t *val, uint32_t mask)
{
	return ss_gpio_read(&gpio_context0, val, mask);
}

static void dfss_gpio_0_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_0;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_0.gpio_info);

	gpio_context0.info = &(gpio_0.gpio_info);

	info_ptr->gpio_ctrl = (void *)&gpio_context0;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = NULL;

	gpio->gpio_open = dfss_gpio_0_open;
	gpio->gpio_close = dfss_gpio_0_close;
	gpio->gpio_control = dfss_gpio_0_control;
	gpio->gpio_write = dfss_gpio_0_write;
	gpio->gpio_read = dfss_gpio_0_read;
}
#endif // USE_DFSS_GPIO_1

#if (USE_DFSS_GPIO_1)

#define DFSS_GPIO_1_LEN	23
static void gpio_int_cb1(void *param);
static DEV_GPIO		gpio_1;
static DEV_GPIO_HANDLER gpio_int_handlers1[DFSS_GPIO_1_LEN];
static SS_GPIO_DEV_CONTEXT gpio_context1 = {
	AR_IO_GPIO1_SWPORTA_DR,
	(DFSS_GPIO_1_ID-DFSS_GPIO_BASE_ID), DFSS_GPIO_1_LEN, IO_GPIO1_INT_INTR_FLAG,
	0, gpio_int_cb1, gpio_int_handlers1, 0
};


static void gpio_int_cb1(void *param)
{
	ss_gpio_int_cb(&gpio_context1, param);
}

static int32_t dfss_gpio_1_open(uint32_t dir)
{
	return ss_gpio_open(&gpio_context1, dir);
}

static int32_t dfss_gpio_1_close(void)
{
	return ss_gpio_close(&gpio_context1);
}

static int32_t dfss_gpio_1_control(uint32_t ctrl_cmd, void *param)
{
	return ss_gpio_control(&gpio_context1, ctrl_cmd, param);
}

static int32_t dfss_gpio_1_write(uint32_t val, uint32_t mask)
{
	return ss_gpio_write(&gpio_context1, val, mask);
}

static int32_t dfss_gpio_1_read(uint32_t *val, uint32_t mask)
{
	return ss_gpio_read(&gpio_context1, val, mask);
}

static void dfss_gpio_1_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_1;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_1.gpio_info);

	gpio_context1.info = &(gpio_1.gpio_info);

	info_ptr->gpio_ctrl = (void *)&gpio_context1;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = NULL;

	gpio->gpio_open = dfss_gpio_1_open;
	gpio->gpio_close = dfss_gpio_1_close;
	gpio->gpio_control = dfss_gpio_1_control;
	gpio->gpio_write = dfss_gpio_1_write;
	gpio->gpio_read = dfss_gpio_1_read;
}
#endif // USE_DFSS_GPIO_1

#if (USE_DFSS_GPIO_2)

#define DFSS_GPIO_2_LEN	32
static void gpio_int_cb2(void *param);
static DEV_GPIO		gpio_2;
static DEV_GPIO_HANDLER gpio_int_handlers2[DFSS_GPIO_2_LEN];
static SS_GPIO_DEV_CONTEXT gpio_context2 = {
	AR_IO_GPIO2_SWPORTA_DR,
	(DFSS_GPIO_2_ID-DFSS_GPIO_BASE_ID), DFSS_GPIO_2_LEN, IO_GPIO2_INT_INTR_FLAG,
	0, gpio_int_cb2, gpio_int_handlers2, 0
};


static void gpio_int_cb2(void *param)
{
	ss_gpio_int_cb(&gpio_context2, param);
}

static int32_t dfss_gpio_2_open(uint32_t dir)
{
	return ss_gpio_open(&gpio_context2, dir);
}

static int32_t dfss_gpio_2_close(void)
{
	return ss_gpio_close(&gpio_context2);
}

static int32_t dfss_gpio_2_control(uint32_t ctrl_cmd, void *param)
{
	return ss_gpio_control(&gpio_context2, ctrl_cmd, param);
}

static int32_t dfss_gpio_2_write(uint32_t val, uint32_t mask)
{
	return ss_gpio_write(&gpio_context2, val, mask);
}

static int32_t dfss_gpio_2_read(uint32_t *val, uint32_t mask)
{
	return ss_gpio_read(&gpio_context2, val, mask);
}

static void dfss_gpio_2_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_2;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_2.gpio_info);

	gpio_context2.info = &(gpio_2.gpio_info);

	info_ptr->gpio_ctrl = (void *)&gpio_context2;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = NULL;

	gpio->gpio_open = dfss_gpio_2_open;
	gpio->gpio_close = dfss_gpio_2_close;
	gpio->gpio_control = dfss_gpio_2_control;
	gpio->gpio_write = dfss_gpio_2_write;
	gpio->gpio_read = dfss_gpio_2_read;
}
#endif // USE_DFSS_GPIO_2

#if (USE_DFSS_GPIO_3)

#define DFSS_GPIO_3_LEN	8
static void gpio_int_cb3(void *param);
static DEV_GPIO		gpio_3;
static DEV_GPIO_HANDLER gpio_int_handlers3[DFSS_GPIO_3_LEN];
static SS_GPIO_DEV_CONTEXT gpio_context3 = {
	AR_IO_GPIO3_SWPORTA_DR,
	(DFSS_GPIO_3_ID-DFSS_GPIO_BASE_ID), DFSS_GPIO_3_LEN, IO_GPIO3_INT_INTR_FLAG,
	0, gpio_int_cb3, gpio_int_handlers3, 0
};


static void gpio_int_cb3(void *param)
{
	ss_gpio_int_cb(&gpio_context3, param);
}

static int32_t dfss_gpio_3_open(uint32_t dir)
{
	return ss_gpio_open(&gpio_context3, dir);
}

static int32_t dfss_gpio_3_close(void)
{
	return ss_gpio_close(&gpio_context3);
}

static int32_t dfss_gpio_3_control(uint32_t ctrl_cmd, void *param)
{
	return ss_gpio_control(&gpio_context3, ctrl_cmd, param);
}

static int32_t dfss_gpio_3_write(uint32_t val, uint32_t mask)
{
	return ss_gpio_write(&gpio_context3, val, mask);
}

static int32_t dfss_gpio_3_read(uint32_t *val, uint32_t mask)
{
	return ss_gpio_read(&gpio_context3, val, mask);
}

static void dfss_gpio_3_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_3;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_3.gpio_info);

	gpio_context3.info = &(gpio_3.gpio_info);

	info_ptr->gpio_ctrl = (void *)&gpio_context3;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = NULL;

	gpio->gpio_open = dfss_gpio_3_open;
	gpio->gpio_close = dfss_gpio_3_close;
	gpio->gpio_control = dfss_gpio_3_control;
	gpio->gpio_write = dfss_gpio_3_write;
	gpio->gpio_read = dfss_gpio_3_read;
}
#endif // USE_DFSS_GPIO_3

#if (USE_DFSS_GPIO_8B0)

#define DFSS_GPIO_8B0_LEN	8
static void gpio_int_cb4(void *param);
static DEV_GPIO		gpio_4;
static DEV_GPIO_HANDLER gpio_int_handlers4[DFSS_GPIO_8B0_LEN];
static SS_GPIO_DEV_CONTEXT gpio_context4 = {
	AR_IO_GPIO_8B0_SWPORTA_DR,
	(DFSS_GPIO_8B0_ID-DFSS_GPIO_BASE_ID), DFSS_GPIO_8B0_LEN, IO_GPIO_8B0_INT_INTR_FLAG,
	0, gpio_int_cb4, gpio_int_handlers4, 0
};


static void gpio_int_cb4(void *param)
{
	ss_gpio_int_cb(&gpio_context4, param);
}

static int32_t dfss_gpio_4_open(uint32_t dir)
{
	return ss_gpio_open(&gpio_context4, dir);
}

static int32_t dfss_gpio_4_close(void)
{
	return ss_gpio_close(&gpio_context4);
}

static int32_t dfss_gpio_4_control(uint32_t ctrl_cmd, void *param)
{
	return ss_gpio_control(&gpio_context4, ctrl_cmd, param);
}

static int32_t dfss_gpio_4_write(uint32_t val, uint32_t mask)
{
	return ss_gpio_write(&gpio_context4, val, mask);
}

static int32_t dfss_gpio_4_read(uint32_t *val, uint32_t mask)
{
	return ss_gpio_read(&gpio_context4, val, mask);
}

static void dfss_gpio_4_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_4;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_4.gpio_info);

	gpio_context4.info = &(gpio_4.gpio_info);

	info_ptr->gpio_ctrl = (void *)&gpio_context4;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = NULL;

	gpio->gpio_open = dfss_gpio_4_open;
	gpio->gpio_close = dfss_gpio_4_close;
	gpio->gpio_control = dfss_gpio_4_control;
	gpio->gpio_write = dfss_gpio_4_write;
	gpio->gpio_read = dfss_gpio_4_read;
}
#endif // USE_DFSS_GPIO_8B0

#if USE_DFSS_GPIO_8B1

#define DFSS_GPIO_8B1_LEN	8
static void gpio_int_cb5(void *param);
static DEV_GPIO		gpio_5;
static DEV_GPIO_HANDLER gpio_int_handlers5[DFSS_GPIO_8B1_LEN];
static SS_GPIO_DEV_CONTEXT gpio_context5 = {
	AR_IO_GPIO_8B1_SWPORTA_DR,
	(DFSS_GPIO_8B1_ID-DFSS_GPIO_BASE_ID), DFSS_GPIO_8B1_LEN, IO_GPIO_8B1_INT_INTR_FLAG,
	0, gpio_int_cb5, gpio_int_handlers5, 0
};

static void gpio_int_cb5(void *param)
{
	ss_gpio_int_cb(&gpio_context5, param);
}

static int32_t dfss_gpio_5_open(uint32_t dir)
{
	return ss_gpio_open(&gpio_context5, dir);
}

static int32_t dfss_gpio_5_close(void)
{
	return ss_gpio_close(&gpio_context5);
}

static int32_t dfss_gpio_5_control(uint32_t ctrl_cmd, void *param)
{
	return ss_gpio_control(&gpio_context5, ctrl_cmd, param);
}

static int32_t dfss_gpio_5_write(uint32_t val, uint32_t mask)
{
	return ss_gpio_write(&gpio_context5, val, mask);
}

static int32_t dfss_gpio_5_read(uint32_t *val, uint32_t mask)
{
	return ss_gpio_read(&gpio_context5, val, mask);
}

static void dfss_gpio_5_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_5;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_5.gpio_info);

	gpio_context5.info = &(gpio_5.gpio_info);

	info_ptr->gpio_ctrl = (void *)&gpio_context5;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = NULL;

	gpio->gpio_open = dfss_gpio_5_open;
	gpio->gpio_close = dfss_gpio_5_close;
	gpio->gpio_control = dfss_gpio_5_control;
	gpio->gpio_write = dfss_gpio_5_write;
	gpio->gpio_read = dfss_gpio_5_read;
}
#endif // USE_DFSS_GPIO_8B1


#if USE_DFSS_GPIO_8B2

#define DFSS_GPIO_8B2_LEN	8
static void gpio_int_cb6(void *param);
static DEV_GPIO		gpio_6;
static DEV_GPIO_HANDLER gpio_int_handlers6[DFSS_GPIO_8B2_LEN];
static SS_GPIO_DEV_CONTEXT gpio_context6 = {
	AR_IO_GPIO_8B2_SWPORTA_DR,
	(DFSS_GPIO_8B2_ID-DFSS_GPIO_BASE_ID), DFSS_GPIO_8B2_LEN, IO_GPIO_8B2_INT_INTR_FLAG,
	0, gpio_int_cb6, gpio_int_handlers6, 0
};

static void gpio_int_cb6(void *param)
{
	ss_gpio_int_cb(&gpio_context6, param);
}

static int32_t dfss_gpio_6_open(uint32_t dir)
{
	return ss_gpio_open(&gpio_context6, dir);
}

static int32_t dfss_gpio_6_close(void)
{
	return ss_gpio_close(&gpio_context6);
}

static int32_t dfss_gpio_6_control(uint32_t ctrl_cmd, void *param)
{
	return ss_gpio_control(&gpio_context6, ctrl_cmd, param);
}

static int32_t dfss_gpio_6_write(uint32_t val, uint32_t mask)
{
	return ss_gpio_write(&gpio_context6, val, mask);
}

static int32_t dfss_gpio_6_read(uint32_t *val, uint32_t mask)
{
	return ss_gpio_read(&gpio_context6, val, mask);
}

static void dfss_gpio_6_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_6;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_6.gpio_info);

	gpio_context6.info = &(gpio_6.gpio_info);

	info_ptr->gpio_ctrl = (void *)&gpio_context6;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = NULL;

	gpio->gpio_open = dfss_gpio_6_open;
	gpio->gpio_close = dfss_gpio_6_close;
	gpio->gpio_control = dfss_gpio_6_control;
	gpio->gpio_write = dfss_gpio_6_write;
	gpio->gpio_read = dfss_gpio_6_read;
}
#endif // USE_DFSS_GPIO_8B2

#if USE_DFSS_GPIO_8B3

#define DFSS_GPIO_8B3_LEN	8
static void gpio_int_cb7(void *param);
static DEV_GPIO		gpio_7;
static DEV_GPIO_HANDLER gpio_int_handlers7[DFSS_GPIO_8B3_LEN];
static SS_GPIO_DEV_CONTEXT gpio_context7 = {
	AR_IO_GPIO_8B3_SWPORTA_DR,
	(DFSS_GPIO_8B3_ID-DFSS_GPIO_BASE_ID), DFSS_GPIO_8B3_LEN, IO_GPIO_8B3_INT_INTR_FLAG,
	0, gpio_int_cb7, gpio_int_handlers7, 0
};

static void gpio_int_cb7(void *param)
{
	ss_gpio_int_cb(&gpio_context7, param);
}

static int32_t dfss_gpio_7_open(uint32_t dir)
{
	return ss_gpio_open(&gpio_context7, dir);
}

static int32_t dfss_gpio_7_close(void)
{
	return ss_gpio_close(&gpio_context7);
}

static int32_t dfss_gpio_7_control(uint32_t ctrl_cmd, void *param)
{
	return ss_gpio_control(&gpio_context7, ctrl_cmd, param);
}

static int32_t dfss_gpio_7_write(uint32_t val, uint32_t mask)
{
	return ss_gpio_write(&gpio_context7, val, mask);
}

static int32_t dfss_gpio_7_read(uint32_t *val, uint32_t mask)
{
	return ss_gpio_read(&gpio_context7, val, mask);
}

static void dfss_gpio_7_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_7;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_7.gpio_info);

	gpio_context7.info = &(gpio_7.gpio_info);

	info_ptr->gpio_ctrl = (void *)&gpio_context7;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = NULL;

	gpio->gpio_open = dfss_gpio_7_open;
	gpio->gpio_close = dfss_gpio_7_close;
	gpio->gpio_control = dfss_gpio_7_control;
	gpio->gpio_write = dfss_gpio_7_write;
	gpio->gpio_read = dfss_gpio_7_read;
}
#endif // USE_DFSS_GPIO_8B3

#if USE_DFSS_GPIO_4B0

#define DFSS_GPIO_4B0_LEN	4
static void gpio_int_cb8(void *param);
static DEV_GPIO		gpio_8;
static DEV_GPIO_HANDLER gpio_int_handlers8[DFSS_GPIO_4B0_LEN];
static SS_GPIO_DEV_CONTEXT gpio_context8 = {
	AR_IO_GPIO_4B0_SWPORTA_DR,
	(DFSS_GPIO_4B0_ID-DFSS_GPIO_BASE_ID), DFSS_GPIO_4B0_LEN, IO_GPIO_4B0_INT_INTR_FLAG,
	0, gpio_int_cb8, gpio_int_handlers8, 0
};

static void gpio_int_cb8(void *param)
{
	ss_gpio_int_cb(&gpio_context8, param);
}

static int32_t dfss_gpio_8_open(uint32_t dir)
{
	return ss_gpio_open(&gpio_context8, dir);
}

static int32_t dfss_gpio_8_close(void)
{
	return ss_gpio_close(&gpio_context8);
}

static int32_t dfss_gpio_8_control(uint32_t ctrl_cmd, void *param)
{
	return ss_gpio_control(&gpio_context8, ctrl_cmd, param);
}

static int32_t dfss_gpio_8_write(uint32_t val, uint32_t mask)
{
	return ss_gpio_write(&gpio_context8, val, mask);
}

static int32_t dfss_gpio_8_read(uint32_t *val, uint32_t mask)
{
	return ss_gpio_read(&gpio_context8, val, mask);
}

static void dfss_gpio_8_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_8;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_8.gpio_info);

	gpio_context8.info = &(gpio_8.gpio_info);

	info_ptr->gpio_ctrl = (void *)&gpio_context8;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = NULL;

	gpio->gpio_open = dfss_gpio_8_open;
	gpio->gpio_close = dfss_gpio_8_close;
	gpio->gpio_control = dfss_gpio_8_control;
	gpio->gpio_write = dfss_gpio_8_write;
	gpio->gpio_read = dfss_gpio_8_read;
}
#endif // USE_DFSS_GPIO_4B0


#if USE_DFSS_GPIO_4B1

#define DFSS_GPIO_4B1_LEN	4
static void gpio_int_cb9(void *param);
static DEV_GPIO		gpio_9;
static DEV_GPIO_HANDLER gpio_int_handlers9[DFSS_GPIO_4B1_LEN];
static SS_GPIO_DEV_CONTEXT gpio_context9 = {
	AR_IO_GPIO_4B1_SWPORTA_DR,
	(DFSS_GPIO_4B1_ID-DFSS_GPIO_BASE_ID), DFSS_GPIO_4B1_LEN, IO_GPIO_4B1_INT_INTR_FLAG,
	0, gpio_int_cb9, gpio_int_handlers9, 0
};

static void gpio_int_cb9(void *param)
{
	ss_gpio_int_cb(&gpio_context9, param);
}

static int32_t dfss_gpio_9_open(uint32_t dir)
{
	return ss_gpio_open(&gpio_context9, dir);
}

static int32_t dfss_gpio_9_close(void)
{
	return ss_gpio_close(&gpio_context9);
}

static int32_t dfss_gpio_9_control(uint32_t ctrl_cmd, void *param)
{
	return ss_gpio_control(&gpio_context9, ctrl_cmd, param);
}

static int32_t dfss_gpio_9_write(uint32_t val, uint32_t mask)
{
	return ss_gpio_write(&gpio_context9, val, mask);
}

static int32_t dfss_gpio_9_read(uint32_t *val, uint32_t mask)
{
	return ss_gpio_read(&gpio_context9, val, mask);
}

static void dfss_gpio_9_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_9;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_9.gpio_info);

	gpio_context9.info = &(gpio_9.gpio_info);

	info_ptr->gpio_ctrl = (void *)&gpio_context9;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->direction = 0;
	info_ptr->extra = NULL;

	gpio->gpio_open = dfss_gpio_9_open;
	gpio->gpio_close = dfss_gpio_9_close;
	gpio->gpio_control = dfss_gpio_9_control;
	gpio->gpio_write = dfss_gpio_9_write;
	gpio->gpio_read = dfss_gpio_9_read;

}
#endif // USE_DFSS_GPIO_4B1

#if USE_DFSS_GPIO_4B2

#define DFSS_GPIO_4B2_LEN	4
static void gpio_int_cb10(void *param);
static DEV_GPIO		gpio_10;
static DEV_GPIO_HANDLER gpio_int_handlers10[DFSS_GPIO_4B2_LEN];
static SS_GPIO_DEV_CONTEXT gpio_context10 = {
	AR_IO_GPIO_4B2_SWPORTA_DR,
	(DFSS_GPIO_4B2_ID-DFSS_GPIO_BASE_ID), DFSS_GPIO_4B2_LEN, IO_GPIO_4B2_INT_INTR_FLAG,
	0, gpio_int_cb10, gpio_int_handlers10, 0
};

static void gpio_int_cb10(void *param)
{
	ss_gpio_int_cb(&gpio_context10, param);
}

static int32_t dfss_gpio_10_open(uint32_t dir)
{
	return ss_gpio_open(&gpio_context10, dir);
}

static int32_t dfss_gpio_10_close(void)
{
	return ss_gpio_close(&gpio_context10);
}

static int32_t dfss_gpio_10_control(uint32_t ctrl_cmd, void *param)
{
	return ss_gpio_control(&gpio_context10, ctrl_cmd, param);
}

static int32_t dfss_gpio_10_write(uint32_t val, uint32_t mask)
{
	return ss_gpio_write(&gpio_context10, val, mask);
}

static int32_t dfss_gpio_10_read(uint32_t *val, uint32_t mask)
{
	return ss_gpio_read(&gpio_context10, val, mask);
}

static void dfss_gpio_10_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_10;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_10.gpio_info);

	gpio_context10.info = &(gpio_10.gpio_info);

	info_ptr->gpio_ctrl = (void *)&gpio_context10;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = NULL;

	gpio->gpio_open = dfss_gpio_10_open;
	gpio->gpio_close = dfss_gpio_10_close;
	gpio->gpio_control = dfss_gpio_10_control;
	gpio->gpio_write = dfss_gpio_10_write;
	gpio->gpio_read = dfss_gpio_10_read;
}
#endif // USE_DFSS_GPIO_4B2

#if USE_DFSS_GPIO_4B3

#define DFSS_GPIO_4B3_LEN	4
static void gpio_int_cb11(void *param);
static DEV_GPIO		gpio_11;
static DEV_GPIO_HANDLER gpio_int_handlers11[DFSS_GPIO_4B3_LEN];
static SS_GPIO_DEV_CONTEXT gpio_context11 = {
	AR_IO_GPIO_4B3_SWPORTA_DR,
	(DFSS_GPIO_4B3_ID-DFSS_GPIO_BASE_ID), DFSS_GPIO_4B3_LEN, IO_GPIO_4B3_INT_INTR_FLAG,
	0, gpio_int_cb11, gpio_int_handlers11, 0
};

static void gpio_int_cb11(void *param)
{
	ss_gpio_int_cb(&gpio_context11, param);
}

static int32_t dfss_gpio_11_open(uint32_t dir)
{
	return ss_gpio_open(&gpio_context11, dir);
}

static int32_t dfss_gpio_11_close(void)
{
	return ss_gpio_close(&gpio_context11);
}

static int32_t dfss_gpio_11_control(uint32_t ctrl_cmd, void *param)
{
	return ss_gpio_control(&gpio_context11, ctrl_cmd, param);
}

static int32_t dfss_gpio_11_write(uint32_t val, uint32_t mask)
{
	return ss_gpio_write(&gpio_context11, val, mask);
}

static int32_t dfss_gpio_11_read(uint32_t *val, uint32_t mask)
{
	return ss_gpio_read(&gpio_context11, val, mask);
}

static void dfss_gpio_11_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_11;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_11.gpio_info);

	gpio_context11.info = &(gpio_11.gpio_info);

	info_ptr->gpio_ctrl = (void *)&gpio_context11;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = NULL;

	gpio->gpio_open = dfss_gpio_11_open;
	gpio->gpio_close = dfss_gpio_11_close;
	gpio->gpio_control = dfss_gpio_11_control;
	gpio->gpio_write = dfss_gpio_11_write;
	gpio->gpio_read = dfss_gpio_11_read;
}
#endif // USE_DFSS_GPIO_4B3

void dfss_gpio_all_install(void);

DEV_GPIO_PTR dfss_gpio_get_dev(int32_t gpio_id)
{
	static uint32_t install_flag = 0;

	/* intall device objects */
	if (install_flag == 0) {
		install_flag = 1;
		dfss_gpio_all_install();
	}

	switch (gpio_id) {
#if (USE_DFSS_GPIO_0)
		case DFSS_GPIO_0_ID: return &gpio_0;
#endif
#if (USE_DFSS_GPIO_1)
		case DFSS_GPIO_1_ID: return &gpio_1;
#endif
#if (USE_DFSS_GPIO_2)
		case DFSS_GPIO_2_ID: return &gpio_2;
#endif
#if (USE_DFSS_GPIO_3)
		case DFSS_GPIO_3_ID: return &gpio_3;
#endif
#if (USE_DFSS_GPIO_8B0)
		case DFSS_GPIO_8B0_ID: return &gpio_4;
#endif
#if (USE_DFSS_GPIO_8B1)
		case DFSS_GPIO_8B1_ID: return &gpio_5;
#endif
#if (USE_DFSS_GPIO_8B2)
		case DFSS_GPIO_8B2_ID: return &gpio_6;
#endif
#if (USE_DFSS_GPIO_8B3)
		case DFSS_GPIO_8B3_ID: return &gpio_7;
#endif
#if (USE_DFSS_GPIO_4B0)
		case DFSS_GPIO_4B0_ID: return &gpio_8;
#endif
#if (USE_DFSS_GPIO_4B1)
		case DFSS_GPIO_4B1_ID: return &gpio_9;
#endif
#if (USE_DFSS_GPIO_4B2)
		case DFSS_GPIO_4B2_ID: return &gpio_10;
#endif
#if (USE_DFSS_GPIO_4B3)
		case DFSS_GPIO_4B3_ID: return &gpio_11;
#endif
		default:
			break;
	}
	return NULL;
}

void dfss_gpio_all_install(void)
{
#if (USE_DFSS_GPIO_0)
	dfss_gpio_0_install();
#endif
#if (USE_DFSS_GPIO_1)
	dfss_gpio_1_install();
#endif
#if (USE_DFSS_GPIO_2)
	dfss_gpio_2_install();
#endif
#if (USE_DFSS_GPIO_3)
	dfss_gpio_3_install();
#endif
#if (USE_DFSS_GPIO_8B0)
	dfss_gpio_4_install();
#endif
#if (USE_DFSS_GPIO_8B1)
	dfss_gpio_5_install();
#endif
#if (USE_DFSS_GPIO_8B2)
	dfss_gpio_6_install();
#endif
#if (USE_DFSS_GPIO_8B3)
	dfss_gpio_7_install();
#endif
#if (USE_DFSS_GPIO_4B0)
	dfss_gpio_8_install();
#endif
#if (USE_DFSS_GPIO_4B1)
	dfss_gpio_9_install();
#endif
#if (USE_DFSS_GPIO_4B2)
	dfss_gpio_10_install();
#endif
#if (USE_DFSS_GPIO_4B3)
	dfss_gpio_11_install();
#endif
}
