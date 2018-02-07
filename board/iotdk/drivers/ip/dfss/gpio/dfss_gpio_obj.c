/* ------------------------------------------
 * Copyright (c) 2016, Synopsys, Inc. All rights reserved.

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
 * \date 2017-07-31
 * \author Qiang Gu(Qiang.Gu@synopsys.com)
--------------------------------------------- */
/**
 * \defgroup	BOARD_IOTDK_DRV_DFSS_GPIO_OBJ	IOTDK DFSS GPIO Object
 * \ingroup	BOARD_IOTDK_DRIVER
 * \brief	IOTDK DFSS GPIO Objects
 */

/**
 * \file
 * \ingroup	BOARD_IOTDK_DRV_DFSS_GPIO_OBJ
 * \brief	DFSS GPIO object instantiation on iotdk
 */

/**
 * \addtogroup	BOARD_IOTDK_DRV_DFSS_GPIO_OBJ
 * @{
 */
#include <string.h>

#include "arc.h"
#include "arc_builtin.h"
#include "arc_exception.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "../../iotdk.h"
#include "dev_gpio.h"
#include "gpio.h"

typedef struct gpio_dev_context
{
	uint8_t		dev_id;
	uint8_t 	width;
	uint8_t		intno;
	uint8_t 	reserved;
	IO_CB_FUNC	int_cb;
	DEV_GPIO_HANDLER *handlers;
	DEV_GPIO_INFO	*info;
	uint32_t	int_bit_type;
	uint32_t	int_bit_polarity;
	uint32_t	int_bit_debounce;
} GPIO_DEV_CONTEXT;



static int32_t gpio_open(GPIO_DEV_CONTEXT *ctx, uint32_t dir)
{
	DEV_GPIO_INFO *info = ctx->info;
	int32_t ret = E_OK;
	io_cb_t cb;

	info->opn_cnt++;

	if (info->opn_cnt > 1) { /* opened before */
		if (dir == info->direction) { /* direction is the same */
			return E_OK;
		} else { /* open with different direction */
			return E_OPNED;
		}
	}

	if (io_gpio_open(ctx->dev_id)) {
		return E_SYS;
	}

	info->direction = dir;
	info->method = 0;
	ctx->int_bit_type = 0;
	ctx->int_bit_polarity = 0;
	ctx->int_bit_debounce = 0;
	cb.cb = ctx->int_cb;
	io_gpio_ioctl(ctx->dev_id, IO_SET_CB_RX, &cb);
	io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_INT_ENABLE, &info->method);
	io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_DIRECTION, &info->direction);
	io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_INT_POLARITY, &ctx->int_bit_polarity);
	io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_DEBOUNCE, &ctx->int_bit_debounce);


	return ret;
}

static int32_t gpio_close(GPIO_DEV_CONTEXT *ctx)
{
	DEV_GPIO_INFO *info = ctx->info;

	info->opn_cnt--;

	if (info->opn_cnt == 0) {
		io_gpio_close(ctx->dev_id);
		int_disable(ctx->intno);
	} else {
		return E_OPNED;
	}

	return E_OK;
}

