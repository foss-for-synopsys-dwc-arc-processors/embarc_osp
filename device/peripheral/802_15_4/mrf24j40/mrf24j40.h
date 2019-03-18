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

#ifndef _MRF24J40_H_
#define _MRF24J40_H_

/* MRF24JRO RXFIFO size */
#define MRF24J40_BUF_SIZE		144
#define MRF24J40_RXFIFO_SIZE		144

/* Return values */
#define MRF24J40_INT_RX			0x01
#define MRF24J40_INT_TX			0x02
#define MRF24J40_INT_SEC		0x04
#define MRF24J40_INT_SLP		0x08
#define MRF24J40_INT_ENC		0x10
#define MRF24J40_INT_DEC		0x20

/* IEEE 802.15.4 constants needed for some flags */
#define IEEE_802_15_4_HAS_SEC(x)	((x >> 3) & 0x01)
#define IEEE_802_15_4_WANTS_ACK(x)	((x >> 5) & 0x01)

/* enc dec parameters */
#define MRF24J40_TX_KEY 		0x01
#define MRF24J40_RX_KEY 		0x02
#define MRF24J40_UP_KEY 		MRF24J40_TX_KEY

#define MRF24J40_AES_CBC_MAC32 		0x07
#define MRF24J40_AES_CBC_MAC64 		0x06
#define MRF24J40_AES_CBC_MAC128		0x05
#define MRF24J40_AES_CCM32 		0x04
#define MRF24J40_AES_CCM64 		0x03
#define MRF24J40_AES_CCM128 		0x02
#define MRF24J40_AES_CTR 		0x01
#define MRF24J40_ALGO_NONE 		0x00

/* Short Address Control Register Map */
#define MRF24J40_RXMCR 			0x00
#define MRF24J40_PANIDL 		0x01
#define MRF24J40_PANIDH 		0x02
#define MRF24J40_SADRL 			0x03
#define MRF24J40_SADRH 			0x04
#define MRF24J40_EADR0 			0x05
#define MRF24J40_EADR1			0x06
#define MRF24J40_EADR2			0x07
#define MRF24J40_EADR3			0x08
#define MRF24J40_EADR4			0x09
#define MRF24J40_EADR5			0x0A
#define MRF24J40_EADR6			0x0B
#define MRF24J40_EADR7			0x0C
#define MRF24J40_RXFLUSH 		0x0D

#define MRF24J40_ORDER 			0x10
#define MRF24J40_TXMCR 			0x11
#define MRF24J40_ACKTMOUT 		0x12
#define MRF24J40_ESLOTG1 		0x13
#define MRF24J40_SYMTICKL 		0x14
#define MRF24J40_SYMTICKH 		0x15
#define MRF24J40_PACON0 		0x16
#define MRF24J40_PACON1 		0x17
#define MRF24J40_PACON2 		0x18
#define MRF24J40_TXBCON0 		0x1A
#define MRF24J40_TXNCON 		0x1B
#define MRF24J40_TXG1CON 		0x1C
#define MRF24J40_TXG2CON 		0x1D
#define MRF24J40_ESLOTG23 		0x1E
#define MRF24J40_ESLOTG45 		0x1F

#define MRF24J40_ESLOTG67 		0x20
#define MRF24J40_TXPEND 		0x21
#define MRF24J40_WAKECON 		0x22
#define MRF24J40_FRMOFFSET 		0x23
#define MRF24J40_TXSTAT 		0x24
#define MRF24J40_TXBCON1 		0x25
#define MRF24J40_GATECLK 		0x26
#define MRF24J40_TXTIME 		0x27
#define MRF24J40_HSYMTMRL 		0x28
#define MRF24J40_HSYMTMRH 		0x29
#define MRF24J40_SOFTRST 		0x2A
#define MRF24J40_SECCON0 		0x2c
#define MRF24J40_SECCON1 		0x2d
#define MRF24J40_TXSTBL 		0x2e

