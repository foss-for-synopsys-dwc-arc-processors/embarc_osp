/*
 * Contiki SeedEye Platform project
 *
 * Copyright (c) 2012,
 *  Scuola Superiore Sant'Anna (http://www.sssup.it) and
 *  Consorzio Nazionale Interuniversitario per le Telecomunicazioni
 *  (http://www.cnit.it).
 *
 * Copyright (c) 2017, Synopsys, Inc.
 * Modified for port to ARC processor by Wayne Ren wei.ren@synopsys.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * \file   mrf24j40.c
 * \brief  MRF24J40 Driver based on the driver in seedeye
 */

#include "contiki.h"

#include "mrf24j40.h"

#include "dev_spi.h"
#include "dw_spi.h"

#include "dev_gpio.h"

#include "net/packetbuf.h"
#include "net/netstack.h"

#define DBG_LESS
#include "embARC_debug.h"

/* Read/Write SPI Commands for Short and Long Address registers. */
#define MRF24J40_READSHORT(reg) ((reg) << 1)
#define MRF24J40_WRITESHORT(reg) ((reg) << 1 | 1)
#define MRF24J40_READLONG(reg) (1 << 15 | (reg) << 5)
#define MRF24J40_WRITELONG(reg) (1 << 15 | (reg) << 5 | 1 << 4)

PROCESS(mrf24j40_process, "MRF24J40 driver");

static volatile uint8_t mrf24j40_last_lqi;
static volatile uint8_t mrf24j40_last_rssi;
static volatile uint8_t status_tx;
static volatile uint8_t receive_on;
static DEV_SPI_PTR spi;
static DEV_GPIO_PTR ctrl_port;
static uint32_t cs = MRF24J40_CS_LINE;
static void flush_rx_fifo(void);


/**
 * \brief read short memory
 *
 * \param[in] addr memory address
 * \return memory value
 */
static uint8_t read_short_mem(uint8_t addr)
{
	uint8_t msg;
	uint8_t ret_val;
	DEV_SPI_TRANSFER cmd_xfer;
	unsigned int cpu_status;

	msg = MRF24J40_READSHORT(addr) & 0x7f;

	DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, &msg, 0, 1);
	DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, &ret_val, 1, 1);
	DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

	cpu_status = cpu_lock_save();
	spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(cs));
	spi->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&cmd_xfer));
	spi->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(cs));
	cpu_unlock_restore(cpu_status);

	return ret_val;
}

/**
 * \brief read long memory
 *
 * \param[in] addr memory address
 * \return memory value
 */
static uint8_t read_long_mem(uint16_t addr)
{
	uint16_t cmd;
	uint8_t msg[2];
	uint8_t ret_val;
	DEV_SPI_TRANSFER cmd_xfer;
	unsigned int cpu_status;

	cmd = MRF24J40_READLONG(addr);
	msg[0] = cmd >> 8 & 0xff;
	msg[1] = cmd & 0xff;

	DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, msg, 0, 2);
	DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, &ret_val, 2, 1);
	DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

	cpu_status = cpu_lock_save();
	spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(cs));
	spi->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&cmd_xfer));
	spi->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(cs));
	cpu_unlock_restore(cpu_status);

	return ret_val;
}

/**
 * \brief write short memory
 *
 * \param[in] addr memory address
 * \param[in] val value to write
 */
static void write_short_mem(uint8_t addr, uint8_t val)
{
	DEV_SPI_TRANSFER cmd_xfer;
	uint8_t msg[3];
	unsigned int cpu_status;

	msg[0] = MRF24J40_WRITESHORT(addr);
	msg[1] = val;
	msg[2] = 0x0;	/* have to write 1 more byte, why ?*/

	DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, msg, 0, 3);
	DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 3, 0);
	DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

	cpu_status = cpu_lock_save();
	spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(cs));
	spi->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&cmd_xfer));
	spi->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(cs));
	cpu_unlock_restore(cpu_status);
}

/**
 * \brief write long memory
 *
 * \param[in] addr memory address
 * \param[in] val value to write
 */