static int32_t gpio_control(GPIO_DEV_CONTEXT *ctx, uint32_t ctrl_cmd, void *param)
{
	int32_t ret = E_OK;
	DEV_GPIO_INFO *info = ctx->info;
	DEV_GPIO_INT_CFG *cfg = (DEV_GPIO_INT_CFG *)param;
	DEV_GPIO_BIT_ISR *isr = (DEV_GPIO_BIT_ISR *)param;
	uint32_t mask;

	switch (ctrl_cmd) {
		case GPIO_CMD_SET_BIT_DIR_INPUT:
		case GPIO_CMD_SET_BIT_DIR_OUTPUT:
			if (ctrl_cmd == GPIO_CMD_SET_BIT_DIR_OUTPUT) {
				info->direction |= ((uint32_t)param);
			} else {
				info->direction &= ~((uint32_t)param);
			}
			io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_DIRECTION, &(info->direction));
			break;
		case GPIO_CMD_GET_BIT_DIR:
			*((uint32_t *) param) = info->direction;
			break;

		case GPIO_CMD_ENA_BIT_INT:
		case GPIO_CMD_DIS_BIT_INT:
			if (ctrl_cmd == GPIO_CMD_ENA_BIT_INT) {
				info->method |= ((uint32_t)param);
			}
			else {
				info->method &= ~((uint32_t)param);
			}
			io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_INT_ENABLE, &(info->method));
			if (info->method) {
				int_enable(ctx->intno);
			} else {
				int_disable(ctx->intno);
			}
			break;

		case GPIO_CMD_GET_BIT_MTHD:
			io_gpio_ioctl(ctx->dev_id, IO_GPIO_GET_INT_ENABLE, &(info->method));
			*((uint32_t *) param) = info->method;
			break;
		case GPIO_CMD_SET_BIT_INT_CFG:
			mask = cfg->int_bit_mask;
			ctx->int_bit_type &= ~(mask);
			ctx->int_bit_type |= cfg->int_bit_type;
			io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_INT_TYPE, &ctx->int_bit_type);

			ctx->int_bit_polarity &= ~(mask);
			ctx->int_bit_polarity |= cfg->int_bit_polarity;
			io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_INT_POLARITY, &ctx->int_bit_polarity);

			cfg->int_bit_debounce &= mask;
			ctx->int_bit_debounce &= ~(mask);
			ctx->int_bit_debounce |= cfg->int_bit_debounce;
			io_gpio_ioctl(ctx->dev_id, IO_GPIO_SET_DEBOUNCE, &ctx->int_bit_debounce);
			break;

		case GPIO_CMD_GET_BIT_INT_CFG:
			cfg->int_bit_type = ctx->int_bit_type & cfg->int_bit_mask;
			cfg->int_bit_polarity = ctx->int_bit_polarity & cfg->int_bit_mask;
			cfg->int_bit_debounce = ctx->int_bit_debounce & cfg->int_bit_mask;
			break;

		case GPIO_CMD_SET_BIT_ISR:
			if (isr->int_bit_ofs < ctx->width) {
				ctx->handlers[isr->int_bit_ofs] = isr->int_bit_handler;
			} else {
				ret = E_PAR;
			}
			break;

		case GPIO_CMD_GET_BIT_ISR:
			if (isr->int_bit_ofs < ctx->width) {
				isr->int_bit_handler = ctx->handlers[isr->int_bit_ofs];
			} else {
				ret = E_PAR;
			}
			break;
		default:
			ret = E_NOSPT;
			break;
	}

	return ret;
}

static int32_t gpio_write(GPIO_DEV_CONTEXT *ctx, uint32_t val, uint32_t mask)
{
	uint32_t temp_val;

	io_gpio_read(ctx->dev_id, &temp_val);
	temp_val &= (~mask);
	val &= mask;
	io_gpio_write(ctx->dev_id, temp_val | val);

	return 0;
}

static int32_t gpio_read(GPIO_DEV_CONTEXT *ctx, uint32_t *val, uint32_t mask)
{
	uint32_t raw;

	io_gpio_read(ctx->dev_id, &raw);
	*val = raw & mask;

	return 0;
}

#if (USE_DFSS_GPIO_8B0)

static void gpio_int_cb0(uint32_t status);
static DEV_GPIO		gpio_0;
static DEV_GPIO_HANDLER gpio_int_handlers0[8];
static GPIO_DEV_CONTEXT gpio_context0 = {
	DFSS_GPIO_8B0_ID, 8, IO_GPIO_8B0_INT_INTR_FLAG,
	0, gpio_int_cb0, gpio_int_handlers0,0
};


static void gpio_int_cb0(uint32_t status)
{
	GPIO_DEV_CONTEXT *ctx = &gpio_context0;
	DEV_GPIO_INFO *info = ctx->info;
	uint32_t i;

	for (i = 0; i < ctx->width; i++) {
		if ((status & (1 << i)) && ctx->handlers[i]) {
			ctx->handlers[i](info);
		}
	}
}

static int32_t dfss_gpio_0_open(uint32_t dir)
{
	return gpio_open(&gpio_context0, dir);
}

