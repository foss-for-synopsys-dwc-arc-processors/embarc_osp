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
 * SOFTWARE, EVEN IF ADVISED OF  THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "arc.h"
#include "arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "arc_exception.h"
#include "board.h"

#ifdef ENABLE_OS
#include "os_hal_inc.h"
#endif

#include "dev_gpio.h"
#include "dev_spi.h"
#include "mrf24j40.h"

#define DEBUG
#include "embARC_debug.h"

/* Enable CPU lock in SPI transmission */
#define EMSK_PMRF_0_SPI_CPULOCK_ENABLE

#define MRF_CHECK_EXP(EXPR, ERROR_CODE)			CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)
#define MRF_GPIO_SETUP(port_pin, port, pin)		{ \
		port = (port_pin & 0xffff0000) >>16; \
		pin = port_pin & 0xffff; \
	}
#define MRF_GPIO_PORT_SETUP(port_pin, port)		port = (port_pin & 0xffff0000) >>16
#define MRF_GPIO_PIN_SETUP(port_pin, pin)		pin = port_pin & 0xffff

static int32_t spi_check_obj(MRF24J40_DEF *mrf24j40)
{
	int32_t ercd = E_OK;
	MRF_CHECK_EXP(mrf24j40 != NULL, E_OBJ);
	DEV_SPI_PTR dev_spi_ptr = spi_get_dev(mrf24j40->spi);
	MRF_CHECK_EXP(dev_spi_ptr != NULL, E_OBJ);

error_exit:
	return ercd;
}

static int32_t gpio_check_obj(MRF24J40_DEF *mrf24j40)
{
	int32_t ercd = E_OK;
	int16_t port_reset, port_wake;
	MRF_GPIO_PORT_SETUP((mrf24j40->gpio_pin_reset), port_reset);
	MRF_GPIO_PORT_SETUP((mrf24j40->gpio_pin_wake), port_wake);
	MRF_CHECK_EXP(mrf24j40 != NULL, E_OBJ);
	MRF_CHECK_EXP(gpio_get_dev(port_reset) != NULL, E_OBJ);
	MRF_CHECK_EXP(gpio_get_dev(port_wake) != NULL, E_OBJ);
	/* No interrupt PIN (INT) check */

error_exit:
	return ercd;
}

static int32_t spi_read_long_ctrl_reg(MRF24J40_DEF *mrf24j40, uint16_t addr, uint8_t *val)
{
	int32_t ercd = E_OK;
	uint8_t msg[2];
	uint8_t ret_val = 0;
	int32_t ret0, ret1, ret2;
	DEV_SPI_TRANSFER dev_spi_xfer;
	uint32_t cpu_status;
	uint32_t cs_line = mrf24j40->spi_cs;

	DEV_SPI_PTR dev_spi_ptr = spi_get_dev(mrf24j40->spi);

	msg[0] = (uint8_t)(((addr >> 3) & 0x7F) | 0x80);
	msg[1] = (uint8_t)((addr << 5) & 0xE0);

	DEV_SPI_XFER_SET_TXBUF(&dev_spi_xfer, msg, 0, 2);
	DEV_SPI_XFER_SET_RXBUF(&dev_spi_xfer, &ret_val, 2, 1);
	DEV_SPI_XFER_SET_NEXT(&dev_spi_xfer, NULL);

#ifdef EMSK_PMRF_0_SPI_CPULOCK_ENABLE
	cpu_status = cpu_lock_save();
#endif
	ret0 = dev_spi_ptr->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(cs_line));
	ret1 = dev_spi_ptr->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&dev_spi_xfer));
	ret2 = dev_spi_ptr->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(cs_line));
#ifdef EMSK_PMRF_0_SPI_CPULOCK_ENABLE
	cpu_unlock_restore(cpu_status);
#endif

	if (!((ret0 == E_OK) && (ret1 == E_OK) && (ret2 == E_OK))) {
		ercd = E_SYS;
	} else {
		*val = ret_val;
	}

error_exit:
	return ercd;
}

static int32_t spi_read_short_ctrl_reg(MRF24J40_DEF *mrf24j40, uint16_t addr, uint8_t *val)
{
	int32_t ercd = E_OK;
	uint8_t msg;
	uint8_t ret_val = 0;
	int32_t ret0, ret1, ret2;
	DEV_SPI_TRANSFER dev_spi_xfer;
	uint32_t cpu_status;
	uint32_t cs_line = mrf24j40->spi_cs;

	DEV_SPI_PTR dev_spi_ptr = spi_get_dev(mrf24j40->spi);

	msg = (((addr << 1) & 0x7E) | 0);

	DEV_SPI_XFER_SET_TXBUF(&dev_spi_xfer, &msg, 0, 1);
	DEV_SPI_XFER_SET_RXBUF(&dev_spi_xfer, &ret_val, 1, 1);
	DEV_SPI_XFER_SET_NEXT(&dev_spi_xfer, NULL);

#ifdef EMSK_PMRF_0_SPI_CPULOCK_ENABLE
	cpu_status = cpu_lock_save();
#endif
	ret0 = dev_spi_ptr->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(cs_line));
	ret1 = dev_spi_ptr->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&dev_spi_xfer));
	ret2 = dev_spi_ptr->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(cs_line));
#ifdef EMSK_PMRF_0_SPI_CPULOCK_ENABLE
	cpu_unlock_restore(cpu_status);
#endif

	if (!((ret0 == E_OK) && (ret1 == E_OK) && (ret2 == E_OK))) {
		ercd = E_SYS;
	} else {
		ercd = E_OK;
		*val = ret_val;
	}

error_exit:
	return ercd;
}