#define MRF24J40_RXSR 			0x30
#define MRF24J40_INTSTAT 		0x31
#define MRF24J40_INTCON 		0x32
#define MRF24J40_GPIO 			0x33
#define MRF24J40_TRISGPIO 		0x34
#define MRF24J40_SLPACK 		0x35
#define MRF24J40_RFCTL 			0x36
#define MRF24J40_SECCR2 		0x37
#define MRF24J40_BBREG0 		0x38
#define MRF24J40_BBREG1 		0x39
#define MRF24J40_BBREG2 		0x3A
#define MRF24J40_BBREG3 		0x3B
#define MRF24J40_BBREG4 		0x3C
#define MRF24J40_BBREG6 		0x3E
#define MRF24J40_CCAEDTH 		0x3F

/* Long Address Control Register Map */
#define MRF24J40_RFCON0			0x200
#define MRF24J40_RFCON1			0x201
#define MRF24J40_RFCON2			0x202
#define MRF24J40_RFCON3			0x203
#define MRF24J40_RFCON5			0x205
#define MRF24J40_RFCON6			0x206
#define MRF24J40_RFCON7			0x207
#define MRF24J40_RFCON8			0x208
#define MRF24J40_RFSTATE		0x20F

#define MRF24J40_RSSI 			0x210
#define MRF24J40_SLPCON0		0x211
#define MRF24J40_SLPCON1		0x220

#define MRF24J40_WAKETIMEL		0x222
#define MRF24J40_WAKETIMEH		0x223

#define MRF24J40_AINCNT0		0x226
#define MRF24J40_AINCNT1		0x227
#define MRF24J40_AINCNT2		0x228
#define MRF24J40_AINCNT3		0x229

#define MRF24J40_ASSOEADR0		0x230
#define MRF24J40_ASSOEADR1		0x231
#define MRF24J40_ASSOEADR2		0x232
#define MRF24J40_ASSOEADR3		0x233
#define MRF24J40_ASSOEADR4		0x234
#define MRF24J40_ASSOEADR5		0x235
#define MRF24J40_ASSOEADR6		0x236
#define MRF24J40_ASSOEADR7		0x237

#define MRF24J40_ASSOSADR0		0x238
#define MRF24J40_ASSOSADR1		0x239

#define MRF24J40_UPNONCE0		0x240
#define MRF24J40_UPNONCE1		0x241
#define MRF24J40_UPNONCE2		0x242
#define MRF24J40_UPNONCE3		0x243
#define MRF24J40_UPNONCE4		0x244
#define MRF24J40_UPNONCE5		0x245
#define MRF24J40_UPNONCE6		0x246
#define MRF24J40_UPNONCE7		0x247
#define MRF24J40_UPNONCE8		0x248
#define MRF24J40_UPNONCE9		0x249
#define MRF24J40_UPNONCE10		0x24A
#define MRF24J40_UPNONCE11		0x24B
#define MRF24J40_UPNONCE12		0x24C

/* Long Address Memory Map */
#define MRF24J40_TXNFIFO 		0x000 /* - 0x07F, 128 bytes */
#define MRF24J40_TXBFIFO 		0x080 /* - 0x0FF, 128 bytes */
#define MRF24J40_TXG1FIFO 		0x100 /* - 0x17F, 128 bytes */
#define MRF24J40_TXG2FIFO 		0x180 /* - 0x1FF, 128 bytes */
#define MRF24J40_SECKFIFO 		0x280 /* - 0x2BF, 64 bytes */
#define MRF24J40_SECKTXNFIFO 		0x280 /* - 0x28F, 16 bytes */
#define MRF24J40_SECKRXFIFO 		0x2B0 /* - 0x2BF, 16 bytes */
#define MRF24J40_RXFIFO 		0x300 /* - 0x38F, 144 bytes */