static void write_long_mem(uint16_t addr, uint8_t val)
{
	uint8_t msg[4];
	uint16_t cmd;
	DEV_SPI_TRANSFER cmd_xfer;
	unsigned int cpu_status;

	cmd = MRF24J40_WRITELONG(addr);
	msg[0] = cmd >> 8 & 0xff;
	msg[1] = cmd & 0xff;
	msg[2] = val;
	msg[3] = 0x0;	/* have to write 1 more byte, why ?*/

	DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, msg, 0, 4);
	DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 4, 0);
	DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

	cpu_status = cpu_lock_save();
	spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(cs));
	spi->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&cmd_xfer));
	spi->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(cs));
	cpu_unlock_restore(cpu_status);

}

/**
 * \brief flush rx fifo
 */
Inline void flush_rx_fifo(void)
{
	write_short_mem(MRF24J40_RXFLUSH, read_short_mem(MRF24J40_RXFLUSH) | 0b00000101);
}


/**
 * \brief write data into TX_FIFO
 *
 * \param[in] addr memory address
 * \param[in] data data to write
 * \param[in] len data length
 * \return 0 succeeded -1 failed
 */
static int32_t write_tx_buf(uint16_t addr, const void *data, uint8_t len)
{
	uint16_t cmd;
	uint8_t msg[4];
	DEV_SPI_TRANSFER cmd_xfer;
	DEV_SPI_TRANSFER data_xfer;
	unsigned int cpu_status;


	/* check param */
	if((len == 0) || (len > PACKETBUF_SIZE)) {
		return -1;
	}

	cmd = MRF24J40_WRITELONG(addr);
	msg[0] = cmd >> 8 & 0xff;
	msg[1] = cmd & 0xff;
	msg[2] = 0;
	msg[3] = len;

	DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, msg, 0, 4);
	DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, NULL, 4, 0);
	DEV_SPI_XFER_SET_NEXT(&cmd_xfer, &data_xfer);

	DEV_SPI_XFER_SET_TXBUF(&data_xfer, data, 0, len);
	DEV_SPI_XFER_SET_RXBUF(&data_xfer, NULL, len, 0);
	DEV_SPI_XFER_SET_NEXT(&data_xfer, NULL);

	cpu_status = cpu_lock_save();
	spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(cs));
	spi->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&cmd_xfer));
	spi->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(cs));
	cpu_unlock_restore(cpu_status);

	return 0;
}

/**
 * \brief read data from RX_FIFO
 *
 * \param[out] data data to write
 * \param[in] len data length to read
 * \return >0 read data length, -1 failed
 */
static int32_t read_rx_buf(void *data, uint8_t len)
{
	uint16_t cmd;
	uint8_t buf_len;
	uint8_t msg[2];
	unsigned int cpu_status;
	DEV_SPI_TRANSFER cmd_xfer;

	/* Disable packet reception */
	write_short_mem(MRF24J40_BBREG1, 0b00000100);

	/* received packet = 1 byte length + dataload + 2 bytes FCS + 1 byte LQI + 1 byte RSSI */
	/* buf_len = packet length - 2 */
	buf_len = read_long_mem(MRF24J40_RX_FIFO) - 2;

	if (buf_len > (MRF24J40_RX_FIFO_SIZE - 5) ) {
		/* packet is too large */
		len = 0;
	}
	if (buf_len <= len) {

		cmd = MRF24J40_READLONG(MRF24J40_RX_FIFO + 1);

		msg[0] = cmd >> 8 & 0xff;
		msg[1] = cmd & 0xff;

		DEV_SPI_XFER_SET_TXBUF(&cmd_xfer, msg, 0, 2);
		DEV_SPI_XFER_SET_RXBUF(&cmd_xfer, data, 2, buf_len);
		DEV_SPI_XFER_SET_NEXT(&cmd_xfer, NULL);

		cpu_status = cpu_lock_save();
		spi->spi_control(SPI_CMD_MST_SEL_DEV, CONV2VOID(cs));
		spi->spi_control(SPI_CMD_TRANSFER_POLLING, CONV2VOID(&cmd_xfer));
		spi->spi_control(SPI_CMD_MST_DSEL_DEV, CONV2VOID(cs));
		cpu_unlock_restore(cpu_status);

//#ifdef ADD_RSSI_AND_LQI_TO_PACKET
		mrf24j40_last_lqi = read_long_mem(MRF24J40_RX_FIFO + buf_len + 3);
		mrf24j40_last_rssi = read_long_mem(MRF24J40_RX_FIFO + buf_len + 4);
//#endif
		len = buf_len;
	} else {
		len = 0;
	}

	/* Enable packet reception */
	write_short_mem(MRF24J40_BBREG1, 0b00000000);

//#ifdef MRF24J40_PROMISCUOUS_MODE
	/*
	 * Flush RX FIFO as suggested by the work around 1 in
	 * MRF24J40 Silicon Errata.
	 */
	flush_rx_fifo();
//#endif

	return len == 0 ? -1 : len;
}

