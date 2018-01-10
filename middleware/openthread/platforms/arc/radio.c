/*
 *  Copyright (c) 2017, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for radio communication.
 *
 */

#include "openthread/types.h"

#include <utils/code_utils.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/radio.h>
#include "platform-arc.h"
#include "embARC_error.h"
#include "mrf24j40.h"

#include "dev_gpio.h"
#include <string.h>

#include <stdio.h>
#define DBG(fmt, ...)   printf(fmt, ##__VA_ARGS__)

#define MRF_GPIO_SETUP(port_pin, port, pin)		{ \
		port = (port_pin & 0xffff0000) >>16; \
		pin = port_pin & 0xffff; \
	}

enum
{
	IEEE802154_MIN_LENGTH         = 5,
	IEEE802154_MAX_LENGTH         = 127,
	IEEE802154_ACK_LENGTH         = 5,

	IEEE802154_BROADCAST          = 0xffff,

	IEEE802154_FRAME_TYPE_ACK     = 2 << 0,
	IEEE802154_FRAME_TYPE_MACCMD  = 3 << 0,
	IEEE802154_FRAME_TYPE_MASK    = 7 << 0,

	IEEE802154_SECURITY_ENABLED   = 1 << 3,
	IEEE802154_FRAME_PENDING      = 1 << 4,
	IEEE802154_ACK_REQUEST        = 1 << 5,
	IEEE802154_PANID_COMPRESSION  = 1 << 6,

	IEEE802154_DST_ADDR_NONE      = 0 << 2,
	IEEE802154_DST_ADDR_SHORT     = 2 << 2,
	IEEE802154_DST_ADDR_EXT       = 3 << 2,
	IEEE802154_DST_ADDR_MASK      = 3 << 2,

	IEEE802154_SRC_ADDR_NONE      = 0 << 6,
	IEEE802154_SRC_ADDR_SHORT     = 2 << 6,
	IEEE802154_SRC_ADDR_EXT       = 3 << 6,
	IEEE802154_SRC_ADDR_MASK      = 3 << 6,

	IEEE802154_DSN_OFFSET         = 2,
	IEEE802154_DSTPAN_OFFSET      = 3,
	IEEE802154_DSTADDR_OFFSET     = 5,

	IEEE802154_SEC_LEVEL_MASK     = 7 << 0,

	IEEE802154_KEY_ID_MODE_0      = 0 << 3,
	IEEE802154_KEY_ID_MODE_1      = 1 << 3,
	IEEE802154_KEY_ID_MODE_2      = 2 << 3,
	IEEE802154_KEY_ID_MODE_3      = 3 << 3,
	IEEE802154_KEY_ID_MODE_MASK   = 3 << 3,

	IEEE802154_MACCMD_DATA_REQ    = 4
};

enum
{
	ARC_RECEIVE_SENSITIVITY = -100,  // dBm
};

enum
{
	MRF24J40_RSSI_OFFSET = 90,
	MRF24J40_RSSI_SLOPE = 5
};

static MRF24J40_DEF mrf24j40_def;

static void radioTransmitMessage(otInstance *aInstance);

static otRadioFrame sTransmitFrame;
static otRadioFrame sReceiveFrame;
static otRadioFrame sAckFrame;

static otError sTransmitError;
static otError sReceiveError;

static uint8_t sTransmitPsdu[IEEE802154_MAX_LENGTH];
static uint8_t sReceivePsdu[IEEE802154_MAX_LENGTH];
static uint8_t sAckPsdu[IEEE802154_MAX_LENGTH];

static otRadioState sState = OT_RADIO_STATE_DISABLED;
static bool sIsReceiverEnabled = false;

static volatile uint8_t Mrf24StatusTx = 0;
static volatile uint8_t Mrf24StatusRx = 0;
static volatile uint8_t Mrf24StatusSec = 0;

static DEV_SPI_PTR pmrf_spi_ptr;

static void RadioIsr(void *ptr);