/* RXMCR */
#define MRF24J40_NOACKRSP 		(1<<5)
#define MRF24J40_PANCOORD 		(1<<3)
#define MRF24J40_COORD 			(1<<2)
#define MRF24J40_ERRPKT 		(1<<1)
#define MRF24J40_PROMI 			(1)

/* RXFLUSH */
#define MRF24J40_WAKEPOL 		(1<<6)
#define MRF24J40_WAKEPAD 		(1<<5)
#define MRF24J40_CMDONLY 		(1<<3)
#define MRF24J40_DATAONLY 		(1<<2)
#define MRF24J40_BCNONLY 		(1<<1)
#define MRF24J40__RXFLUSH 		(1)

/* TXMCR */
#define MRF24J40_NOCSMA 		(1<<7)
#define MRF24J40_BATLIFEXT 		(1<<6)
#define MRF24J40_SLOTTED 		(1<<5)
#define MRF24J40_MACMINBE(x) 		((x & 0x03)<<3)
#define MRF24J40_CSMABF(x) 		(x & 0x07)

/* ACKTMOUT */
#define MRF24J40_DRPACK 		(1<<7)

/* PACON2 */
#define MRF24J40_FIFOEN 		(1<<7)
#define MRF24J40_TXONTS(x) 		(x & 0x3F)

/* TXNCON */
#define MRF24J40_FPSTAT 		(1<<4)
#define MRF24J40_INDIRECT 		(1<<3)
#define MRF24J40_TXNACKREQ 		(1<<2)
#define MRF24J40_TXNSECEN 		(1<<1)
#define MRF24J40_TXNTRIG 		(1)

/* TXPEND */
#define MRF24J40_FPACK 			(1)

/* WAKECON */
#define MRF24J40_IMMWAKE 		(1<<7)
#define MRF24J40_REGWAKE 		(1<<6)

/* TXSTAT */
#define MRF24J40_CCAFAIL 		(1<<5)
#define MRF24J40_TXNSTAT 		(1)

/* SOFTRST */
#define MRF24J40_RSTPWR 		(1<<2)
#define MRF24J40_RSTBB 			(1<<1)
#define MRF24J40_RSTMAC 		(1)

/* SECCON0 */
#define MRF24J40_SECIGNORE 		(1<<7)
#define MRF24J40_SECSTART 		(1<<6)
#define MRF24J40_RXCIPHER(x) 		((x & 0x07) << 3)
#define MRF24J40_TXNCIPHER(x) 		((x & 0x07))

/* SECCON1 */
#define MRF24J40_DISDEC 		(1<<1)
#define MRF24J40_DISENC 		(1)

/* TXSTBL */
#define MRF24J40_RFSTBL(x) 		((x & 0x0f) << 4)
#define MRF24J40_MSIFS(x) 		((x & 0x0f))

/* RXSR */
#define MRF24J40_UPSECERR 		(1<<6)
#define MRF24J40_SECDECERR 		(1<<2)

/* INTSTAT */
#define MRF24J40_SLPIF 			(1<<7)
#define MRF24J40_WAKEIF 		(1<<6)
#define MRF24J40_HSYMTMRIF 		(1<<5)
#define MRF24J40_SECIF 			(1<<4)
#define MRF24J40_RXIF 			(1<<3)
#define MRF24J40_TXG2IF 		(1<<2)
#define MRF24J40_TXG1IF 		(1<<1)
#define MRF24J40_TXNIF 			(1)

/* INTCON */
#define MRF24J40_SLPIE 			(1<<7)
#define MRF24J40_WAKEIE 		(1<<6)
#define MRF24J40_HSYMTMRIE 		(1<<5)
#define MRF24J40_SECIE 			(1<<4)
#define MRF24J40_RXIE 			(1<<3)
#define MRF24J40_TXG2IE 		(1<<2)
#define MRF24J40_TXG1IE 		(1<<1)
#define MRF24J40_TXNIE 			(1)