static int32_t spi_write_long_ctrl_reg(MRF24J40_DEF *mrf24j40, uint16_t addr, uint8_t value)
{
	int32_t ercd = E_OK;
	uint8_t msg[4];
	DEV_SPI_TRANSFER dev_spi_xfer;
	int32_t ret0, ret1, ret2;
	uint32_t cpu_status;
	uint32_t cs_line = mrf24j40->spi_cs;

	DEV_SPI_PTR dev_spi_ptr = spi_get_dev(mrf24j40->spi);

	msg[0] = (uint8_t)(((addr >> 3) & 0x7F) | 0x80);
	msg[1] = (uint8_t)(((addr << 5) & 0xE0) | (1 << 4));
	msg[2] = value;
	msg[3] = 0x0;	/* have to write 1 more byte, why ?*/

	DEV_SPI_XFER_SET_TXBUF(&dev_spi_xfer, msg, 0, 4);
	DEV_SPI_XFER_SET_RXBUF(&dev_spi_xfer, NULL, 4, 0);
	DEV_SPI_XFER_SET_NEXT(&dev_spi_xfer, NULL);

#ifdef EMSK_PMRF_0_SPI_CPULOCK_ENABLE
	cpu_status = cpu_lock_save();
#endif
	ret0 = dev_spi_ptr->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(cs_line));
	ret1 = dev_spi_ptr->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&dev_spi_xfer));
	ret2 = dev_spi_ptr->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(cs_line));
#ifdef EMSK_PMRF_0_SPI_CPULOCK_ENABLE
	cpu_unlock_restore(cpu_status);
#endif

	if (!((ret0 == E_OK) && (ret1 == E_OK) && (ret2 == E_OK))) {
		ercd = E_SYS;
	}

error_exit:
	return ercd;
}

static int32_t spi_write_short_ctrl_reg(MRF24J40_DEF *mrf24j40, uint8_t addr, uint8_t value)
{
	int32_t ercd = E_OK;
	DEV_SPI_TRANSFER dev_spi_xfer;
	int32_t ret0, ret1, ret2;
	uint8_t msg[3];
	uint32_t cpu_status;
	uint32_t cs_line = mrf24j40->spi_cs;

	DEV_SPI_PTR dev_spi_ptr = spi_get_dev(mrf24j40->spi);

	msg[0] = (((addr << 1) & 0x7E) | 1);
	msg[1] = value;
	msg[2] = 0x0;	/* have to write 1 more byte, why ?*/

	DEV_SPI_XFER_SET_TXBUF(&dev_spi_xfer, msg, 0, 3);
	DEV_SPI_XFER_SET_RXBUF(&dev_spi_xfer, NULL, 3, 0);
	DEV_SPI_XFER_SET_NEXT(&dev_spi_xfer, NULL);

#ifdef EMSK_PMRF_0_SPI_CPULOCK_ENABLE
	cpu_status = cpu_lock_save();
#endif
	ret0 = dev_spi_ptr->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(cs_line));
	ret1 = dev_spi_ptr->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&dev_spi_xfer));
	ret2 = dev_spi_ptr->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(cs_line));
#ifdef EMSK_PMRF_0_SPI_CPULOCK_ENABLE
	cpu_unlock_restore(cpu_status);
#endif

	if (!((ret0 == E_OK) && (ret1 == E_OK) && (ret2 == E_OK))) {
		ercd = E_SYS;
	}

error_exit:
	return ercd;
}

static void mrf24j40_delay_us(uint32_t us)
{
	uint64_t start_us, us_delayed;

	us_delayed = (uint64_t)us;
	start_us = board_get_cur_us();

	while ((board_get_cur_us() - start_us) < us_delayed);
}

static void mrf24j40_delay_ms(uint32_t ms)
{
	board_delay_ms(ms, OSP_DELAY_OS_COMPAT_DISABLE);
}

/**
 * \brief	Hardware reset
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]	flag		GPIO direction { 1: high, 2: low }
 * \retval	E_OK	Control device successfully
 * \retval	E_SYS	Error in GPIO process
 */
