/**
 * Free YModem implementation.
 *
 * Fredrik Hederstierna 2014
 *
 * Copyright (c) 2015 Synopsys, Inc.
 * Modified for port to ARC processor
 *
 * This file is in the public domain.
 * You can do whatever you want with it.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "cmds_fs_cfg.h"
#if NTSHELL_USE_CMDS_FS_YMODEM
#include "ymodem.h"
#include "embARC_debug.h"
#include "ymodem_conf.h"
// filesize 999999999999999 should be enough...
#define FILE_SIZE_LENGTH		(16)

#define PACKET_SEQNO_INDEX		(1)
#define PACKET_SEQNO_COMP_INDEX		(2)
#define PACKET_HEADER			(3)	 /* start, block, block-complement */
#define PACKET_TRAILER			(2)	 /* CRC bytes */
#define PACKET_OVERHEAD			(PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE			(128)
#define PACKET_1K_SIZE			(1024)

#define ABT1				(0x41)	// 'A' == 0x41, abort by user
#define ABT2				(0x61)	// 'a' == 0x61, abort by user
#define SOH				(0x01)	// start of 128-byte data packet
#define STX				(0x02)	// start of 1024-byte data packet
#define EOT				(0x04)	// End Of Transmission
#define ACK				(0x06)	// ACKnowledge, receive OK
#define NAK				(0x15)	// Negative ACKnowledge, receiver ERROR; retry
#define CAN				(0x18)	// two CAN in succession will abort transfer
#define CRC				(0x43)	// 'C' == 0x43, request 16-bit CRC;
						// use in place of first NAK for CRC mode

#define YM_FAIL				(-1)

extern void ym_sleep_ms(uint32_t ms);
extern int32_t ym_get_char(int32_t delay);
extern int32_t ym_put_char(int32_t ch);
extern int32_t ym_open_dest(const char *filename);
extern int32_t ym_close_dest(void);
extern int32_t ym_output_dest(uint8_t *buf, int32_t length, const char *filename);
extern int32_t ym_get_size(const char *filename);
extern int32_t ym_get_data(char *buf, int32_t pt, int32_t length, const char *filename);

// user callbacks
#define _getchar(tmo_s)			ym_get_char(tmo_s)//read(tmo_s)
#define _putchar(c)			ym_put_char(c)
#define _sleep(s)			ym_sleep_ms(s*1000)
#define _flush() 			do { ; } while(0) //stdin_flush()

// error log function
#define YM_ERR(fmt, ...)		EMBARC_PRINTF(fmt, __VA_ARGS__)

//------------------------------------------------
// calculate crc16-ccitt very fast
// Idea from: http://www.ccsinfo.com/forum/viewtopic.php?t=24977
static uint16_t ym_crc16(const uint8_t *buf, uint16_t len)
{
	uint16_t x;
	uint16_t crc = 0;
	while (len--) {
		x = (crc >> 8) ^ *buf++;
		x ^= x >> 4;
		crc = (crc << 8) ^ (x << 12) ^ (x << 5) ^ x;
	}
	return crc;
}

//-------------------------------------------------
// write 32bit value as asc to buffer, return chars written.
static uint32_t ym_writeU32(uint32_t val, uint8_t *buf)
{
	uint32_t ci = 0;
	if (val == 0) {
		// If already zero then just return zero
		buf[ci++] = '0';
	}
	else {
		// Maximum number of decimal digits in uint32_t is 10
		uint8_t s[11];
		int32_t i = sizeof(s)-1;
		s[i] = 0;
		while ((val > 0) && (i > 0)) {
			s[--i] = (val % 10) + '0';
			val /= 10;
		}
		uint8_t *sp = &s[i];
		while (*sp) {
			buf[ci++] = *sp++;
		}
	}
	buf[ci] = 0;
	return ci;
}

//-------------------------------------------------
// read 32bit asc value from buffer
static void ym_readU32(const uint8_t* buf, uint32_t *val)
{
	const uint8_t *s = buf;
	uint32_t res = 0;
	uint8_t c;
	// Trim & strip leading spaces if any
	do {
		c = *s++;
	} while (c == ' ');
	while ((c >= '0') && (c <= '9')) {
		c -= '0';
		res *= 10;
		res += c;
		c = *s++;
	}
	*val = res;
}