/* SLPACK */
#define MRF24J40__SLPACK 		(1<<7)
#define MRF24J40_WAKECNT_L(x) 		(x & 0x03F)

/* RFCTL */
#define MRF24J40_WAKECNT_H(x) 		((x & 0x03) << 3)
#define MRF24J40_RFRST 			(1<<2)
#define MRF24J40_RFTXMODE 		(1<<1)
#define MRF24J40_RFRXMODE 		(1)

/* SECCR2 */
#define MRF24J40_UPDEC			(1<<7)
#define MRF24J40_UPENC			(1<<6)

/* BBREG0 */
#define MRF24J40_TURBO			(1)

/* BBREG1 */
#define MRF24J40_RXDECINV		(1<<2)

/* BBREG2 */
#define MRF24J40_CCAMODE(x)		((x & 0x03) <<6)
#define MRF24J40_CCASTH(x)		((x & 0x0F) <<2)

/* BBREG3 */
#define MRF24J40_PREVALIDTH(x)		((x & 0x0F) <<4)

/* BBREG4 */
#define MRF24J40_CSTH(x)		((x & 0x07) << 5)

/* BBREG6 */
#define MRF24J40_RSSIMODE1		(1 << 7)
#define MRF24J40_RSSIMODE2		(1<<6)
#define MRF24J40_RSSIRDY		(1)

/* RFCON0 */
#define MRF24J40_CHANNEL(x)		((x & 0x0F) << 4)
#define MRF24J40_RFOPT(x)		((x & 0x0F))

/* RFCON1 */
#define MRF24J40_VCOOPT(x)		((x & 0xFF))

/* RFCON2 */
#define MRF24J40_PLLEN 			(1<<7)

/* RFCON3 */
#define MRF24J40_TXPWRL(x)		((x & 0x03) << 6)
#define MRF24J40_TXPWRS(x)		((x & 0x07) << 3)

/* RFCON6 */
#define MRF24J40_TXFIL			(1 << 7)
#define MRF24J40__20MRECVR		(1 << 4)
#define MRF24J40_BATEN 			(1 << 3)

/* RFCON 7 */
#define MRF24J40_SLPCLKSEL(x) 		((x & 0x03) << 6)
#define MRF24J40_SLPCLKSEL_100k 	(MRF24J40_SLPCLKSEL(0x02))
#define MRF24J40_SLPCLKSEL_32k 		(MRF24J40_SLPCLKSEL(0x01))

/* RFCON8 */
#define MRF24J40_RFVCO 			(1 << 4)

/* SLPCON0 */
#define MRF24J40_SLPCLKEN 		(1)

/* SLPCON1 */
#define MRF24J40_CLKOUTDIS		(1 << 5) 		/* CLKOUTEN' */
#define MRF24J40_SLPCLKDIV(x) 		((x & 0x1F))		/* division ratio: 2^(SLPCLKDIV) */

/* Channel state */
typedef enum mrf24j40_chstate {
	MRF24J40_EIO = 0,	/* Channel idle */
	MRF24J40_EBUSY = 1	/* Channel busy */
} MRF24J40_CHSTATE;

/* MRF24J40 definition */
/* DEV_GPIO_PORT_PIN_DEF is used in gpio_pin_xxx definition
	 spi_pin_cs
 */