static int32_t mrf24j40_hard_reset(MRF24J40_DEF *mrf24j40, uint8_t flag)
{
	int32_t ercd = E_OK;
	int32_t result = E_OK;
	uint16_t port_reset, pin_reset;

	MRF_GPIO_SETUP((mrf24j40->gpio_pin_reset), port_reset, pin_reset);
	DEV_GPIO_PTR dev_gpio_ptr = gpio_get_dev(port_reset);

	if (flag == 1) {
		result = dev_gpio_ptr->gpio_write((1 << pin_reset), (1 << pin_reset));
	} else {
		result = dev_gpio_ptr->gpio_write(0, (1 << pin_reset));
	}

	MRF_CHECK_EXP(result == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Read register in long address memory space
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]	addr		Register address
 * \param[out]	val		Point of value from control register
 * \retval	E_OK	Control device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	value point (val) is NULL
 * \retval	E_SYS	Error in SPI process
 */
int32_t mrf24j40_read_long_ctrl_reg(MRF24J40_DEF *mrf24j40, uint16_t addr, uint8_t *val)
{
	int32_t ercd = E_OK;
	uint8_t ret_val = 0;

	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP(val != NULL, E_PAR);

	ercd = spi_read_long_ctrl_reg(mrf24j40, addr, &ret_val);

	if (ercd == E_OK) {
		*val = ret_val;
	}

error_exit:
	return ercd;
}

/**
 * \brief	Read register in short address memory space
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]	addr		Register address
 * \param[out]	val		Point of value from control register
 * \retval	E_OK	Control device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	value point (val) is NULL
 * \retval	E_SYS	Error in SPI process
 */
int32_t mrf24j40_read_short_ctrl_reg(MRF24J40_DEF *mrf24j40, uint8_t addr, uint8_t *val)
{
	int32_t ercd = E_OK;
	uint8_t ret_val = 0;

	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP(val != NULL, E_PAR);

	ercd = spi_read_short_ctrl_reg(mrf24j40, addr, &ret_val);

	if (ercd == E_OK) {
		*val = ret_val;
	}

error_exit:
	return ercd;
}

/**
 * \brief	Write register in long address memory space
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]	addr		Register address
 * \param[in]	value		Value to control register
 * \retval	E_OK	Control device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_SYS	Error in SPI process
 */
int32_t mrf24j40_write_long_ctrl_reg(MRF24J40_DEF *mrf24j40, uint16_t addr, uint8_t value)
{
	int32_t ercd = E_OK;

	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	ercd = spi_write_long_ctrl_reg(mrf24j40, addr, value);

error_exit:
	return ercd;
}

/**
 * \brief	Write register in short address memory space
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]	addr		Register address
 * \param[in]	value		Value to control register
 * \retval	E_OK	Control device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_SYS	Error in SPI process
 */
int32_t mrf24j40_write_short_ctrl_reg(MRF24J40_DEF *mrf24j40, uint8_t addr, uint8_t value)
{
	int32_t ercd = E_OK;

	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	ercd = spi_write_short_ctrl_reg(mrf24j40, addr, value);

error_exit:
	return ercd;
}

/**
 * \brief	Set RXFLUSH in RXFLUSH: Receive FIFO Flush Register
 * \details	Reset RXFIFO address pointer to zero
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \retval	E_OK	Control device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_SYS	Error in SPI process
 */
int32_t mrf24j40_rxfifo_flush(MRF24J40_DEF *mrf24j40)
{
	int32_t ercd = E_OK;
	uint8_t ret_val = 0;
	int32_t result = E_OK;

	result = mrf24j40_read_short_ctrl_reg(mrf24j40, MRF24J40_RXFLUSH, &ret_val);
	MRF_CHECK_EXP(result == E_OK, E_SYS);
	result = mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_RXFLUSH, (ret_val | MRF24J40__RXFLUSH));
	MRF_CHECK_EXP(result == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Get FPSTAT in TXNCON: Transmit Normal FIFO Control Register
 * \details	Get status of the frame pending bit in the received Acknowledgement frame
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[out]	pend 		Point of frame pending bit { 1: set frame pending bit, 2: clear frame pending bit }
 * \retval	E_OK	Control device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	value point (pend) is NULL
 * \retval	E_SYS	Error in SPI process
 */
int32_t mrf24j40_get_pending_frame(MRF24J40_DEF *mrf24j40, uint8_t *pend)
{
	int32_t ercd = E_OK;
	uint8_t ret_val = 0;
	int32_t result = E_OK;

	MRF_CHECK_EXP(pend != NULL, E_PAR);
	result = mrf24j40_read_short_ctrl_reg(mrf24j40, MRF24J40_TXNCON, &ret_val);
	MRF_CHECK_EXP(result == E_OK, E_SYS);
	*pend = (ret_val >> 4) & 0x01;

error_exit:
	return ercd;
}

/**
 * \brief	Perform hardware Reset by asserting RESET pin
 * \details	The MRF24J40 will be released from Reset approximately 250 us after the RESET pin is released.
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \retval	E_OK	Control device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_SYS	Error in GPIO process
 */
int32_t mrf24j40_reset(MRF24J40_DEF *mrf24j40)
{
	int32_t ercd = E_OK;
	int32_t result = E_OK;

	MRF_CHECK_EXP(gpio_check_obj(mrf24j40) == E_OK, E_OBJ);
	result = mrf24j40_hard_reset(mrf24j40, 0);
	MRF_CHECK_EXP(result == E_OK, E_SYS);
	mrf24j40_delay_us(500);
	result = mrf24j40_hard_reset(mrf24j40, 1);
	MRF_CHECK_EXP(result == E_OK, E_SYS);
	mrf24j40_delay_us(500);

error_exit:
	return ercd;
}

static int32_t mrf24j40_wake_pin(MRF24J40_DEF *mrf24j40, uint8_t flag)
{
	int32_t ercd = E_OK;
	int32_t result = E_OK;
	uint16_t port_wake, pin_wake;

	MRF_GPIO_SETUP((mrf24j40->gpio_pin_wake), port_wake, pin_wake);
	DEV_GPIO_PTR dev_gpio_ptr = gpio_get_dev(port_wake);

	if (flag == 1) {
		result = dev_gpio_ptr->gpio_write((1 << pin_wake), (1 << pin_wake));
	} else {
		result = dev_gpio_ptr->gpio_write(0, (1 << pin_wake));
	}

	MRF_CHECK_EXP(result == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Perform hardware Reset by asserting RESET pin
 * \details	The MRF24J40 will be released from Reset approximately 250 us after the RESET pin is released.
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \retval	E_OK	Control device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_SYS	Error in GPIO process
 */
int32_t mrf24j40_sleep(MRF24J40_DEF *mrf24j40)
{
	int32_t ercd = E_OK;
	int32_t result = E_OK;
	uint8_t ret_val = 0;

	MRF_CHECK_EXP(gpio_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	result = mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_WAKECON, MRF24J40_IMMWAKE);
	MRF_CHECK_EXP(result == E_OK, E_SYS);
	result = mrf24j40_read_short_ctrl_reg(mrf24j40, MRF24J40_SLPACK, &ret_val);
	MRF_CHECK_EXP(result == E_OK, E_SYS);
	result = mrf24j40_wake_pin(mrf24j40, 1);
	MRF_CHECK_EXP(result == E_OK, E_SYS);
	// Set SOFTRST: Software Reset Register, reset power management circuitry (RSTPWR = 1)
	result = mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_SOFTRST, MRF24J40_RSTPWR);
	MRF_CHECK_EXP(result == E_OK, E_SYS);
	result = mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_SLPACK, ret_val | MRF24J40__SLPACK);
	MRF_CHECK_EXP(result == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Set RFCTL: RF Mode Control Register
 * \details	Perform RF Reset by setting RFRST = 1 and then RFRST = 0
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \retval	E_OK	Control device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_SYS	Error in GPIO process
 */
static int32_t mrf24j40_rf_reset(MRF24J40_DEF *mrf24j40)
{
	int32_t ercd = E_OK;
	int32_t result = E_OK;
	uint8_t retval = 0;

	MRF_CHECK_EXP(gpio_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	result = mrf24j40_read_short_ctrl_reg(mrf24j40, MRF24J40_RFCTL, &retval);
	MRF_CHECK_EXP(result == E_OK, E_SYS);
	result = mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_RFCTL, retval | MRF24J40_RFRST);
	MRF_CHECK_EXP(result == E_OK, E_SYS);
	mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_RFCTL, retval & ~MRF24J40_RFRST);
	MRF_CHECK_EXP(result == E_OK, E_SYS);
	mrf24j40_delay_ms(2);

error_exit:
	return ercd;
}

/**
 * \brief	Initialize MRF24J40
 */
void mrf24j40_initialize(MRF24J40_DEF *mrf24j40)
{
	uint8_t softrst_status;
	uint8_t rf_state;
	uint8_t rxmcr;

	//mrf24j40_cs_pin(1);
	//mrf24j40_wake_pin(1);
	mrf24j40_wake_pin(mrf24j40, 1);

	mrf24j40_reset(mrf24j40);
	mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_SOFTRST, (MRF24J40_RSTPWR | MRF24J40_RSTBB | MRF24J40_RSTMAC));
	mrf24j40_delay_us(500);

	do {
		mrf24j40_read_short_ctrl_reg(mrf24j40, MRF24J40_SOFTRST, &softrst_status);
	} while ((softrst_status & (MRF24J40_RSTPWR | MRF24J40_RSTBB | MRF24J40_RSTMAC)) != 0);

	mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_PACON2, MRF24J40_FIFOEN | MRF24J40_TXONTS(0x18));
	mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_TXSTBL, MRF24J40_RFSTBL(9) | MRF24J40_MSIFS(5));
	mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_RFCON0, MRF24J40_CHANNEL(0) | MRF24J40_RFOPT(0x03));
	mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_RFCON1, MRF24J40_VCOOPT(0x02)); // Initialize VCOOPT = 0x02
	mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_RFCON2, MRF24J40_PLLEN);
	mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_RFCON6, (MRF24J40_TXFIL | MRF24J40__20MRECVR));
	mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_RFCON7, MRF24J40_SLPCLKSEL(0x02));
	mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_RFCON8, MRF24J40_RFVCO);
	mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_SLPCON1, MRF24J40_SLPCLKDIV(1) | MRF24J40_CLKOUTDIS);

	mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_RXFLUSH, (MRF24J40_WAKEPAD | MRF24J40_WAKEPOL));

	mrf24j40_read_short_ctrl_reg(mrf24j40, MRF24J40_RXMCR, &rxmcr);
	mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_RXMCR, rxmcr & (~MRF24J40_NOACKRSP));
	// mrf24j40_write_short_ctrl_reg(MRF24J40_RXMCR, mrf24j40_read_short_ctrl_reg(MRF24J40_RXMCR) | MRF24J40_PANCOORD);
	// mrf24j40_write_short_ctrl_reg(MRF24J40_RXMCR, mrf24j40_read_short_ctrl_reg(MRF24J40_RXMCR) | (MRF24J40_NOACKRSP));
	// mrf24j40_write_short_ctrl_reg(MRF24J40_ACKTMOUT, mrf24j40_read_short_ctrl_reg(MRF24J40_ACKTMOUT) | 0x7f);
	mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_TXMCR, 0b00011100);
	// mrf24j40_set_coordinator();
	mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_ORDER, 0xFF);

	mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_BBREG1, 0x0);
	mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_BBREG2, MRF24J40_CCAMODE(0x02) | MRF24J40_CCASTH(0x00));
	mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_CCAEDTH, 0x60);
	mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_BBREG6, MRF24J40_RSSIMODE2);

	// TURNTIME Defaule value: 0x4 and TURNTIME=0x3
	mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_TXTIME, 0x30);
	//mrf24j40_write_short_ctrl_reg(MRF24J40_TXPEND, mrf24j40_read_short_ctrl_reg(MRF24J40_TXPEND) & MRF24J40_FPACK);
	//mrf24j40_write_long_ctrl_reg(MRF24J40_SLPCON0, MRF24J40_SLPCLKEN);
	// mrf24j40_write_short_ctrl_reg(MRF24J40_ACKTMOUT, mrf24j40_read_short_ctrl_reg(MRF24J40_ACKTMOUT) | MRF24J40_DRPACK);
	// mrf24j40_write_short_ctrl_reg(MRF24J40_RXMCR, mrf24j40_read_short_ctrl_reg(MRF24J40_RXMCR) | MRF24J40_PROMI);

	// Set INTCON: Interrupt Control Register
	// Enable TX Normal FIFO transmission interrupt (TXNIE = 0),
	// RX FIFO reception interrupt (RXIE = 0),
	// security key request interrupt (SECIE = 0)
	mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_INTCON, ~(MRF24J40_TXNIE | MRF24J40_RXIE | MRF24J40_SECIE));
	mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_RFCON3, 0x0);

	mrf24j40_rf_reset(mrf24j40);

	do {
		mrf24j40_read_long_ctrl_reg(mrf24j40, MRF24J40_RFSTATE, &rf_state);
	} while (((rf_state >> 5) & 0x05) != 0x05);

	mrf24j40_rxfifo_flush(mrf24j40);
}