/**
 * \brief reset receive state machine
 */
static void reset_rf_state_machine(void)
{
	/*
	 * Reset RF state machine
	 */
	const uint8_t rfctl = read_short_mem(MRF24J40_RFCTL);

	write_short_mem(MRF24J40_RFCTL, rfctl | 0b00000100);
	write_short_mem(MRF24J40_RFCTL, rfctl & 0b11111011);

	clock_wait(10);
}

/**
 * \brief Start sleep
 *
 *        This routine puts the radio in sleep mode.
 */
static void put_to_sleep(void)
{
	/* Prepare WAKE pin: */
	ctrl_port->gpio_write(MRF24J40_WAKE_OFF, MRF24J40_WAKE_PIN);
	/* Enable Immediate Wake-up mode */
	write_short_mem(MRF24J40_WAKECON, 0b10000000);
	write_short_mem(MRF24J40_SOFTRST, 0b00000100);
	write_short_mem(MRF24J40_RXFLUSH, 0b01100100);
    /* Put to sleep */
	write_short_mem(MRF24J40_SLPACK, 0b10000000);
}

/**
 * \brief wake up the radio
 *
 *        This routine turns on and sets the radio on receiving mode.
 *        Note: After performing this routine the radio is in the receiving state.
 */
static void wake(void)
{
	/* Wake-up  */
	ctrl_port->gpio_write(MRF24J40_WAKE_ON, MRF24J40_WAKE_PIN);
	/* RF State Machine reset */
	write_short_mem(MRF24J40_RFCTL, 0b00000100);
	write_short_mem(MRF24J40_RFCTL, 0b00000000);

	clock_wait(10);
}

/**
 * \brief isr routine of mrf24j40
 * \param[in] ptr pointer transferred frome interrupt entry
 */
static void mrf24j40_isr(void *ptr)
{
	INT_STATUS int_status;
   	TX_STATUS tx_status;


	int_status.val = read_short_mem(MRF24J40_INTSTAT);

	if (!int_status.val) {
		int_status.bits.RXIF = 1; /* a trick to avoid problem */
	}

	if (int_status.bits.RXIF) {
		/* a frame received */
		process_poll(&mrf24j40_process);
	}

	if (int_status.bits.TXNIF) {
		/* a frame is tranmitted */
		tx_status.val = read_short_mem(MRF24J40_TXSTAT);

		if(tx_status.bits.TXNSTAT) {
			if(tx_status.bits.CCAFAIL) {
			status_tx = MRF24J40_TX_ERR_COLLISION;
		} else {
			status_tx = MRF24J40_TX_ERR_MAXRETRY;
		}
		} else {
			status_tx = MRF24J40_TX_ERR_NONE;
		}
	}
}

/**
 * \brief set the rx/tx channel
 */
void mrf24j40_write_channel(uint16_t ch)
{
	write_long_mem(MRF24J40_RFCON0, ((ch - 11) << 4) | 0b00000011);

	reset_rf_state_machine();
}

/**
 * \brief Store MAC PAN ID
 *
 *        This routine sets the MAC PAN ID in the MRF24J40.
 */
void mrf24j40_write_panid(uint16_t id)
{
	write_short_mem(MRF24J40_PANIDL, (uint8_t)id);
	write_short_mem(MRF24J40_PANIDH, (uint8_t)(id >> 8));
}

/**
 * \brief Store short MAC address
 *
 *        This routine sets the short MAC address in the MRF24J40.
 */
void mrf24j40_write_short_mac_addr(uint16_t addr)
{
	write_short_mem(MRF24J40_SADRL, (uint8_t)(addr >> 8));
	write_short_mem(MRF24J40_SADRH, (uint8_t)addr);
}

/**
 * \brief Store extended MAC address
 *
 *        This routine sets the extended MAC address in the MRF24J40.
 */