/* Variables for test */
static uint32_t numInterruptRev = 0;
static uint32_t numInterruptTrans = 0;
static uint32_t numRadioProcess = 0;

static inline bool isSecurityEnabled(const uint8_t *frame)
{
	return (frame[0] & IEEE802154_SECURITY_ENABLED) != 0;
}

static inline bool isAckRequested(const uint8_t *frame)
{
	return (frame[0] & IEEE802154_ACK_REQUEST) != 0;
}

static inline bool isPanIdCompressed(const uint8_t *frame)
{
	return (frame[0] & IEEE802154_PANID_COMPRESSION) != 0;
}

static inline uint8_t getHeadLength(const uint8_t *frame)
{
	uint8_t length = 0;
	/* Frame Control-2 + Sequence Number-1 */
	length += 2 + 1;

	/* Destination PAN + Address */
	switch (frame[1] & IEEE802154_DST_ADDR_MASK)
	{
	case IEEE802154_DST_ADDR_SHORT:
		length += sizeof(otPanId) + sizeof(otShortAddress);
		break;

	case IEEE802154_DST_ADDR_EXT:
		length += sizeof(otPanId) + sizeof(otExtAddress);
		break;

	default:
		length = 0;
		goto exit;
	}

	/* Source PAN + Address */
	switch (frame[1] & IEEE802154_SRC_ADDR_MASK)
	{
	case IEEE802154_SRC_ADDR_SHORT:
		if (!isPanIdCompressed(frame))
		{
			length += sizeof(otPanId);
		}

		length += sizeof(otShortAddress);
		break;

	case IEEE802154_SRC_ADDR_EXT:
		if (!isPanIdCompressed(frame))
		{
			length += sizeof(otPanId);
		}

		length += sizeof(otExtAddress);
		break;

	default:
		length = 0;
		goto exit;
	}

exit:
	return length;
}