/**
 * \brief	Setup MRF24J40 module
 * \details	Setup MRF24J40 structure and initialize MRF24J40 hardware
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_CLSED	GPIO is no open
 */
int32_t mrf24j40_setup(MRF24J40_DEF *mrf24j40)
{
	int32_t ercd = E_OK;
	DEV_GPIO_PTR gpio_wake_ptr;
	DEV_GPIO_PTR gpio_reset_ptr;
	int32_t result;
	/* No interrupt pin */
	uint16_t port_wake, port_reset, pin_wake, pin_reset;

	MRF_CHECK_EXP(gpio_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);

	MRF_GPIO_SETUP(mrf24j40->gpio_pin_wake, port_wake, pin_wake);
	MRF_GPIO_SETUP(mrf24j40->gpio_pin_reset, port_reset, pin_reset)
	gpio_wake_ptr = gpio_get_dev(port_wake);
	gpio_reset_ptr = gpio_get_dev(port_reset);

	result = gpio_wake_ptr->gpio_open(1 << pin_wake);
	MRF_CHECK_EXP(result == E_OPNED, E_CLSED);
	gpio_wake_ptr->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT,
	                            (void *)(1 << pin_wake));
	gpio_wake_ptr->gpio_control(GPIO_CMD_DIS_BIT_INT,
	                            (void *)(1 << pin_wake));

	result = gpio_reset_ptr->gpio_open(1 << pin_reset);
	MRF_CHECK_EXP(result == E_OPNED, E_CLSED);
	gpio_reset_ptr->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT,
	                             (void *)(1 << pin_reset));
	gpio_reset_ptr->gpio_control(GPIO_CMD_DIS_BIT_INT,
	                             (void *)(1 << pin_reset));

	/* Flush buffer */
	memset(&(mrf24j40->rx_buf[0]), 0, sizeof(uint8_t)*MRF24J40_BUF_SIZE);
	memset(&(mrf24j40->tx_buf[0]), 0, sizeof(uint8_t)*MRF24J40_BUF_SIZE);