void mrf24j40_write_extended_mac_addr(uint64_t addr)
{
	write_short_mem(MRF24J40_EADR7, (uint8_t)addr);
	write_short_mem(MRF24J40_EADR6, (uint8_t)(addr >> 8));
	write_short_mem(MRF24J40_EADR5, (uint8_t)(addr >> 16));
	write_short_mem(MRF24J40_EADR4, (uint8_t)(addr >> 24));
	write_short_mem(MRF24J40_EADR3, (uint8_t)(addr >> 32));
	write_short_mem(MRF24J40_EADR2, (uint8_t)(addr >> 40));
	write_short_mem(MRF24J40_EADR1, (uint8_t)(addr >> 48));
	write_short_mem(MRF24J40_EADR0, (uint8_t)(addr >> 56));
}

/**
 * \brief Get short MAC address
 *
 *        This routine gets the short MAC address stored in the MRF24J40.
 */
void mrf24j40_read_short_mac_addr(uint16_t *addr)
{
	*(((uint8_t *)& addr)) = read_short_mem(MRF24J40_SADRL);
	*(((uint8_t *)& addr) + 1) = read_short_mem(MRF24J40_SADRH);
}

/**
 * \brief Gets extended MAC address
 *
 *        This routine gets the extended MAC address stored in the MRF24J40.
 */
void mrf24j40_read_extended_mac_addr(uint64_t *addr)
{
	*(((uint8_t *)& addr)) = read_short_mem(MRF24J40_EADR7);
	*(((uint8_t *)& addr) + 1) = read_short_mem(MRF24J40_EADR6);
	*(((uint8_t *)& addr) + 2) = read_short_mem(MRF24J40_EADR5);
	*(((uint8_t *)& addr) + 3) = read_short_mem(MRF24J40_EADR4);
   	*(((uint8_t *)& addr) + 4) = read_short_mem(MRF24J40_EADR3);
	*(((uint8_t *)& addr) + 5) = read_short_mem(MRF24J40_EADR2);
	*(((uint8_t *)& addr) + 6) = read_short_mem(MRF24J40_EADR1);
	*(((uint8_t *)& addr) + 7) = read_short_mem(MRF24J40_EADR0);
}

/**
 * \brief Set TX power
 *
 *        This routine sets the transmission power of the MRF24J40.
 */
void mrf24j40_write_tx_power(uint8_t pwr)
{
	write_long_mem(MRF24J40_RFCON3, pwr);
}

/**
 * \brief Get radio status
 *
 *        This routine returns the MRF24J40 status.
 */
uint8_t mrf24j40_read_status(void)
{
	return read_long_mem(MRF24J40_RFSTATE);
}


/**
 * \brief Get the RSSI
 *
 *        This routine returns the rssi value mesured in dbm
 *        Note: to convert the returned value to dBm, use the table 3-8 available
 *        in the MRF24J40 datasheet.
 */
uint8_t mrf24j40_read_rssi(void)
{
	/*
	* 3.6.1 RSSI FIRMWARE REQUEST (RSSI MODE1)
	* In this mode, the host microcontroller sends a request
	* to calculate RSSI, then waits until it is done and then
	* reads the RSSI value. The steps are:
	*
	*  1.
	*    Set RSSIMODE1 0x3E<7> - Initiate RSSI
	*    calculation.
	*
	*  2.
	*    Wait until RSSIRDY 0x3E<0> is set to '1' - RSSI
	*    calculation is complete.
	*
	*  3.
	*    Read RSSI 0x210<7:0> - The RSSI register
	*    contains the averaged RSSI received power
	*    level for 8 symbol periods.
	*/

	/* Initiate RSSI calculation */
	write_short_mem(MRF24J40_BBREG6, read_short_mem(MRF24J40_BBREG6) | 0b10000000);

	/* Wait until RSSI calculation is done */
	while(!(read_short_mem(MRF24J40_BBREG6) & 0b00000001));

	mrf24j40_last_rssi = read_long_mem(MRF24J40_RSSI);

	return mrf24j40_last_rssi;
}

/**
 * \brief Get the last read RSSI
 *
 *        This routine returns the last rssi value mesured in dbm
 *        Note: to convert the returned value to dBm, use the table 3-8 available
 *        in the MRF24J40 datasheet.
 */
uint8_t mrf24j40_read_last_rssi(void)
{
	return mrf24j40_last_rssi;
}

/**
 * \brief Get the last read LQI
 *
 *        This routine returns the last lqi
 */
uint8_t mrf24j40_read_last_lqi(void)
{
	return mrf24j40_last_lqi;
}

/**
 * \brief turn on mrf24j40
 */
int mrf24j40_on(void)
{
#if 1
	if(!receive_on) {
		wake();
		receive_on = 1;
	}
#endif
	return 1;
}