static void mrf24j40Init(void)
{
	DEV_GPIO_BIT_ISR isr;
	DEV_GPIO_INT_CFG int_cfg;
	uint32_t port_wake, port_reset, port_intr;
	uint32_t pin_wake, pin_reset, pin_intr;
	uint32_t pin = 0;
	int32_t ercd;

#ifdef BOARD_HSDK
	/* Set PMOD-A as SPI. WAKE pin is n.c.. 
	   SPI1 is used and gpio[16] is INT and gpio[17] is RST. */
	io_pmod_config(PMOD_A, PMOD_SPI, 1);
#endif /* BOARD_HSDK */

	mrf24j40_def.spi = MRF24J40_SPI_ID;
	mrf24j40_def.spi_cs = MRF24J40_SPI_CS;
	mrf24j40_def.gpio_pin_wake = DEV_GPIO_PORT_PIN_DEF(MRF24J40_GPIO_PORT_WAKE, MRF24J40_GPIO_PIN_WAKE);	// DEV_GPIO_PORT_0 --  DW_GPIO_PORT_A
	mrf24j40_def.gpio_pin_reset = DEV_GPIO_PORT_PIN_DEF(MRF24J40_GPIO_PORT_RESET, MRF24J40_GPIO_PIN_RESET);
	mrf24j40_def.gpio_pin_intr = DEV_GPIO_PORT_PIN_DEF(MRF24J40_GPIO_PIN_INTR, MRF24J40_GPIO_PIN_INTR);

	memset(&(mrf24j40_def.rx_buf[0]), 0, sizeof(uint8_t)*MRF24J40_BUF_SIZE);
	memset(&(mrf24j40_def.tx_buf[0]), 0, sizeof(uint8_t)*MRF24J40_BUF_SIZE);
	mrf24j40_def.ch_state = MRF24J40_EIO;

	pmrf_spi_ptr = spi_get_dev(mrf24j40_def.spi);
	ercd = pmrf_spi_ptr->spi_open(DEV_MASTER_MODE, MRF24J40_SPIFREQ);

	if ((ercd != E_OK) && (ercd != E_OPNED))
	{
		DBG("PmodRF2 SPI open error\r\n");
	}

	DBG("Board SPI Init\r\n");
	pmrf_spi_ptr->spi_control(SPI_CMD_SET_CLK_MODE, CONV2VOID(MRF24J40_SPICLKMODE));

	/*MRF24J40 wakepin:output, rstpin:output, INT_PIN:input, interrupt */
	DBG("Board GPIO Init\r\n");
	MRF_GPIO_SETUP(mrf24j40_def.gpio_pin_wake, port_wake, pin_wake);
	MRF_GPIO_SETUP(mrf24j40_def.gpio_pin_reset, port_reset, pin_reset);
	MRF_GPIO_SETUP(mrf24j40_def.gpio_pin_intr, port_intr, pin_intr);
	/* In ARC boards, the pin wake, reset and intr are in a same port */
	DEV_GPIO_PTR pmrf_gpio_ptr = gpio_get_dev(port_reset);
	// ercd = pmrf_gpio_ptr->gpio_open((1 << pin_wake) | (1 << pin_reset) | (1 << pin_intr));
	if (pin_wake != DEV_GPIO_PIN_NC)
	{
		pin |= (1 << pin_wake);
	}
	if (pin_reset != DEV_GPIO_PIN_NC)
	{
		pin |= (1 << pin_reset);
	}
	ercd = pmrf_gpio_ptr->gpio_open(pin);

	if ((ercd != E_OK) && (ercd != E_OPNED))
	{
		DBG("PmodRF2 CRTL port open error");
	}

	ercd = pmrf_gpio_ptr->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, (void *)((1 << pin_wake) | (1 << pin_reset)));
	ercd = pmrf_gpio_ptr->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT, (void *)(1 << pin_intr));

	// if (ercd == E_OPNED)
	// {
		// ercd = pmrf_gpio_ptr->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, (void *)((1 << pin_wake) | (1 << pin_reset)));
		// ercd = pmrf_gpio_ptr->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT, (void *)(1 << pin_intr));
	// }

	DBG("ARC Interrupt Config\r\n");
	pmrf_gpio_ptr->gpio_control(GPIO_CMD_DIS_BIT_INT, (void *)(1 << pin_intr));

	int_cfg.int_bit_mask = 1 << pin_intr;
	int_cfg.int_bit_type = GPIO_INT_BITS_EDGE_TRIG(1 << pin_intr);
	int_cfg.int_bit_polarity = GPIO_INT_BITS_POL_FALL_EDGE(1 << pin_intr);
	int_cfg.int_bit_debounce = GPIO_INT_BITS_DIS_DEBOUNCE(1 << pin_intr);
	pmrf_gpio_ptr->gpio_control(GPIO_CMD_SET_BIT_INT_CFG, (void *)(&int_cfg));

	isr.int_bit_ofs = pin_intr;
	isr.int_bit_handler = RadioIsr;
	pmrf_gpio_ptr->gpio_control(GPIO_CMD_SET_BIT_ISR, (void *)(&isr));

	/* interrupt for mrf24j40 is enabled at the end of the init process */
	DBG("MRF24J40 Init Started\r\n");
	mrf24j40_initialize(&mrf24j40_def);
	DBG("MRF24J40 Init Finished\r\n");

	pmrf_gpio_ptr->gpio_control(GPIO_CMD_ENA_BIT_INT, (void *)(1 << pin_intr));

}

void enableReceiver(void)
{
	if (!sIsReceiverEnabled)
	{
		mrf24j40_rxfifo_flush(&mrf24j40_def);
		/* add code to enable receiver (wakeup) */
		sIsReceiverEnabled = true;
	}
}

void disableReceiver(void)
{
	if (sIsReceiverEnabled)
	{
		mrf24j40_rxfifo_flush(&mrf24j40_def);
		/* add code to disable receiver (sleep) */
		sIsReceiverEnabled = false;
	}
}

void setChannel(uint8_t channel)
{
	mrf24j40_set_channel(&mrf24j40_def, (int16_t)(channel - 11));
}