error_exit:
	return ercd;
}

/**
 * \brief	Set short address in SADRL and SADRH
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]	addr 		Pointer of short address, addr[0] for low byte and addr[1] for high byte
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	addr point is NULL
 * \retval	E_SYS	Error in SPI/GPIO process
 */
int32_t mrf24j40_set_short_addr(MRF24J40_DEF *mrf24j40, uint8_t *addr)
{
	int32_t ercd = E_OK;

	MRF_CHECK_EXP(addr != NULL, E_PAR);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_SADRL, addr[0]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_SADRH, addr[1]) == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Set associated coordinator short address in ASSOSADR0 and ASSOSADR1
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]  *addr	Pointer of associated coordinator short address
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	addr point is NULL
 * \retval	E_SYS	Error in SPI/GPIO process
 */
int32_t mrf24j40_set_coordinator_short_addr(MRF24J40_DEF *mrf24j40, uint8_t *addr)
{
	int32_t ercd = E_OK;

	MRF_CHECK_EXP(addr != NULL, E_PAR);
	MRF_CHECK_EXP(mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_ASSOSADR0, addr[0]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_ASSOSADR1, addr[1]) == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Set associated coordinator extended address from ASSOEADR0 to ASSOEADR7
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]  *eui	Pointer of associated coordinator extended address, eui[0] to eui[7]
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	eui point is NULL
 * \retval	E_SYS	Error in SPI/GPIO process
 */
int32_t mrf24j40_set_coordinator_eui(MRF24J40_DEF *mrf24j40, uint8_t *eui)
{
	int32_t ercd = E_OK;

	MRF_CHECK_EXP(eui != NULL, E_PAR);
	MRF_CHECK_EXP(mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_ASSOEADR0, eui[0]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_ASSOEADR1, eui[1]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_ASSOEADR2, eui[2]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_ASSOEADR3, eui[3]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_ASSOEADR4, eui[4]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_ASSOEADR5, eui[5]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_ASSOEADR6, eui[6]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_ASSOEADR7, eui[7]) == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Set extended address from EADR0 to EADR7
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]  *eui	Pointer of extended address, eui[0] to eui[7]
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	eui point is NULL
 * \retval	E_SYS	Error in SPI/GPIO process
 */
int32_t mrf24j40_set_eui(MRF24J40_DEF *mrf24j40, uint8_t *eui)
{
	int32_t ercd = E_OK;

	MRF_CHECK_EXP(eui != NULL, E_PAR);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_EADR0, eui[0]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_EADR1, eui[1]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_EADR2, eui[2]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_EADR3, eui[3]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_EADR4, eui[4]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_EADR5, eui[5]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_EADR6, eui[6]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_EADR7, eui[7]) == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Set PAN ID in PANIDL and PANIDH
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]  *pan	Pointer of PAN ID, pan[0] for low byte and pan[1] for high byte
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	pan point is NULL
 * \retval	E_SYS	Error in SPI/GPIO process
 */
int32_t mrf24j40_set_pan(MRF24J40_DEF *mrf24j40, uint8_t *pan)
{
	int32_t ercd = E_OK;

	MRF_CHECK_EXP(pan != NULL, E_PAR);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_PANIDL, pan[0]) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_PANIDH, pan[1]) == E_OK, E_SYS);

error_exit:
	return ercd;
}