/**
 * \brief turn off mrf24j40
 */
int mrf24j40_off(void)
{
#if 1
	if(receive_on) {
		receive_on = 0;
		put_to_sleep();
	}
#endif
	return 1;
}

/**
 * \brief init the raido transceiver
 */
int mrf24j40_init(void)
{
	int32_t ercd;

	uint8_t i;
	uint32_t temp;
	DEV_GPIO_BIT_ISR isr;
	DEV_GPIO_INT_CFG int_cfg;

	/* Set the IO pins direction */
	/* PortA must be opened before mrf24j40 */
	spi = spi_get_dev(DW_SPI_0_ID);
	ercd = spi->spi_open(DEV_MASTER_MODE, MRF24J40_SPI_FREQ);
	if ((ercd != E_OK) && (ercd != E_OPNED)) {
		DBG("spi open error\r\n");
		return -1;
	}
	spi->spi_control(SPI_CMD_SET_CLK_MODE, CONV2VOID(MRF24J40_SPI_CLKMODE));

	ctrl_port = gpio_get_dev(DW_GPIO_PORT_A);
	/* wakepin:output, rstpin: output, INT_PIN:input, interrupt */
	ercd = ctrl_port->gpio_open(MRF24J40_WAKE_PIN | MRF24J40_RST_PIN);

	if ((ercd != E_OK) && (ercd != E_OPNED)) {
		DBG("MRF24j40 ctrl port open error");
		return -1;
	}
	if (ercd == E_OPNED) {
		temp = MRF24J40_WAKE_PIN | MRF24J40_RST_PIN;
		ctrl_port->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, (void *)temp);
		temp = MRF24J40_INT_PIN;
		ctrl_port->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT, (void *)temp);
	}

	/* interrupt configuration */
	ctrl_port->gpio_control(GPIO_CMD_DIS_BIT_INT, (void *)MRF24J40_INT_PIN);

	temp = MRF24J40_INT_PIN;
	int_cfg.int_bit_mask = temp;
	int_cfg.int_bit_type = GPIO_INT_BITS_EDGE_TRIG(temp);
	int_cfg.int_bit_polarity = GPIO_INT_BITS_POL_FALL_EDGE(temp);
	int_cfg.int_bit_debounce = GPIO_INT_BITS_DIS_DEBOUNCE(temp);
	ercd = ctrl_port->gpio_control(GPIO_CMD_SET_BIT_INT_CFG, (void *)(&int_cfg));

	isr.int_bit_ofs = MRF24J40_INT_PIN_OFS;
	isr.int_bit_handler = mrf24j40_isr;
	ctrl_port->gpio_control(GPIO_CMD_SET_BIT_ISR, (void *)(&isr));

	/* interrupt for mrf24j40 is enabled at the end of the init process */
	DBG("MRF24J40 Initialization started\r\n");

	ctrl_port->gpio_write(MRF24J40_WAKE_OFF, MRF24J40_WAKE_PIN);
	ctrl_port->gpio_write(MRF24J40_RST_LOW, MRF24J40_RST_PIN);
	clock_wait(10);

	ctrl_port->gpio_write(MRF24J40_RST_HIGH, MRF24J40_RST_PIN);

	clock_wait(10);

	/*
	 * bit 7:3 reserved: Maintain as '0'
	 * bit 2   RSTPWR: Power Management Reset bit
	 *         1 = Reset power management circuitry (bit is automatically cleared to '0' by hardware)
	 * bit 1   RSTBB: Baseband Reset bit
	 *         1 = Reset baseband circuitry (bit is automatically cleared to '0' by hardware)
	 * bit 0   RSTMAC: MAC Reset bit
	 *         1 = Reset MAC circuitry (bit is automatically cleared to '0' by hardware)
	 */
	//mrf24j40_dump();
	//while(1);
	write_short_mem(MRF24J40_SOFTRST, 0b00000111);

	/*
	 * wait until the radio reset is completed
	 */
	do {
		i = read_short_mem(MRF24J40_SOFTRST);
	} while((i & 0b0000111) != 0);
	/*
	 * bit 7   FIFOEN: FIFO Enable bit 1 = Enabled (default). Always maintain this bit as a '1'.
	 * bit 6   reserved: Maintain as '0'
	 * bit 5:2 TXONTS<3:0>: Transmitter Enable On Time Symbol bits(1)
	 *         Transmitter on time before beginning of packet. Units: symbol period (16 us).
	 *         Minimum value: 0x1. Default value: 0x2 (2 * 16 us = 32 us). Recommended value: 0x6 (6 * 16 us = 96 us).
	 * bit 1:0 TXONT<8:7>: Transmitter Enable On Time Tick bits(1)
	 *         Transmitter on time before beginning of packet. TXONT is a 9-bit value. TXONT<6:0> bits are located
	 *         in SYMTICKH<7:1>. Units: tick (50 ns). Default value = 0x028 (40 * 50 ns = 2 us).
	 */
	write_short_mem(MRF24J40_PACON2, 0x98);
	mrf24j40_write_channel(MRF24J40_DEFAULT_CHANNEL);
	/*
	 * Set TX turn around time as defined by IEEE802.15.4 standard
	 */
	write_long_mem(MRF24J40_RFCON1, 0x01);      /* program the RF and Baseband Register */
                                                /* as suggested by the datasheet */
	write_long_mem(MRF24J40_RFCON2, 0x80);      /* enable PLL */
	write_long_mem(MRF24J40_RFCON3, 0x0);	/* 0db */
	/*
	 * Set up
	 *
	 * bit 7   '1' as suggested by the datasheet
	 * bit 6:5 '00' reserved
	 * bit 4   '1' recovery from sleep 1 usec
	 * bit 3   '0' battery monitor disabled
	 * bit 2:0 '000' reserved
	 */
	write_long_mem(MRF24J40_RFCON6, 0x90);
	write_long_mem(MRF24J40_RFCON7, 0x80);      /* Sleep clock = 100kHz */
	write_long_mem(MRF24J40_RFCON8, 0x10);      /* as suggested by the datasheet */

	write_long_mem(MRF24J40_SLPCON1, 0x21);     /* as suggested by the datasheet */

	/* Program CCA, RSSI threshold values */
	/* Enable packet reception */
	write_short_mem(MRF24J40_BBREG1, 0b00000000);
	write_short_mem(MRF24J40_BBREG2, 0x80);     /* Recommended value by the datashet */
	write_short_mem(MRF24J40_CCAEDTH, 0x60);    /* Recommended value by the datashet */

	/* Enable the packet RSSI */
	write_short_mem(MRF24J40_BBREG6, 0x40);
	DBG("MRF24J40 Init append RSSI and LQI to packet\r\n");

	/*
	 * Wait until the radio state machine is not on rx mode
	 */
	//do {
	//	i = read_long_mem(MRF24J40_RFSTATE);
	//} while((i & 0xA0) == 0xA0);

	i = 0;

	i = i | 0b00100000;
	DBG("MRF24J40 Init NO_AUTO_ACK\r\n");

