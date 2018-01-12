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
 * \defgroup	MICROCHIP_MRF24G_DEV_HAL	Mircochip MRF24G WIFI Device Port HAL
 * \ingroup	DEVICE_MICROCHIP
 * \brief	microchip mrf24g device port hal, declaration of
 * 	gpio operation, timer operation, spi operation used in wifi device
 *
 * @{
 *
 * \file	wf_dev_hal.h
 * \brief	microchip wifi device driver port related hal layer
 *
 */
#ifndef _WF_DEV_HAL_H
#define _WF_DEV_HAL_H

#include "dev_wnic.h"
#include "dev_gpio.h"
#include "dev_spi.h"
#include "embARC_error.h"

#define LOW_LEVEL			(0)
#define HIGH_LEVEL			(1)

#define WF_GPIO_INT_PIN_OFS		(0)
#define WF_GPIO_RST_PIN_OFS		(1)
#define WF_GPIO_WP_PIN_OFS		(2)
#define WF_GPIO_HIB_PIN_OFS		(3)

#define WF_GPIO_INT_PIN_MASK		(1<<WF_GPIO_INT_PIN_OFS)
#define WF_GPIO_RST_PIN_MASK		(1<<WF_GPIO_RST_PIN_OFS)
#define WF_GPIO_WP_PIN_MASK		(1<<WF_GPIO_WP_PIN_OFS)
#define WF_GPIO_HIB_PIN_MASK		(1<<WF_GPIO_HIB_PIN_OFS)

#define WF_GPIO_PIN_MASK_ALL		(WF_GPIO_INT_PIN_MASK |		\
						WF_GPIO_RST_PIN_MASK |	\
						WF_GPIO_WP_PIN_MASK |	\
						WF_GPIO_HIB_PIN_MASK)

#define WF_GPIO_MASK			(0x0f)

#define WF_EINT_ENABLE			(1)
#define WF_EINT_DISABLE			(0)

#define WF_CHIP_SELECTED		(1)
#define WF_CHIP_DESELECTED		(0)

#define WF_TIMER_FREQ_1K		(1000)

/** \brief mrf24wg device gpio related operation */
typedef struct wf_gpio_ops {
	int32_t (*gpio_open) (uint32_t pin_mask, uint32_t pin_dir);	/* open wifi related gpio, default all set to poll */
	int32_t (*gpio_close) (uint32_t pin_mask);	/*!< close wifi related gpio, include stop interrupts */
	int32_t (*gpio_read) (uint32_t pin_mask, uint32_t *value);	/*!< read gpio data with mask */
	int32_t (*gpio_write) (uint32_t pin_mask, uint32_t value);	/*!< write gpio data with mask */
	int32_t (*gpio_eint_install) (DEV_GPIO_HANDLER gpio_handler);	/* install but don't enable external interrupt */
	int32_t (*gpio_eint_is_enabled) (void);		/*!< poll external interrupt enable state */
	int32_t (*gpio_eint_control) (int32_t state);	/*!< control external interrupt, enable or disable */
} WF_GPIO_OPS, * WF_GPIO_OPS_PTR;

/** \brief mrf24wg device timer related operation */
typedef struct wf_timer_ops {
	int32_t (*timer_init) (uint32_t freq);	/*!< init timer with freq */
	uint32_t (*timer_read_ms) (void);	/*!< get timer count ms */
	void (*timer_delay) (uint32_t ms);	/*!< delay ms */
} WF_TIMER_OPS, * WF_TIMER_OPS_PTR;

typedef struct wf_spi_ops {
	int32_t (*spi_open) (uint32_t freq, uint32_t clk_mode);	/*!< open spi with this freq & clk_mode */
	int32_t (*spi_close) (void);				/*!< close spi */
	int32_t (*spi_cs) (int32_t cs);				/*!< spi chip select control */
	int32_t (*spi_transfer) (DEV_SPI_TRANSFER *xfer);	/*!< transfer data by spi */
	uint32_t working_freq;			/*!< spi working frequence */
	uint32_t clk_mode;			/*!< spi working clkmode */
} WF_SPI_OPS, * WF_SPI_OPS_PTR;

typedef struct wf_operations {
	WF_GPIO_OPS *gpio_ops;
	WF_TIMER_OPS *timer_ops;
	WF_SPI_OPS *spi_ops;
} WF_OPS, * WF_OPS_PTR;

#ifdef __cplusplus
extern "C" {
#endif

extern WF_OPS_PTR get_wf_ops(void);

#ifdef __cplusplus
}
#endif

#endif /* _WF_DEV_HAL_H */
/** @} */