/* Need test */
static int32_t mrf24j40_set_key(MRF24J40_DEF *mrf24j40, uint16_t addr, uint8_t *key)
{
	int32_t ercd = E_OK;
	uint8_t msg[16+2];
	DEV_SPI_TRANSFER dev_spi_xfer;
	int32_t ret0, ret1, ret2;
	uint32_t cpu_status;
	uint8_t i;
	uint32_t cs_line = mrf24j40->spi_cs;

	DEV_SPI_PTR dev_spi_ptr = spi_get_dev(mrf24j40->spi);

	msg[0] = (uint8_t)(((addr >> 3) & 0x7F) | 0x80);
	msg[1] = (uint8_t)(((addr << 5) & 0xE0) | (1 << 4));

	for (i = 0; i < 16; i++) {
		msg[i+2] = key[0];
	}

	DEV_SPI_XFER_SET_TXBUF(&dev_spi_xfer, msg, 0, 18);
	DEV_SPI_XFER_SET_RXBUF(&dev_spi_xfer, NULL, 18, 0);
	DEV_SPI_XFER_SET_NEXT(&dev_spi_xfer, NULL);

#ifdef EMSK_PMRF_0_SPI_CPULOCK_ENABLE
	cpu_status = cpu_lock_save();
#endif
	ret0 = dev_spi_ptr->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(cs_line));
	ret1 = dev_spi_ptr->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&dev_spi_xfer));
	ret2 = dev_spi_ptr->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(cs_line));
#ifdef EMSK_PMRF_0_SPI_CPULOCK_ENABLE
	cpu_unlock_restore(cpu_status);
#endif

	if (!((ret0 == E_OK) && (ret1 == E_OK) && (ret2 == E_OK))) {
		ercd = E_SYS;
	}

error_exit:
	return ercd;
}

/**
 * \brief	Set key in address for SECKTXNFIFO
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]  *key		Pointer of security key (16 bytes)
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	key point is NULL
 * \retval	E_SYS	Error in SPI/GPIO process
 */
int32_t mrf24j40_set_tx_key(MRF24J40_DEF *mrf24j40, uint8_t *key)
{
	int32_t ercd = E_OK;

	MRF_CHECK_EXP(key != NULL, E_PAR);
	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP(mrf24j40_set_key(mrf24j40, MRF24J40_SECKTXNFIFO, key) == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Set key in address for SECKRXFIFO
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]  *key		Pointer of security key (16 bytes)
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	key point is NULL
 * \retval	E_SYS	Error in SPI/GPIO process
 */
int32_t mrf24j40_set_rx_key(MRF24J40_DEF *mrf24j40, uint8_t *key)
{
	int32_t ercd = E_OK;

	MRF_CHECK_EXP(key != NULL, E_PAR);
	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP(mrf24j40_set_key(mrf24j40, MRF24J40_SECKRXFIFO, key) == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Set CHANNEL in RFCON0: RF Control 0 Register
 * \details	Set channel number
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in] 	ch	Channel number, 0000 = Channel 11 (2405 MHz) as default
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_SYS	Error in SPI/GPIO process
 */
int32_t mrf24j40_set_channel(MRF24J40_DEF *mrf24j40, int16_t ch)
{
	int32_t ercd = E_OK;

	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP(mrf24j40_write_long_ctrl_reg(mrf24j40, MRF24J40_RFCON0,
	              MRF24J40_CHANNEL(ch) | MRF24J40_RFOPT(0x03)) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_rf_reset(mrf24j40) == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Set PROMI in RXMCR: Receive MAC Control Register
 * \details	Set promiscuous mode
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]	crc_check	1 = Discard packet when there is a MAC address mismatch,
 *				illegal frame type, dPAN/sPAN or MAC short address mismatch,
 * 				0 = Receive all packet types with good CRC
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	crc_check is not 0 or 1
 * \retval	E_SYS	Error in SPI/GPIO process
 */
int32_t mrf24j40_set_promiscuous(MRF24J40_DEF *mrf24j40, uint8_t crc_check)
{
	int32_t ercd = E_OK;
	uint8_t retval = 0;

	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP((crc_check == 0) || (crc_check == 1), E_OBJ);
	MRF_CHECK_EXP(mrf24j40_read_short_ctrl_reg(mrf24j40, MRF24J40_RXMCR, &retval) == E_OK, E_SYS);

	if (!crc_check) {
		retval |= MRF24J40_PROMI;
	} else {
		retval &= (~MRF24J40_PROMI);
	}

	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_RXMCR, retval) == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Set PANCOORD in RXMCR: Receive MAC Control Register
 * \details	Set device as PAN coordinator
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]	mark		1 = Set as PAN coordinator
 * 				0 = Not Set as PAN coordinator
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	mark is not 0 or 1
 * \retval	E_SYS	Error in SPI/GPIO process
 */
int32_t mrf24j40_set_coordinator(MRF24J40_DEF *mrf24j40, uint8_t mark)
{
	int32_t ercd = E_OK;
	uint8_t retval = 0;

	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP(mrf24j40_read_short_ctrl_reg(mrf24j40, MRF24J40_RXMCR, &retval) == E_OK, E_SYS);

	if (!mark) {
		retval |= MRF24J40_PANCOORD;
	} else {
		retval &= (~MRF24J40_PANCOORD);
	}

	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_RXMCR, retval) == E_OK, E_SYS);

error_exit:
	return ercd;

}