void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64)
{
	(void)aInstance;
	/* should set it manually or preset it in memory */
	aIeeeEui64[0] = 0x00;
	aIeeeEui64[1] = 0x50;
	aIeeeEui64[2] = 0xC2;
	aIeeeEui64[3] = 0xFF;
	aIeeeEui64[4] = 0XFE;
	aIeeeEui64[5] = 0X1D;
	aIeeeEui64[6] = 0X30;
	aIeeeEui64[7] = 0x00;
}

void otPlatRadioSetPanId(otInstance *aInstance, uint16_t panid)
{
	(void)aInstance;
	uint8_t pan[2];

	pan[0] = (uint8_t)(panid & 0xFF);
	pan[1] = (uint8_t)(panid >> 8);
	mrf24j40_set_pan(&mrf24j40_def, pan);
}

void otPlatRadioSetExtendedAddress(otInstance *aInstance, otExtAddress *address)
{
	(void)aInstance;
	mrf24j40_set_eui(&mrf24j40_def, address->m8);
}

void otPlatRadioSetShortAddress(otInstance *aInstance, uint16_t address)
{
	(void)aInstance;
	uint8_t addr[2];

	addr[0] = (uint8_t)(address & 0xFF);
	addr[1] = (uint8_t)(address >> 8);
	mrf24j40_set_short_addr(&mrf24j40_def, addr);
}

void arcRadioInit(void)
{
	sTransmitFrame.mLength = 0;
	sTransmitFrame.mPsdu = sTransmitPsdu;
	sReceiveFrame.mLength = 0;
	sReceiveFrame.mPsdu = sReceivePsdu;
	sAckFrame.mLength = 0;
	sAckFrame.mPsdu = sAckPsdu;

	mrf24j40Init();
}

bool otPlatRadioIsEnabled(otInstance *aInstance)
{
	(void)aInstance;
	return (sState != OT_RADIO_STATE_DISABLED);
}

otError otPlatRadioEnable(otInstance *aInstance)
{
	if (!otPlatRadioIsEnabled(aInstance))
	{
		sState = OT_RADIO_STATE_SLEEP;
	}

	return OT_ERROR_NONE;
}

otError otPlatRadioDisable(otInstance *aInstance)
{
	if (otPlatRadioIsEnabled(aInstance))
	{
		sState = OT_RADIO_STATE_DISABLED;
	}

	return OT_ERROR_NONE;
}

otError otPlatRadioSleep(otInstance *aInstance)
{
	otError error = OT_ERROR_INVALID_STATE;
	(void)aInstance;

	if (sState == OT_RADIO_STATE_SLEEP || sState == OT_RADIO_STATE_RECEIVE)
	{
		error = OT_ERROR_NONE;
		sState = OT_RADIO_STATE_SLEEP;
		disableReceiver();
	}

	return error;
}

otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
	otError error = OT_ERROR_INVALID_STATE;
	(void)aInstance;

	if (sState != OT_RADIO_STATE_DISABLED)
	{
		error = OT_ERROR_NONE;
		sState = OT_RADIO_STATE_RECEIVE;
		setChannel(aChannel);
		sReceiveFrame.mChannel = aChannel;
		enableReceiver();
	}

	return error;
}

otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
	otError error = OT_ERROR_INVALID_STATE;
	(void)aInstance;
	(void)aFrame;

	if (sState == OT_RADIO_STATE_RECEIVE)
	{
		error = OT_ERROR_NONE;
		sState = OT_RADIO_STATE_TRANSMIT;
	}

	return error;

}

otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
	(void)aInstance;
	return &sTransmitFrame;
}

int8_t otPlatRadioGetRssi(otInstance *aInstance)
{
	(void)aInstance;
	return 0;
}

otRadioCaps otPlatRadioGetCaps(otInstance *aInstance)
{
	(void)aInstance;
	return OT_RADIO_CAPS_NONE;
}