typedef struct mrf24j40_def {
	/* interface and pin */
	uint8_t spi;				/* SPI ID */
	uint32_t spi_cs;			/* SPI CS */
	uint32_t gpio_pin_wake;			/* GPIO pin wake, port(16b) + pin(16b) */
	uint32_t gpio_pin_reset;		/* GPIO pin reset, port(16b) + pin(16b) */
	uint32_t gpio_pin_intr;			/* GPIO pin interrupt, port(16b) + pin(16b) */
	/* RX/TX buffer point */
	uint8_t rx_buf[MRF24J40_BUF_SIZE];
	uint8_t tx_buf[MRF24J40_BUF_SIZE];
	//MRF24J40_RXBUF *rx_buf;
	/* TX buffer point */
	//MRF24J40_TXBUF *tx_buf;
	/* channel state */
	volatile uint8_t ch_state;
} MRF24J40_DEF, *MRF24J40_DEF_PTR;

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t mrf24j40_read_long_ctrl_reg(MRF24J40_DEF *mrf24j40, uint16_t addr, uint8_t *val);
extern int32_t mrf24j40_read_short_ctrl_reg(MRF24J40_DEF *mrf24j40, uint8_t addr, uint8_t *val);
extern int32_t mrf24j40_write_long_ctrl_reg(MRF24J40_DEF *mrf24j40, uint16_t addr, uint8_t val);
extern int32_t mrf24j40_write_short_ctrl_reg(MRF24J40_DEF *mrf24j40, uint8_t addr, uint8_t val);
extern int32_t mrf24j40_rxfifo_flush(MRF24J40_DEF *mrf24j40);
extern int32_t mrf24j40_get_pending_frame(MRF24J40_DEF *mrf24j40, uint8_t *pend);
extern int32_t mrf24j40_reset(MRF24J40_DEF *mrf24j40);
extern int32_t mrf24j40_sleep(MRF24J40_DEF *mrf24j40);
extern int32_t mrf24j40_wakeup(MRF24J40_DEF *mrf24j40);
extern void mrf24j40_initialize(MRF24J40_DEF *mrf24j40);
extern int32_t mrf24j40_setup(MRF24J40_DEF *mrf24j40);
extern int32_t mrf24j40_set_short_addr(MRF24J40_DEF *mrf24j40, uint8_t *addr);
extern int32_t mrf24j40_set_coordinator_short_addr(MRF24J40_DEF *mrf24j40, uint8_t *addr);
extern int32_t mrf24j40_set_coordinator_eui(MRF24J40_DEF *mrf24j40, uint8_t *eui);
extern int32_t mrf24j40_set_eui(MRF24J40_DEF *mrf24j40, uint8_t *eui);
extern int32_t mrf24j40_set_pan(MRF24J40_DEF *mrf24j40, uint8_t *pan);
extern int32_t mrf24j40_set_tx_key(MRF24J40_DEF *mrf24j40, uint8_t *key);
extern int32_t mrf24j40_set_rx_key(MRF24J40_DEF *mrf24j40, uint8_t *key);
extern int32_t mrf24j40_set_channel(MRF24J40_DEF *mrf24j40, int16_t ch);
extern int32_t mrf24j40_set_promiscuous(MRF24J40_DEF *mrf24j40, uint8_t crc_check);
extern int32_t mrf24j40_set_coordinator(MRF24J40_DEF *mrf24j40, uint8_t mark);
extern int32_t mrf24j40_txpkt(MRF24J40_DEF *mrf24j40);
extern int32_t mrf24j40_set_cipher(MRF24J40_DEF *mrf24j40, uint8_t rxcipher, uint8_t txcipher);
extern int32_t mrf24j40_rx_sec_fail(MRF24J40_DEF *mrf24j40, uint8_t *err);
extern int32_t mrf24j40_rxpkt(MRF24J40_DEF *mrf24j40, uint8_t *plqi, uint8_t *prssi);
extern int32_t mrf24j40_set_decrypt_start(MRF24J40_DEF *mrf24j40, uint8_t accept);
extern int32_t mrf24j40_txfifo_write(MRF24J40_DEF *mrf24j40, uint16_t address, uint8_t *data, uint8_t hdr_len,
                                     uint8_t len);
extern int32_t mrf24j40_set_transmit_trig(MRF24J40_DEF *mrf24j40);

#ifdef __cplusplus
}
#endif

#endif /* _MRF24J40_H_ */