static int32_t mrf24j40_txpkt_frame_write(MRF24J40_DEF *mrf24j40, uint16_t address, uint8_t *frame, uint8_t hdr_len,
        uint8_t frame_len)
{
	int32_t ercd = E_OK;
	uint8_t msg[frame_len+2+2];
	DEV_SPI_TRANSFER dev_spi_xfer;
	int32_t ret0, ret1, ret2;
	uint32_t cpu_status;
	uint8_t i = 0;
	uint32_t cs_line = mrf24j40->spi_cs;

	DEV_SPI_PTR dev_spi_ptr = spi_get_dev(mrf24j40->spi);
	msg[0] = (uint8_t)(((address >> 3) & 0x7F) | 0x80);
	msg[1] = (uint8_t)(((address << 5) & 0xE0) | (1 << 4));
	msg[2] = (uint8_t)hdr_len;
	msg[3] = (uint8_t)frame_len;

	for (i = 0; i < frame_len; i++) {
		msg[i+4] = *frame;
		frame++;
	}

	DEV_SPI_XFER_SET_TXBUF(&dev_spi_xfer, msg, 0, frame_len+2+2);
	DEV_SPI_XFER_SET_RXBUF(&dev_spi_xfer, NULL, frame_len+2+2, 0);
	DEV_SPI_XFER_SET_NEXT(&dev_spi_xfer, NULL);

#ifdef EMSK_PMRF_0_SPI_CPULOCK_ENABLE
	cpu_status = cpu_lock_save();
#endif
	ret0 = dev_spi_ptr->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(cs_line));
	ret1 = dev_spi_ptr->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&dev_spi_xfer));
	ret2 = dev_spi_ptr->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(cs_line));
#ifdef EMSK_PMRF_0_SPI_CPULOCK_ENABLE
	cpu_unlock_restore(cpu_status);
#endif

	if (!((ret0 == E_OK) && (ret1 == E_OK) && (ret2 == E_OK))) {
		ercd = E_SYS;
	}

error_exit:
	return ercd;
}

/**
 * \brief	Enable and transmit frame in TX Normal FIFO from tx_buf
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	key point is NULL
 * \retval	E_SYS	Error in SPI/GPIO process
 *
 */
/* TX buffer packet */
//typedef struct mrf24j40_txbuf
//{				/* Name: byte */
//	uint8_t hlen;		/* Head length (m): 1 */
//	uint8_t flen;		/* Frame length (m+n): 1 */
//	uint8_t pck[128];	/* Header： m + Data payload: n */
//}MRF24J40_TXBUF;
int32_t mrf24j40_txpkt(MRF24J40_DEF *mrf24j40)
{
	int32_t ercd = E_OK;
	uint8_t retval = 0;
	uint8_t hdr_len = mrf24j40->rx_buf[0];
	uint8_t frame_len = mrf24j40->rx_buf[1];
	uint8_t *frame = &(mrf24j40->rx_buf[2]);

	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP(mrf24j40_read_short_ctrl_reg(mrf24j40, MRF24J40_TXNCON, &retval) == E_OK, E_SYS);
	retval &= ~(MRF24J40_TXNSECEN);

	if (IEEE_802_15_4_HAS_SEC(frame[0])) {
		retval |= MRF24J40_TXNSECEN;
	}

	if (IEEE_802_15_4_WANTS_ACK(frame[0])) {
		retval |= MRF24J40_TXNACKREQ;
	}

	MRF_CHECK_EXP(mrf24j40_txpkt_frame_write(mrf24j40, MRF24J40_TXNFIFO, frame, hdr_len, frame_len) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_TXNCON, retval | MRF24J40_TXNTRIG) == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Set RXCIPHER and TXNCIPHER in SECCON0: Security Control 0 Register
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]	rxcipher	RX FIFO security suite select bits
 * \param[in]	txcipher	TX normal FIFO security suite select bits
 * \details	111 = AES-CBC-MAC-32
 *		110 = AES-CBC-MAC-64
 *		101 = AES-CBC-MAC-128
 *		100 = AES-CMM-32
 *		011 = AES-CMM-64
 *		010 = AES-CMM-128
 *		001 = AES-CTR
 *		000 = None (default)
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_SYS	Error in SPI/GPIO process
 */
int32_t mrf24j40_set_cipher(MRF24J40_DEF *mrf24j40, uint8_t rxcipher, uint8_t txcipher)
{
	int32_t ercd = E_OK;

	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_SECCON0,
	              MRF24J40_RXCIPHER(rxcipher) | MRF24J40_TXNCIPHER(txcipher)) == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Read SECDECERR in RXSR: RX MAC Status Register
 * \details	Read security decryption error bit
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[out] 	err	0	Security decryption error occurred
			1	Security decryption error did not occur
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_SYS	Error in SPI/GPIO process
 */
// Need test
int32_t mrf24j40_rx_sec_fail(MRF24J40_DEF *mrf24j40, uint8_t *err)
{
	int32_t ercd = E_OK;
	uint8_t retval = 0;

	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP(mrf24j40_read_short_ctrl_reg(mrf24j40, MRF24J40_RXSR, &retval) == E_OK, E_SYS);
	*err = (retval >> 2) & 0x01;
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_RXSR, 0x00) == E_OK, E_SYS);

error_exit:
	return ercd;
}

/* Need test */
static int32_t mrf24j40_rxpkt_frame_write(MRF24J40_DEF *mrf24j40, uint8_t *buf, uint8_t length)
{
	int32_t ercd = E_OK;
	uint8_t msg[length];
	DEV_SPI_TRANSFER dev_spi_xfer;
	int32_t ret0, ret1, ret2;
	uint32_t cpu_status;
	uint32_t cs_line = mrf24j40->spi_cs;

	DEV_SPI_PTR dev_spi_ptr = spi_get_dev(mrf24j40->spi);

	msg[0] = (uint8_t)((((MRF24J40_RXFIFO + 1) >> 3) & 0x7F) | 0x80);
	msg[1] = (uint8_t)(((MRF24J40_RXFIFO + 1) << 5) & 0xE0);

	DEV_SPI_XFER_SET_TXBUF(&dev_spi_xfer, msg, 0, 2);
	DEV_SPI_XFER_SET_RXBUF(&dev_spi_xfer, buf, 2, length);
	DEV_SPI_XFER_SET_NEXT(&dev_spi_xfer, NULL);

#ifdef EMSK_PMRF_0_SPI_CPULOCK_ENABLE
	cpu_status = cpu_lock_save();
#endif
	ret0 = dev_spi_ptr->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(cs_line));
	ret1 = dev_spi_ptr->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&dev_spi_xfer));
	ret2 = dev_spi_ptr->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(cs_line));