static int32_t dfss_gpio_0_close(void)
{
	return gpio_close(&gpio_context0);
}

static int32_t dfss_gpio_0_control(uint32_t ctrl_cmd, void *param)
{
	return gpio_control(&gpio_context0, ctrl_cmd, param);
}

static int32_t dfss_gpio_0_write(uint32_t val, uint32_t mask)
{
	return gpio_write(&gpio_context0, val, mask);
}

static int32_t dfss_gpio_0_read(uint32_t *val, uint32_t mask)
{
	return gpio_read(&gpio_context0, val, mask);
}

static void dfss_gpio_0_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_0;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_0.gpio_info);

	gpio_context0.info = &(gpio_0.gpio_info);

	info_ptr->gpio_ctrl = NULL;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = (void *)&gpio_context0;

	gpio->gpio_open = dfss_gpio_0_open;
	gpio->gpio_close = dfss_gpio_0_close;
	gpio->gpio_control = dfss_gpio_0_control;
	gpio->gpio_write = dfss_gpio_0_write;
	gpio->gpio_read = dfss_gpio_0_read;
}
#endif // USE_DFSS_GPIO_8B0

#if USE_DFSS_GPIO_8B1

static void gpio_int_cb1(uint32_t status);
static DEV_GPIO		gpio_1;
static DEV_GPIO_HANDLER gpio_int_handlers1[8];
static GPIO_DEV_CONTEXT gpio_context1 = {
	DFSS_GPIO_8B1_ID, 8, IO_GPIO_8B1_INT_INTR_FLAG,
	0, gpio_int_cb1, gpio_int_handlers1,0
};

static void gpio_int_cb1(uint32_t status)
{
	GPIO_DEV_CONTEXT *ctx = &gpio_context1;
	DEV_GPIO_INFO *info = ctx->info;
	uint32_t i;

	for (i = 0; i < ctx->width; i++) {
		if ((status & (1 << i)) && ctx->handlers[i]) {
			ctx->handlers[i](info);
		}
	}
}

static int32_t dfss_gpio_1_open(uint32_t dir)
{
	return gpio_open(&gpio_context1, dir);
}

static int32_t dfss_gpio_1_close(void)
{
	return gpio_close(&gpio_context1);
}

static int32_t dfss_gpio_1_control(uint32_t ctrl_cmd, void *param)
{
	return gpio_control(&gpio_context1, ctrl_cmd, param);
}

static int32_t dfss_gpio_1_write(uint32_t val, uint32_t mask)
{
	return gpio_write(&gpio_context1, val, mask);
}

static int32_t dfss_gpio_1_read(uint32_t *val, uint32_t mask)
{
	return gpio_read(&gpio_context1, val, mask);
}

static void dfss_gpio_1_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_1;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_1.gpio_info);

	gpio_context1.info = &(gpio_1.gpio_info);

	info_ptr->gpio_ctrl = NULL;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = (void *)&gpio_context1;

	gpio->gpio_open = dfss_gpio_1_open;
	gpio->gpio_close = dfss_gpio_1_close;
	gpio->gpio_control = dfss_gpio_1_control;
	gpio->gpio_write = dfss_gpio_1_write;
	gpio->gpio_read = dfss_gpio_1_read;
}
#endif // USE_DFSS_GPIO_8B1


#if USE_DFSS_GPIO_8B2

static void gpio_int_cb2(uint32_t status);
static DEV_GPIO		gpio_2;
static DEV_GPIO_HANDLER gpio_int_handlers2[8];
static GPIO_DEV_CONTEXT gpio_context2 = {
	DFSS_GPIO_8B2_ID, 8, IO_GPIO_8B2_INT_INTR_FLAG,
	0, gpio_int_cb2, gpio_int_handlers2,0
};

static void gpio_int_cb2(uint32_t status)
{
	GPIO_DEV_CONTEXT *ctx = &gpio_context2;
	DEV_GPIO_INFO *info = ctx->info;
	uint32_t i;

	for (i = 0; i < ctx->width; i++) {
		if ((status & (1 << i)) && ctx->handlers[i]) {
			ctx->handlers[i](info);
		}
	}
}