#ifdef MRF24J40_PAN_COORDINATOR
	i = i | 0b00001000;
	DBG("MRF24J40 Init PAN COORD\r\n");
	write_short_mem(MRF24J40_ORDER, 0xFF);
#endif

#ifdef MRF24J40_COORDINATOR
	i = i | 0b00000100;
	DBG("MRF24J40 Init COORD\r\n");
#endif

#ifdef MRF24J40_ACCEPT_WRONG_CRC_PKT
	i = i | 0b00000010;
	DBG("MRF24J40 Init Accept Wrong CRC\r\n");
#endif

#ifdef MRF24J40_PROMISCUOUS_MODE
	i = i | 0b00000001;
	DBG("MRF24J40 Init PROMISCUOUS MODE\r\n");
#endif

	/*
	 * Set the RXMCR register.
	 * Default setting i = 0x00, which means:
	 * - Automatic ACK;
	 * - Device is not a PAN coordinator;
	 * - Device is not a coordinator;
	 * - Accept only packets with good CRC
	 * - Discard packet when there is a MAC address mismatch,
	 *   illegal frame type, dPAN/sPAN or MAC short address mismatch.
	 */
	write_short_mem(MRF24J40_RXMCR, i);
	i = read_short_mem(MRF24J40_RXMCR);
	DBG("RXMCR 0x%X\r\n", i);

	/*
	 * Set the TXMCR register.
	 * bit 7   '0' Enable No Carrier Sense Multiple Access (CSMA) Algorithm.
	 * bit 6   '0' Disable Battery Life Extension Mode bit.
	 * bit 5   '0' Disable Slotted CSMA-CA Mode bit.
	 * bit 4:3 '11' MAC Minimum Backoff Exponent bits (macMinBE).
	 * bit 2:0 '100' CSMA Backoff bits (macMaxCSMABackoff)
	 */
	write_short_mem(MRF24J40_TXMCR, 0b00011100);

	i = read_short_mem(MRF24J40_TXMCR);
	DBG("TXMCR 0x%X\r\n", i);

	write_short_mem(MRF24J40_TXSTBL, 0x95);
	write_short_mem(MRF24J40_TXTIME, 0x30);

	write_long_mem(MRF24J40_SLPCON0, 0b00000001);
	DBG("MRF24J40 Init INT Polarity Low\r\n");

	DBG("MRF24J40 Inititialization completed\r\n");

	mrf24j40_last_lqi = 0;
	mrf24j40_last_rssi = 0;
	status_tx = MRF24J40_TX_ERR_NONE;

	receive_on = 1;

	process_start(&mrf24j40_process, NULL);

	/*
	 *
	 * Setup interrupts.
	 *
	 * set INTCON
	 * bit 7 '1' Disables the sleep alert interrupt
	 * bit 6 '1' Disables the wake-up alert interrupt
	 * bit 5 '1' Disables the half symbol timer interrupt
	 * bit 4 '1' Disables the security key request interrupt
	 * bit 3 '0' Enables the RX FIFO reception interrupt
	 * bit 2 '1' Disables the TX GTS2 FIFO transmission interrupt
	 * bit 1 '1' Disables the TX GTS1 FIFO transmission interrupt
	 * bit 0 '0' Enables the TX Normal FIFO transmission interrupt
	 */
	write_short_mem(MRF24J40_INTCON, 0b11110110);
	reset_rf_state_machine();

	/* Flush RX FIFO */
	flush_rx_fifo();

	ctrl_port->gpio_control(GPIO_CMD_ENA_BIT_INT, (void *)MRF24J40_INT_PIN);

	return 0;
}