bool otPlatRadioGetPromiscuous(otInstance *aInstance)
{
	(void)aInstance;
	uint8_t val = 0;
	mrf24j40_read_short_ctrl_reg(&mrf24j40_def, MRF24J40_RXMCR, &val);
	return (bool)(val & MRF24J40_PROMI);
}

// should be checked again with CC2538
void otPlatRadioSetPromiscuous(otInstance *aInstance, bool aEnable)
{
	(void)aInstance;
	mrf24j40_set_promiscuous(&mrf24j40_def, ~aEnable);
}

void readFrame(void)
{

	/* readBuffer
	 * 1 bit -- 5 to 127 bits -- 1 bit -- 1bit
	 * Frame Length -- PSDU (Header + Data Payload + FCS) -- LQI -- RSSI
	 */
	uint8_t readPlqi = 0;
	uint8_t readRssi = 0;

	uint16_t length;

	memset(&(mrf24j40_def.rx_buf[0]), 0, MRF24J40_RXFIFO_SIZE);

	otEXPECT_ACTION(sState == OT_RADIO_STATE_RECEIVE || sState == OT_RADIO_STATE_TRANSMIT, ;);
	otEXPECT_ACTION(Mrf24StatusRx, ;);

	if (Mrf24StatusRx == 1)
	{
		Mrf24StatusRx = 0;
	}

	if (Mrf24StatusSec == 1)
	{
		Mrf24StatusSec = 0;
#if OPENTHREAD_CLI_DEBUG_INFO
		DBG("Receive Frame with encryption!!!!!!!!!!\r\n");
#endif
	}

	/* Read length */
	if (mrf24j40_rxpkt(&mrf24j40_def, &readPlqi, &readRssi) != E_OK)
	{
#if OPENTHREAD_CLI_DEBUG_INFO
		DBG("Read process error");
		goto exit;
#endif
	}
	length = mrf24j40_def.rx_buf[0];
#if OPENTHREAD_CLI_DEBUG_INFO
	DBG("Receive Frame Length = %d\r\n", length);
#endif

	otEXPECT_ACTION(IEEE802154_MIN_LENGTH <= length && length <= IEEE802154_MAX_LENGTH, ;);
	/* Read PSDU */
	memcpy(sReceiveFrame.mPsdu, &(mrf24j40_def.rx_buf[1]), length - 2);
#if OPENTHREAD_CLI_DEBUG_INFO
	DBG("PSDU Frame Receive\r\n");
	for (int i = 0; i < length - 2; i++)
	{
		// sReceiveFrame.mPsdu[i] = mrf24j40_def.rx_buf[i+1];
		DBG("%x ", sReceiveFrame.mPsdu[i]);
	}
	DBG("\r\nPSDU Frame Receive Finish\r\n");
#endif

#if OPENTHREAD_ENABLE_RAW_LINK_API
	// Timestamp
	sReceiveFrame.mMsec = otPlatAlarmMilliGetNow();
	sReceiveFrame.mUsec = 0;  // Don't support microsecond timer for now.
#endif


	sReceiveFrame.mPower = (int8_t)(readRssi / MRF24J40_RSSI_SLOPE) - MRF24J40_RSSI_OFFSET;
	sReceiveFrame.mLength = (uint8_t) length;
	sReceiveFrame.mLqi = readPlqi;

#if OPENTHREAD_CLI_DEBUG_INFO
	DBG("CUR_MS = %d\r\n", (uint32_t)OSP_GET_CUR_MS());
	DBG("numRadioProcess %d, numInterruptRev %d, numInterruptTrans %d \r\n\r\n\r\n", numRadioProcess, numInterruptRev, numInterruptTrans);
#endif

exit:
	return;

}