//--------------------------------------------------
/**
 * Receive a packet from sender
 * @param rxlen
 *	0: end of transmission
 *	-1: abort by sender
 *	>0: packet length
 * @return 0: normally return, success
 *	-1: timeout or packet error
 *	1: abort by user / corrupt packet
 **/
static int32_t ym_rx_packet(uint8_t *rxdata, int32_t *rxlen, uint32_t packets_rxed, uint32_t timeout_sec)
{
	*rxlen = 0;
	int32_t cnt_soh = 0;
	int32_t c = _getchar(timeout_sec);
	if (c < 0) {
		// end of stream
		return -1;
	}

	uint32_t rx_packet_size;

	switch (c) {
	case SOH:
		cnt_soh ++;
		rx_packet_size = PACKET_SIZE;
		break;
	case STX:
		rx_packet_size = PACKET_1K_SIZE;
		break;
	case EOT:
		// ok
		return 0;
	case CAN:
		c = _getchar(timeout_sec);
		if (c == CAN) {
			*rxlen = -1;
			// ok
			return 0;
		} else {
			return -1;
		}
	case CRC:
		if (packets_rxed == 0) {
			// could be start condition, first byte
			return 1;
		}
	case ABT1:
	case ABT2:
		// User try abort, 'A' or 'a' received
		return 1;
	default:
		// This case could be the result of corruption on the first octet
		// of the packet, but it's more likely that it's the user banging
		// on the terminal trying to abort a transfer. Technically, the
		// former case deserves a NAK, but for now we'll just treat this
		// as an abort case.
		*rxlen = -1;
		return 0;
	}

	// store data rxed
	*rxdata = (uint8_t)c;

	uint32_t i;
	for (i = 1; i < (rx_packet_size + PACKET_OVERHEAD); i ++) {
		c = _getchar(timeout_sec);
		if (c < 0) {
			// end of stream
			return -1;
		}
		//store data rxed
		rxdata[i] = (uint8_t)c;
	}
	// Just a sanity check on the sequence number/complement value.
	// Caller should check for in-order arrival.
	uint8_t seq_nbr = (rxdata[PACKET_SEQNO_INDEX] & 0xff);
	uint8_t seq_cmp = ((rxdata[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff);
	if (seq_nbr != seq_cmp) {
		// seq nbr error
		return 1;
	}

	// Check CRC16 match
	uint16_t crc16_val = ym_crc16((const unsigned char *)(rxdata + PACKET_HEADER), \
					rx_packet_size + PACKET_TRAILER);
	if (crc16_val) {
		// CRC error, non zero
		return 1;
	}
	*rxlen = rx_packet_size;
	// success
	return 0;
}

//-------------------------------------------------
/**
 * Receive a file using the ymodem protocol
 * @param rxdata Pointer to the first byte
 * @param rxlen	Max in length
 * @return The length of the file received, or -1 on error
 */
int32_t fymodem_receive(void)
{
	// alloc 1k on stack, ok?
	uint8_t rx_packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
	int32_t rx_packet_len;
	char filename[FYMODEM_FILE_NAME_MAX_LENGTH];

	uint8_t filesize_asc[FILE_SIZE_LENGTH];
	uint32_t filesize = 0;

	bool first_try = true;
	bool session_done = false;

	uint32_t nbr_errors = 0;

	// z-term string
	filename[0] = 0;

	// receive files
	do {
		if (!first_try) {
			_putchar(CRC);
		}
		first_try = false;

		bool crc_nak = true;
		bool file_done = false;
		uint32_t packets_rxed = 0;

		do {
			// receive packets
			int32_t res = ym_rx_packet(rx_packet_data, &rx_packet_len, packets_rxed, PACKET_RX_TIMEOUT_SEC);
			switch (res) {
			case 0: {
				nbr_errors = 0;
				switch (rx_packet_len) {
				case -1: {
					// aborted by sender
					_putchar(ACK);
					ym_close_dest();
					return YM_FAIL;
				}
				case 0: {
					// EOT - End Of Transmission
					_putchar(ACK);
					ym_close_dest();
					// Should add some sort of sanity check on the number of
					// packets received and the advertised file length.
					file_done = true;
					break;
				}
				default: {
					// normal packet - check seq nbr
					uint8_t seq_nbr = rx_packet_data[PACKET_SEQNO_INDEX];
					if (seq_nbr != (packets_rxed & 0xff)) {
						// wrong seq number
						_putchar(NAK);
					} else {
						if (packets_rxed == 0) {
							// The spec suggests that the whole data section should
							// be zeroed, but some senders might not do this. If
							// we have a NULL filename and the first few digits of
							// the file length are zero, we'll call it empty.
							int32_t i;
							for (i = PACKET_HEADER; i < PACKET_HEADER + 4; i++) {
								if (rx_packet_data[i] != 0) {
									break;
								}
							}
							// non-zero bytes found in header, filename packet has data
							if (i < PACKET_HEADER + 4) {
								// read file name
								uint8_t *file_ptr = (uint8_t*)(rx_packet_data + PACKET_HEADER);
								i = 0;
								while (*file_ptr && (i < (FYMODEM_FILE_NAME_MAX_LENGTH-1))) {
									filename[i++] = *file_ptr++;
								}
								filename[i] = '\0';
								file_ptr++;
								// read file size
								i = 0;
								while ((*file_ptr != ' ') && (i < (FILE_SIZE_LENGTH-1))) {
									filesize_asc[i++] = *file_ptr++;
								}
								filesize_asc[i] = '\0';
								// convert file size
								ym_readU32(filesize_asc, &filesize);
								// check file size
								// if (filesize > rxlen) {
								// 	YM_ERR("rx buffer too small (0x%08x vs 0x%08x)\r\n", (int32_t)rxlen, filesize);
								// 	goto rx_err_handler;
								// }
								ym_open_dest(filename);
								_putchar(ACK);
								_putchar(crc_nak ? CRC : NAK);
								_sleep(1);
								crc_nak = false;
							} else {
								// filename packet is empty; end session
								_putchar(ACK);
								file_done = true;
								session_done = true;
								break;
							}
						} else {
							// This shouldn't happen, but we check anyway in case the
							// sender sent wrong info in its filename packet
							// if (((rxptr + rx_packet_len) - rxdata) > (int32_t)rxlen) {
							// 	YM_ERR("rx buffer overflow (exceeded 0x%08x)\r\n", (int32_t)rxlen);
							// 	goto rx_err_handler;
							// }
							// int32_t i;
							// for (i = 0; i < rx_packet_len; i++) {
							// 	rxptr[i] = rx_packet_data[PACKET_HEADER + i];
							// }
							rx_packet_len = filesize > rx_packet_len ? rx_packet_len : filesize;
							filesize -= rx_packet_len;
							ym_output_dest(&rx_packet_data[PACKET_HEADER], rx_packet_len, filename);
							// rxptr += rx_packet_len;
							_putchar(ACK);
						}
						packets_rxed++;
					}	// sequence number check ok
					break;
				} // default
				} // inner switch
				break;
			} // case 0
			default: {
				if (packets_rxed > 0) {
					nbr_errors++;
					if (nbr_errors >= PACKET_ERROR_MAX_NBR) {
						// YM_ERR("rx errors too many: %d - ABORT.\r\n", nbr_errors);
						goto rx_err_handler;
					} else {
						_putchar(NAK);
					}
				} else {
					packets_rxed = 0;
					_putchar(CRC);
				}
				break;
			} // default
			} // switch

			// check end of receive packets
		} while (! file_done);

		// check end of receive files
	} while (! session_done);

	ym_close_dest();
	// return bytes received
	return filesize;

 rx_err_handler:
 	ym_close_dest();
	_putchar(CAN);
	_putchar(CAN);
	_sleep(1);
	return YM_FAIL;
}

//------------------------------------
static void ym_send_packet(uint8_t *txdata, int32_t block_nbr)
{
	int32_t tx_packet_size;

	// We use a short packet for block 0 - all others are 1K
	if (block_nbr == 0) {
		tx_packet_size = PACKET_SIZE;
	} else {
		tx_packet_size = PACKET_1K_SIZE;
	}

	uint16_t crc16_val = ym_crc16(txdata, tx_packet_size);

	// 128 byte packets use SOH, 1K use STX
	_putchar( (block_nbr == 0) ? SOH : STX );
	// _putchar( (block_nbr == 0) ? STX : STX );
	// write seq numbers
	_putchar(block_nbr & 0xFF);
	_putchar(~block_nbr & 0xFF);

	// write txdata
	int32_t i;
	for (i = 0; i < tx_packet_size; i++) {
		_putchar(txdata[i]);
	}

	// write crc16
	_putchar((crc16_val >> 8) & 0xFF);
	_putchar(crc16_val & 0xFF);
}

//-----------------------------------------------
// Send block 0 (the filename block). filename might be truncated to fit.
static void ym_send_packet0(const char* filename, int32_t filesize)
{
	int32_t pos = 0;
	// put 256byte on stack, ok? reuse other stack mem?
	uint8_t block[PACKET_SIZE];
	if (filename) {
		// write filename
		while (*filename && (pos < PACKET_SIZE - FILE_SIZE_LENGTH - 2)) {
			block[pos++] = *filename++;
		}
		// z-term filename
		block[pos++] = 0;

		// write size, TODO: check if buffer can overwritten here.
		pos += ym_writeU32(filesize, &block[pos]);
	}

	// z-terminate string, pad with zeros
	while (pos < PACKET_SIZE) {
		block[pos++] = 0;
	}

	// send header block
	ym_send_packet(block, 0);
}

//-------------------------------------------------
static void ym_send_data_packets(uint32_t txlen, uint32_t timeout_sec, const char *filename)
{
	int32_t block_nbr = 1, buf_pt=0;
	char buffer[PACKET_1K_SIZE] = {0};

	while (txlen > 0) {
		// check if send full 1k packet
		uint32_t send_size=0;
		if (txlen > PACKET_1K_SIZE) {
			send_size = PACKET_1K_SIZE;
		} else {
			send_size = txlen;
		}
		send_size = ym_get_data(buffer, buf_pt, send_size, filename);
		// send packet
		ym_send_packet((uint8_t *)buffer, block_nbr);
		int32_t c = _getchar(timeout_sec);
		switch (c) {
			case ACK:
				buf_pt += send_size;
				txlen -= send_size;
				block_nbr++;
				break;
			case -1:
			case CAN:
				return;
			default:
				break;
		}
	}

	int32_t ch;
	do {
		_putchar(EOT);
		ch = _getchar(timeout_sec);
	} while ((ch != ACK) && (ch != -1));

	// Send last data packet
	if (ch == ACK) {
		ch = _getchar(timeout_sec);
		if (ch == CRC) {
			do {
				ym_send_packet0(0, 0);
				ch = _getchar(timeout_sec);
			} while ((ch != ACK) && (ch != -1));
		}
	}
}

//-------------------------------------------------------
int32_t fymodem_send(const char* filename)
{
	int32_t i = 0, txsize = 0;
	// Flush the RX FIFO, after a cool off delay
	_sleep(1);
	_flush();

	// Not in the specs, just for balance
	int32_t ch;
	for(i=0;i<PACKET_ERROR_MAX_NBR;i++) {
		do {
			_putchar(CRC);
			ch = _getchar(1);
		} while (ch < 0);

		// We require transfer with CRC
		if (ch == CRC) {
			break;
		}
	}
	// We require transfer with CRC
	if (ch != CRC) {
		YM_ERR("CRC ERROR: %c\r\n", ch);
		goto tx_err_handler;
	}

	bool crc_nak = true;
	bool file_done = false;

	txsize = ym_get_size(filename);
	/* when txsize is -1 , it means the file doesn't exists */
	if (txsize == -1) goto tx_err_handler;
	do {
		ym_send_packet0(filename, txsize);
		// When the receiving program receives this block and successfully
		// opened the output file, it shall acknowledge this block with an ACK
		// character and then proceed with a normal XMODEM file transfer
		// beginning with a "C" or NAK tranmsitted by the receiver.
		ch = _getchar(PACKET_RX_TIMEOUT_SEC);
		if (ch == ACK) {
			ch = _getchar(PACKET_RX_TIMEOUT_SEC);
			if (ch == CRC) {
				ym_send_data_packets(txsize, PACKET_RX_TIMEOUT_SEC, filename);
				// success
				file_done = true;
			}
		} else if ((ch == CRC) && (crc_nak)) {
			crc_nak = false;
			continue;
		} else if ((ch != NAK) || (crc_nak)) {
			goto tx_err_handler;
		}
	} while (! file_done);

	return txsize;

 tx_err_handler:
	_putchar(CAN);
	_putchar(CAN);
	_sleep(1);
	return YM_FAIL;
}
#endif /* NTSHELL_USE_CMDS_FS_YMODEM */