static int32_t dfss_gpio_2_open(uint32_t dir)
{
	return gpio_open(&gpio_context2, dir);
}

static int32_t dfss_gpio_2_close(void)
{
	return gpio_close(&gpio_context2);
}

static int32_t dfss_gpio_2_control(uint32_t ctrl_cmd, void *param)
{
	return gpio_control(&gpio_context2, ctrl_cmd, param);
}

static int32_t dfss_gpio_2_write(uint32_t val, uint32_t mask)
{
	return gpio_write(&gpio_context2, val, mask);
}

static int32_t dfss_gpio_2_read(uint32_t *val, uint32_t mask)
{
	return gpio_read(&gpio_context2, val, mask);
}

static void dfss_gpio_2_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_2;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_2.gpio_info);

	gpio_context2.info = &(gpio_2.gpio_info);

	info_ptr->gpio_ctrl = NULL;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = (void *)&gpio_context2;

	gpio->gpio_open = dfss_gpio_2_open;
	gpio->gpio_close = dfss_gpio_2_close;
	gpio->gpio_control = dfss_gpio_2_control;
	gpio->gpio_write = dfss_gpio_2_write;
	gpio->gpio_read = dfss_gpio_2_read;
}
#endif // USE_DFSS_GPIO_8B2

#if USE_DFSS_GPIO_8B3

static void gpio_int_cb3(uint32_t status);
static DEV_GPIO		gpio_3;
static DEV_GPIO_HANDLER gpio_int_handlers3[8];
static GPIO_DEV_CONTEXT gpio_context3 = {
	DFSS_GPIO_8B3_ID, 8, IO_GPIO_8B3_INT_INTR_FLAG,
	0, gpio_int_cb3, gpio_int_handlers3,0
};

static void gpio_int_cb3(uint32_t status)
{
	GPIO_DEV_CONTEXT *ctx = &gpio_context3;
	DEV_GPIO_INFO *info = ctx->info;
	uint32_t i;

	for (i = 0; i < ctx->width; i++) {
		if ((status & (1 << i)) && ctx->handlers[i]) {
			ctx->handlers[i](info);
		}
	}
}

static int32_t dfss_gpio_3_open(uint32_t dir)
{
	return gpio_open(&gpio_context3, dir);
}

static int32_t dfss_gpio_3_close(void)
{
	return gpio_close(&gpio_context3);
}

static int32_t dfss_gpio_3_control(uint32_t ctrl_cmd, void *param)
{
	return gpio_control(&gpio_context3, ctrl_cmd, param);
}

static int32_t dfss_gpio_3_write(uint32_t val, uint32_t mask)
{
	return gpio_write(&gpio_context3, val, mask);
}

static int32_t dfss_gpio_3_read(uint32_t *val, uint32_t mask)
{
	return gpio_read(&gpio_context3, val, mask);
}

static void dfss_gpio_3_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_3;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_3.gpio_info);

	gpio_context3.info = &(gpio_3.gpio_info);

	info_ptr->gpio_ctrl = NULL;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = (void *)&gpio_context3;

	gpio->gpio_open = dfss_gpio_3_open;
	gpio->gpio_close = dfss_gpio_3_close;
	gpio->gpio_control = dfss_gpio_3_control;
	gpio->gpio_write = dfss_gpio_3_write;
	gpio->gpio_read = dfss_gpio_3_read;
}
#endif // USE_DFSS_GPIO_8B3

#if USE_DFSS_GPIO_4B0

static void gpio_int_cb4(uint32_t status);
static DEV_GPIO		gpio_4;
static DEV_GPIO_HANDLER gpio_int_handlers4[4];
static GPIO_DEV_CONTEXT gpio_context4 = {
	DFSS_GPIO_4B0_ID, 4, IO_GPIO_4B0_INT_INTR_FLAG,
	0, gpio_int_cb4, gpio_int_handlers4,0
};