void radioTransmitMessage(otInstance *aInstance)
{
	uint8_t header_len = 0;
	uint8_t reg = 0;
	uint8_t val = 0;

	sTransmitError = OT_ERROR_NONE;
	setChannel(sTransmitFrame.mChannel);

#if OPENTHREAD_CLI_DEBUG_INFO
	DBG("\r\nPSDU Frame Trans\r\n");
	for (int16_t i = 0; i < (sTransmitFrame.mLength - 2); i++)
	{
		DBG("%x ", sTransmitFrame.mPsdu[i]);
	}
	DBG("\r\nPSDU Frame Trans Finish\r\n");

	DBG("Transmit Frame Length = %d\r\n", sTransmitFrame.mLength);
#endif

	mrf24j40_read_short_ctrl_reg(&mrf24j40_def, MRF24J40_TXNCON, &reg);

	header_len = getHeadLength(sTransmitFrame.mPsdu);

	if (isAckRequested(sTransmitFrame.mPsdu))
	{
		reg |= MRF24J40_TXNACKREQ;
#if OPENTHREAD_CLI_DEBUG_INFO
		DBG("PSDU Frame Trans with ACK Request!!!!!!\r\n");
#endif
	}
	else
	{
		reg &= ~(MRF24J40_TXNACKREQ);
	}

	if (isSecurityEnabled(sTransmitFrame.mPsdu))
	{
		reg |= MRF24J40_TXNSECEN;
#if OPENTHREAD_CLI_DEBUG_INFO
		DBG("PSDU Frame Trans with Security\r\n");
#endif
	}
	else
	{
		reg &= ~(MRF24J40_TXNSECEN);
		mrf24j40_read_short_ctrl_reg(&mrf24j40_def, MRF24J40_TXNCON, &val);
		mrf24j40_write_short_ctrl_reg(&mrf24j40_def, MRF24J40_TXNCON, val & (~MRF24J40_TXNSECEN));
	}

	mrf24j40_txfifo_write(&mrf24j40_def, MRF24J40_TXNFIFO, sTransmitFrame.mPsdu, header_len, (sTransmitFrame.mLength - 2));

	mrf24j40_write_short_ctrl_reg(&mrf24j40_def, MRF24J40_TXNCON, reg | MRF24J40_TXNTRIG);

	otPlatRadioTxStarted(aInstance, &sTransmitFrame);

	int16_t tx_timeout = 1000;
	Mrf24StatusTx = 0;

	mrf24j40_read_short_ctrl_reg(&mrf24j40_def, MRF24J40_TXNCON, &val);
	DBG("TXNCON = 0x%x, ", val);
	while ((tx_timeout > 0) && (Mrf24StatusTx != 1))
	{
		board_delay_ms(1, OSP_DELAY_OS_COMPAT_DISABLE);
		tx_timeout--;

#if OPENTHREAD_CLI_DEBUG_INFO
		if (tx_timeout <= 0)
		{
			DBG("Radio Transmit Timeout!!!!!!!!!!!!\r\n");
		}
#endif
	}
#if OPENTHREAD_CLI_DEBUG_INFO
	DBG("Radio Transmit process done.\r\n");
	DBG("Mrf24StatusTx = %d\r\n", Mrf24StatusTx);
	DBG("CUR_MS = %d\r\n", (uint32_t)OSP_GET_CUR_MS());
	DBG("numRadioProcess %d, numInterruptRev %d, numInterruptTrans %d \r\n\r\n\r\n", numRadioProcess, numInterruptRev, numInterruptTrans);
#endif
}

void arcRadioProcess(otInstance *aInstance)
{
	numRadioProcess++;

	readFrame();
	uint8_t reg = 0;
	mrf24j40_read_short_ctrl_reg(&mrf24j40_def, MRF24J40_TXSTAT, &reg);

	if (reg & MRF24J40_TXNSTAT)
	{
#if OPENTHREAD_CLI_DEBUG_INFO
		DBG("TX MAC Timeout!!!!!!\r\n");

		if (reg & MRF24J40_CCAFAIL)
		{
			DBG("Channel busy!!!!!!\r\n");
		}
#endif
	}

	if ((sState == OT_RADIO_STATE_RECEIVE) && (sReceiveFrame.mLength > 0))
	{
		otPlatRadioReceiveDone(aInstance, &sReceiveFrame, sReceiveError);
	}

	if (sState == OT_RADIO_STATE_TRANSMIT)
	{
		radioTransmitMessage(aInstance);

		if (sTransmitError != OT_ERROR_NONE || (sTransmitFrame.mPsdu[0] & IEEE802154_ACK_REQUEST) == 0)
		{
			sState = OT_RADIO_STATE_RECEIVE;
			otPlatRadioTxDone(aInstance, &sTransmitFrame, NULL, sTransmitError);
		}
		else if (Mrf24StatusTx == 1)
		{
			Mrf24StatusTx = 0;
			sState = OT_RADIO_STATE_RECEIVE;
			otPlatRadioTxDone(aInstance, &sTransmitFrame, &sReceiveFrame, sTransmitError);
		}
	}

	sReceiveFrame.mLength = 0;

}