#ifdef EMSK_PMRF_0_SPI_CPULOCK_ENABLE
	cpu_unlock_restore(cpu_status);
#endif

	if (!((ret0 == E_OK) && (ret1 == E_OK) && (ret2 == E_OK))) {
		ercd = E_SYS;
	}

error_exit:
	return ercd;
}

/**
 * \brief	Read RX packet in RX FIFO from rx_buf
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[out]  *plqi	Pointer of LQI in RX FIFO
 * \param[out]  *prssi	Pointer of RSSI in RX FIFO
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_PAR	Frame length is bigger than buffer size
 * \retval	E_SYS	Error in SPI/GPIO process
 *
 */
// RX buffer packet
// typedef struct mrf24j40_rxbuf
//{				 Name: byte
//	uint8_t flen;		 Frame length (m+n+2): 1
//	uint8_t pck[128];	 Header (MHR): m + Data payload (MSDU): n + FCS: 2
//	uint8_t lqi;		/* LQI: 1 */
//	uint8_t rssi;		/* RSSI: 1 */
// } MRF24J40_RXBUF;
int32_t mrf24j40_rxpkt(MRF24J40_DEF *mrf24j40, uint8_t *plqi, uint8_t *prssi)
{
	int32_t ercd = E_OK;
	uint8_t retval = 0;
	uint8_t *frame = &(mrf24j40->rx_buf[1]);

	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP(mrf24j40_read_short_ctrl_reg(mrf24j40, MRF24J40_BBREG1, &retval) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_BBREG1, retval | MRF24J40_RXDECINV) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_read_long_ctrl_reg(mrf24j40, MRF24J40_RXFIFO, &(mrf24j40->rx_buf[0])) == E_OK, E_SYS);

	MRF_CHECK_EXP((mrf24j40->rx_buf[0] + 3) <= MRF24J40_RXFIFO_SIZE, E_PAR)
	// {
	// 	*frame_len = MRF24J40_RXFIFO_SIZE - 3;
	// }

	MRF_CHECK_EXP(mrf24j40_rxpkt_frame_write(mrf24j40, frame, mrf24j40->rx_buf[0] + 2) == E_OK, E_SYS);

	if (plqi != NULL) {
		*plqi = mrf24j40->rx_buf[mrf24j40->rx_buf[0] + 1];
	}

	if (prssi != NULL) {
		*prssi = mrf24j40->rx_buf[mrf24j40->rx_buf[0] + 2];
	}

	MRF_CHECK_EXP(mrf24j40_rxfifo_flush(mrf24j40) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_read_short_ctrl_reg(mrf24j40, MRF24J40_BBREG1, &retval) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_BBREG1, retval & ~MRF24J40_RXDECINV) == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Set SECIGNORE/SECSTART in SECCON0: Security Control 0 Register
 * \details	Set RX security decryption ignore/start bit
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]	accept	1: start decryption process, 0: ignore decryption process
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_SYS	Error in SPI/GPIO process
 */
int32_t mrf24j40_set_decrypt_start(MRF24J40_DEF *mrf24j40, uint8_t accept)
{
	int32_t ercd = E_OK;
	uint8_t retval = 0;

	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP(mrf24j40_read_short_ctrl_reg(mrf24j40, MRF24J40_SECCON0, &retval) == E_OK, E_SYS);
	retval |= accept ? MRF24J40_SECSTART : MRF24J40_SECIGNORE;
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_SECCON0, retval) == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Write TX FIFO
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \param[in]	address		FIFO Address (TX Normal FIFO)
 * \param[in]	data		Pointer of data for TX FIFO
 * \param[in]	hdr_len		Header legnth for TX FIFO
 * \param[in]	len		Frame length for TX FIFO
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_SYS	Error in SPI/GPIO process
 */
int32_t mrf24j40_txfifo_write(MRF24J40_DEF *mrf24j40, uint16_t address, uint8_t *data, uint8_t hdr_len, uint8_t len)
{
	int32_t ercd = E_OK;

	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP(data != NULL, E_OBJ);
	MRF_CHECK_EXP(mrf24j40_txpkt_frame_write(mrf24j40, address, data, hdr_len, len) == E_OK, E_SYS);

error_exit:
	return ercd;
}

/**
 * \brief	Set TXNTRIG in TXNCON: Transmit normal FIFO control register
 * \details	Transmit frame in TX normal FIFO bit
 * \param[in]	mrf24j40	MRF24J40 object pointer
 * \retval	E_OK	Setup device successfully
 * \retval	E_OBJ	Device object is not valid or not exists
 * \retval	E_SYS	Error in SPI/GPIO process
 */
int32_t mrf24j40_set_transmit_trig(MRF24J40_DEF *mrf24j40)
{
	int32_t ercd = E_OK;
	uint8_t retval = 0;

	MRF_CHECK_EXP(spi_check_obj(mrf24j40) == E_OK, E_OBJ);
	MRF_CHECK_EXP(mrf24j40_read_short_ctrl_reg(mrf24j40, MRF24J40_TXNCON, &retval) == E_OK, E_SYS);
	MRF_CHECK_EXP(mrf24j40_write_short_ctrl_reg(mrf24j40, MRF24J40_TXNCON, retval| MRF24J40_TXNTRIG) == E_OK, E_SYS);

error_exit:
	return ercd;
}