static void gpio_int_cb4(uint32_t status)
{
	GPIO_DEV_CONTEXT *ctx = &gpio_context4;
	DEV_GPIO_INFO *info = ctx->info;
	uint32_t i;

	for (i = 0; i < ctx->width; i++) {
		if ((status & (1 << i)) && ctx->handlers[i]) {
			ctx->handlers[i](info);
		}
	}
}

static int32_t dfss_gpio_4_open(uint32_t dir)
{
	return gpio_open(&gpio_context4, dir);
}

static int32_t dfss_gpio_4_close(void)
{
	return gpio_close(&gpio_context4);
}

static int32_t dfss_gpio_4_control(uint32_t ctrl_cmd, void *param)
{
	return gpio_control(&gpio_context4, ctrl_cmd, param);
}

static int32_t dfss_gpio_4_write(uint32_t val, uint32_t mask)
{
	return gpio_write(&gpio_context4, val, mask);
}

static int32_t dfss_gpio_4_read(uint32_t *val, uint32_t mask)
{
	return gpio_read(&gpio_context4, val, mask);
}

static void dfss_gpio_4_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_4;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_0.gpio_info);

	gpio_context4.info = &(gpio_0.gpio_info);

	info_ptr->gpio_ctrl = NULL;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = (void *)&gpio_context4;

	gpio->gpio_open = dfss_gpio_4_open;
	gpio->gpio_close = dfss_gpio_4_close;
	gpio->gpio_control = dfss_gpio_4_control;
	gpio->gpio_write = dfss_gpio_4_write;
	gpio->gpio_read = dfss_gpio_4_read;
}
#endif // USE_DFSS_GPIO_4B0


#if USE_DFSS_GPIO_4B1

static void gpio_int_cb5(uint32_t status);
static DEV_GPIO		gpio_5;
static DEV_GPIO_HANDLER gpio_int_handlers5[4];
static GPIO_DEV_CONTEXT gpio_context5 = {
	DFSS_GPIO_4B1_ID, 4, IO_GPIO_4B1_INT_INTR_FLAG,
	0, gpio_int_cb5, gpio_int_handlers5,0
};

static void gpio_int_cb5(uint32_t status)
{
	GPIO_DEV_CONTEXT *ctx = &gpio_context5;
	DEV_GPIO_INFO *info = ctx->info;
	uint32_t i;

	for (i = 0; i < ctx->width; i++) {
		if ((status & (1 << i)) && ctx->handlers[i]) {
			ctx->handlers[i](info);
		}
	}
}

static int32_t dfss_gpio_5_open(uint32_t dir)
{
	return gpio_open(&gpio_context5, dir);
}

static int32_t dfss_gpio_5_close(void)
{
	return gpio_close(&gpio_context5);
}

static int32_t dfss_gpio_5_control(uint32_t ctrl_cmd, void *param)
{
	return gpio_control(&gpio_context5, ctrl_cmd, param);
}

static int32_t dfss_gpio_5_write(uint32_t val, uint32_t mask)
{
	return gpio_write(&gpio_context5, val, mask);
}

static int32_t dfss_gpio_5_read(uint32_t *val, uint32_t mask)
{
	return gpio_read(&gpio_context5, val, mask);
}

static void dfss_gpio_5_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_5;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_5.gpio_info);

	gpio_context5.info = &(gpio_5.gpio_info);

	info_ptr->gpio_ctrl = NULL;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->direction = 0;
	info_ptr->extra = (void *)&gpio_context5;

	gpio->gpio_open = dfss_gpio_5_open;
	gpio->gpio_close = dfss_gpio_5_close;
	gpio->gpio_control = dfss_gpio_5_control;
	gpio->gpio_write = dfss_gpio_5_write;
	gpio->gpio_read = dfss_gpio_5_read;

}
#endif // USE_DFSS_GPIO_4B1
 
#if USE_DFSS_GPIO_4B2

static void gpio_int_cb6(uint32_t status);
static DEV_GPIO		gpio_6;
static DEV_GPIO_HANDLER gpio_int_handlers6[4];
static GPIO_DEV_CONTEXT gpio_context6 = {
	DFSS_GPIO_4B2_ID, 4, IO_GPIO_4B2_INT_INTR_FLAG,
	0, gpio_int_cb6, gpio_int_handlers6,0
};