/**
 * \brief prepare to send data
 *
 * \param[in] data, data to send
 * \param[in] len, data length
 * \return 0 succeeded, -1 failed
 */
int mrf24j40_prepare(const void *data, unsigned short len)
{

	DBG("PREPARE %u bytes\r\n", len);

	/* \todo Set TXNACKREQ if the ACK bit is set in the packet. */

	write_tx_buf(MRF24J40_NORMAL_TX_FIFO, data, len);

	return 0;
}

/**
 * \brief transmit data
 *
 * \param[in] data, data length to transmit
 * \return tx status
 */
int mrf24j40_transmit(unsigned short len)
{
	uint8_t reg;

	DBG("TRANSMIT %u bytes\r\n", len);

	status_tx = MRF24J40_TX_WAIT;

	reg = read_short_mem(MRF24J40_TXNCON) | 0x1;
	write_short_mem(MRF24J40_TXNCON, reg);

	/* Wait until the transmission has finished. */
	//while (status_tx == MRF24J40_TX_WAIT);

	return RADIO_TX_OK;
	// switch (status_tx) {
	// 	case MRF24J40_TX_ERR_NONE:
	// 		return RADIO_TX_OK;
	// 	case MRF24J40_TX_ERR_COLLISION:
	// 		return RADIO_TX_COLLISION;
	// 	case MRF24J40_TX_ERR_MAXRETRY:
	// 		return RADIO_TX_NOACK;
	// 	default:
	// 		return RADIO_TX_ERR;
	// }
}

/**
 * \brief write data
 *
 * \param[in] data, data to write
 * \param[in] len, data length
 * \return 0 succeeded, -1 failed
 */
int mrf24j40_write(const void *data, uint16_t len)
{
	int ret = -1;

	DBG("PREPARE & TRANSMIT %u bytes\r\n", len);

	if(mrf24j40_prepare(data, len))
		return ret;

	ret = mrf24j40_transmit(len);

	return ret;
}

/**
 * \brief read data
 *
 * \param[in] data, data to read
 * \param[in] len, read data length
 * \return >0 read data length, other failed
 */
int mrf24j40_read(void *data, uint16_t len)
{
	return read_rx_buf(data, len);
}

/**
 * \brief get cca
 * \return 1, above the threshhold, 0 below the threshhold
 */
int mrf24j40_cca(void)
{
	uint8_t ret;

	ret = (mrf24j40_read_rssi() <= 95); /* -69dbm */

	return ret;
}

/**
 * \brief receiving packet?
 */