/**
 * \brief isr routine of mrf24j40
 * \param[in] ptr pointer transferred frome interrupt entry
 */
static void RadioIsr(void *ptr)
{
	uint8_t int_status = 0;
	uint8_t tx_status = 0;

	mrf24j40_read_short_ctrl_reg(&mrf24j40_def, MRF24J40_INTSTAT, &int_status);

	/* a frame is received */
	if (int_status & MRF24J40_RXIF)
	{
		numInterruptRev++;
		Mrf24StatusRx = 1;
	}
	/* a frame is transmitted */
	if (int_status & MRF24J40_TXNIF)
	{
		mrf24j40_read_short_ctrl_reg(&mrf24j40_def, MRF24J40_TXSTAT, &tx_status);
		if (tx_status & MRF24J40_TXNSTAT)
		{
			if (tx_status & MRF24J40_CCAFAIL)
			{
				/* Failed, channel busy */
				mrf24j40_def.ch_state = MRF24J40_EBUSY;
			}
			else
			{
				/* Failed, channel idle */
				mrf24j40_def.ch_state = MRF24J40_EIO;
			}
		}
		else
		{
			numInterruptTrans++;
			Mrf24StatusTx = 1;
		}
	}
	/* a frame with security key request is received */
	if (int_status & MRF24J40_SECIF)
	{
		Mrf24StatusSec = 1;
		/* ignore decryption process here */
		mrf24j40_set_decrypt_start(&mrf24j40_def, 0);
	}

}

/* CC2538 supports source address matching for low power consumption
   and this is not supported in MRF24J40 */
void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable)
{
	(void)aInstance;
	(void)aEnable;
}

otError otPlatRadioAddSrcMatchShortEntry(otInstance *aInstance, const uint16_t aShortAddress)
{
	(void)aInstance;
	(void)aShortAddress;
	return OT_ERROR_NONE;
}

otError otPlatRadioAddSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
	(void)aInstance;
	(void)aExtAddress;
	return OT_ERROR_NONE;
}

otError otPlatRadioClearSrcMatchShortEntry(otInstance *aInstance, const uint16_t aShortAddress)
{
	(void)aInstance;
	(void)aShortAddress;
	return OT_ERROR_NONE;
}

otError otPlatRadioClearSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
	(void)aInstance;
	(void)aExtAddress;
	return OT_ERROR_NONE;
}

void otPlatRadioClearSrcMatchShortEntries(otInstance *aInstance)
{
	(void)aInstance;
}

void otPlatRadioClearSrcMatchExtEntries(otInstance *aInstance)
{
	(void)aInstance;
}

otError otPlatRadioEnergyScan(otInstance *aInstance, uint8_t aScanChannel, uint16_t aScanDuration)
{
	(void)aInstance;
	(void)aScanChannel;
	(void)aScanDuration;
	return OT_ERROR_NOT_IMPLEMENTED;
}

void otPlatRadioSetDefaultTxPower(otInstance *aInstance, int8_t aPower)
{
	// TODO: Create a proper implementation for this driver.
	(void)aInstance;
	(void)aPower;
}

int8_t otPlatRadioGetReceiveSensitivity(otInstance *aInstance)
{
	(void)aInstance;
	return ARC_RECEIVE_SENSITIVITY;
}