static void gpio_int_cb6(uint32_t status)
{
	GPIO_DEV_CONTEXT *ctx = &gpio_context6;
	DEV_GPIO_INFO *info = ctx->info;
	uint32_t i;

	for (i = 0; i < ctx->width; i++) {
		if ((status & (1 << i)) && ctx->handlers[i]) {
			ctx->handlers[i](info);
		}
	}
}

static int32_t dfss_gpio_6_open(uint32_t dir)
{
	return gpio_open(&gpio_context6, dir);
}

static int32_t dfss_gpio_6_close(void)
{
	return gpio_close(&gpio_context6);
}

static int32_t dfss_gpio_6_control(uint32_t ctrl_cmd, void *param)
{
	return gpio_control(&gpio_context6, ctrl_cmd, param);
}

static int32_t dfss_gpio_6_write(uint32_t val, uint32_t mask)
{
	return gpio_write(&gpio_context6, val, mask);
}

static int32_t dfss_gpio_6_read(uint32_t *val, uint32_t mask)
{
	return gpio_read(&gpio_context6, val, mask);
}

static void dfss_gpio_6_install(void)
{
	DEV_GPIO_PTR gpio = &gpio_6;
	DEV_GPIO_INFO_PTR info_ptr = &(gpio_6.gpio_info);

	gpio_context6.info = &(gpio_6.gpio_info);

	info_ptr->gpio_ctrl = NULL;
	info_ptr->opn_cnt = 0;
	info_ptr->method = 0;
	info_ptr->extra = (void *)&gpio_context6;;

	gpio->gpio_open = dfss_gpio_6_open;
	gpio->gpio_close = dfss_gpio_6_close;
	gpio->gpio_control = dfss_gpio_6_control;
	gpio->gpio_write = dfss_gpio_6_write;
	gpio->gpio_read = dfss_gpio_6_read;
}
#endif // USE_DFSS_GPIO_4B2


void dfss_gpio_all_install(void);

DEV_GPIO_PTR gpio_get_dev(int32_t gpio_id)
{
	static uint32_t install_flag = 0;

	/* intall device objects */
	if (install_flag == 0) {
		install_flag = 1;
		dfss_gpio_all_install();
	}

	switch (gpio_id) {
#if (USE_DFSS_GPIO_8B0)
		case DFSS_GPIO_8B0_ID: return &gpio_0;
#endif
#if (USE_DFSS_GPIO_8B1)
		case DFSS_GPIO_8B1_ID: return &gpio_1;
#endif
#if (USE_DFSS_GPIO_8B2)
		case DFSS_GPIO_8B2_ID: return &gpio_2;
#endif
#if (USE_DFSS_GPIO_8B3)
		case DFSS_GPIO_8B3_ID: return &gpio_3;
#endif
#if (USE_DFSS_GPIO_4B0)
		case DFSS_GPIO_4B0_ID: return &gpio_4;
#endif
#if (USE_DFSS_GPIO_4B1)
		case DFSS_GPIO_4B1_ID: return &gpio_5;
#endif
#if (USE_DFSS_GPIO_4B2)
		case DFSS_GPIO_4B2_ID: return &gpio_6;
#endif
		default:
			break;
	}
	return NULL;
}

void dfss_gpio_all_install(void)
{
#if (USE_DFSS_GPIO_8B0)
	dfss_gpio_0_install();
#endif
#if (USE_DFSS_GPIO_8B1)
	dfss_gpio_1_install();
#endif
#if (USE_DFSS_GPIO_8B2)
	dfss_gpio_2_install();
#endif
#if (USE_DFSS_GPIO_8B3)
	dfss_gpio_3_install();
#endif
#if (USE_DFSS_GPIO_4B0)
	dfss_gpio_4_install();
#endif
#if (USE_DFSS_GPIO_4B1)
	dfss_gpio_5_install();
#endif
#if (USE_DFSS_GPIO_4B2)
	dfss_gpio_6_install();
#endif
}

/** @} end of group BOARD_EMSK_DRV_DFSS_GPIO_OBJ */