int mrf24j40_receiving_packet(void)
{
	return 0;
}

/**
 * \brief packet pending?
 */
int mrf24j40_pending_packet(void)
{
	return 0;
}


PROCESS_THREAD(mrf24j40_process, ev, data)
{
	PROCESS_BEGIN();

	int ret = 0;

	while(1) {

		PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
		packetbuf_clear();
		ret = mrf24j40_read(packetbuf_dataptr(), PACKETBUF_SIZE);
		//xprintf("packet In :%d\r\n", ret);

		if (ret > 0) {
			packetbuf_set_datalen(ret);

			packetbuf_set_attr(PACKETBUF_ATTR_RSSI, mrf24j40_last_rssi);
			packetbuf_set_attr(PACKETBUF_ATTR_LINK_QUALITY, mrf24j40_last_lqi);

			NETSTACK_RDC.input();
		}
	}

	PROCESS_END();
}

/* data structure needed by contiki */
const struct radio_driver mrf24j40_driver = {
	mrf24j40_init,
	mrf24j40_prepare,
	mrf24j40_transmit,
	mrf24j40_write,
	mrf24j40_read,
	mrf24j40_cca,
	mrf24j40_receiving_packet,
	mrf24j40_pending_packet,
	mrf24j40_on,
	mrf24j40_off
};

/**
 * \brief dump mrf24j40 register for debug
 */
void mrf24j40_dump(void)
{
	uint8_t reg;

	reg = read_short_mem(MRF24J40_PACON2);
	DBG("PACON2:%x\r\n",reg);
	reg = read_short_mem(MRF24J40_ORDER);
	DBG("ORDER:%x\r\n", reg);
	reg = read_short_mem(MRF24J40_RXMCR);
	DBG("RXMCR:%x\r\n", reg);
	reg = read_short_mem(MRF24J40_TXMCR);
	DBG("TXMCR:%x\r\n", reg);
	reg = read_short_mem(MRF24J40_PANIDH);
	DBG("PANIDH:%x\r\n", reg);
	reg = read_short_mem(MRF24J40_PANIDL);
	DBG("PANIDL:%x\r\n", reg);
	reg = read_short_mem(MRF24J40_SADRH);
	DBG("SADRH:%x\r\n", reg);
	reg = read_short_mem(MRF24J40_SADRL);
	DBG("SADRL:%x\r\n", reg);
	reg = read_short_mem(MRF24J40_TXSTBL);
	DBG("TXSTBL:%x\r\n", reg);
	reg = read_short_mem(MRF24J40_TXTIME);
	DBG("TXTIME:%x\r\n", reg);
	reg = read_short_mem(MRF24J40_BBREG6);
	DBG("BBREG6:%x\r\n", reg);
	reg = read_short_mem(MRF24J40_BBREG2);
	DBG("BBREG2:%x\r\n", reg);
	reg = read_short_mem(MRF24J40_BBREG1);
	DBG("BBREG1:%x\r\n", reg);
	reg = read_short_mem(MRF24J40_CCAEDTH);
	DBG("CCAEDTH:%x\r\n", reg);
	reg = read_short_mem(MRF24J40_INTCON);
	DBG("INTCON:%x\r\n", reg);
	reg = read_long_mem(MRF24J40_RFCON1);
	DBG("RFCON1:%x\r\n", reg);
	reg = read_long_mem(MRF24J40_RFCON2);
	DBG("RFCON2:%x\r\n", reg);
	reg = read_long_mem(MRF24J40_RFCON3);
	DBG("RFCON3:%x\r\n", reg);
	reg = read_long_mem(MRF24J40_RFCON5);
	DBG("RFCON5:%x\r\n", reg);
	reg = read_long_mem(MRF24J40_RFCON6);
	DBG("RFCON6:%x\r\n", reg);
	reg = read_long_mem(MRF24J40_RFCON7);
	DBG("RFCON7:%x\r\n", reg);
	reg = read_long_mem(MRF24J40_RFCON8);
	DBG("RFCON8:%x\r\n", reg);
	reg = read_long_mem(MRF24J40_SLPCON0);
	DBG("SLPCON0:%x\r\n", reg);
	reg = read_long_mem(MRF24J40_SLPCON1);
	DBG("SLPCON1:%x\r\n", reg);
	reg = read_long_mem(MRF24J40_RFSTATE);
	DBG("RFSTAT:%x\r\n", reg);
}