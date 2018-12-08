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
 * \defgroup	BOARD_EMSDP_DRV_MID_NTSHELL_IO_NETCONN	EMSDP NTShell NETCONN IO Driver
 * \ingroup	BOARD_EMSDP_DRV_MID_NTSHELL_IO
 * \brief	EMSDP NTShell Middleware NET Connection IO Driver
 * \details
 *		Implementation of middleware ntshell ntshell_io netconn driver, use net connection as
 *	ntshell interface.
 */

/**
 * \file
 * \ingroup	BOARD_EMSDP_DRV_MID_NTSHELL_IO_NETCONN
 * \brief	middleware ntshell io netconn interface driver
 */

/**
 * \addtogroup	BOARD_EMSDP_DRV_MID_NTSHELL_IO_NETCONN
 * @{
 */
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include <stdarg.h>

#if defined(MID_NTSHELL) && defined(MID_LWIP) \
	&& defined(ENABLE_OS)  /* only available when enable ntshell & lwip middleware */

#include "dev_uart.h"
#include "xprintf.h"

#include "ntshell_io_netconn.h"
#include "ntshell_task.h"

#include "board.h"

#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/api.h"
#include "lwip/pbuf.h"

#define  DEBUG
#include "embARC_debug.h"

#define EMSDP_NTSHELL_NETCONN_CHECK_EXP(EXPR, ERROR_CODE)	CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

#define CONN_NOTCONNECT		(0)
#define CONN_SUCCESS		(1)
#define CONN_LOST		(2)
#define CONN_PORT_EXIST		(3)
#define CONN_FAILED		(4)

#define CONN_CTRL_ECHO		(0x1)
#define CONN_CTRL_CRLF		(0x2)

/* TELNET option */

#define TELNET_OPT_SE		(240)
#define TELNET_OPT_NOP		(241)
#define TELNET_OPT_DM		(242)
#define TELNET_OPT_BRK		(243)
#define TELNET_OPT_IP		(244)
#define TELNET_OPT_AO		(245)
#define TELNET_OPT_AYT		(246)
#define TELNET_OPT_EC		(247)
#define TELNET_OPT_EL		(248)
#define TELNET_OPT_GA		(249)
#define TELNET_OPT_SB		(250)
#define TELNET_OPT_WILL		(251)
#define TELNET_OPT_WONT		(252)
#define TELNET_OPT_DO		(253)
#define TELNET_OPT_DONT		(254)
#define TELNET_OPT_IAC		(255)

typedef struct ntshell_io_netconn NTSHELL_IO_NETCONN, *NTSHELL_IO_NETCONN_PTR;
typedef struct netconn_thread NETCONN_THREAD, *NETCONN_THREAD_PTR;

struct netconn_thread {
	const char *thread_name;		/*!< thread name */
	uint32_t thread_prio;			/*!< thread priority */
	uint32_t thread_stksz;			/*!< thread stack size */
	void *thread_arg;			/*!< thread argument */
	void (* thread)(void *arg);		/*!< thread function */
};

struct ntshell_io_netconn {
	struct netconn *local_conn;
	struct netconn *remote_conn;
	uint32_t rcv_len;
	uint32_t rcv_ofs;
	struct pbuf *pbuf_rcv;
	uint32_t conn_port;
	int32_t conn_status;
	uint32_t conn_ctrl;
	NETCONN_THREAD *conn_thread;
	void (*write_byte)(unsigned char byte);
};

static uint8_t dont_echo[3] = {TELNET_OPT_IAC,TELNET_OPT_DONT,1};

static int32_t ntshell_netconn_nt_write(NTSHELL_IO_NETCONN *nt_netconn, const void *buf, uint32_t cnt);

static void ntshell_netconn_thread(void *arg)
{
	err_t ercd = E_OK;
	NTSHELL_IO *nt_io = (NTSHELL_IO *)arg;
	NTSHELL_IO_NETCONN *nt_netconn;
	struct netconn *temp_conn;
	uint8_t *p_dont_echo = &dont_echo[0];

	EMSDP_NTSHELL_NETCONN_CHECK_EXP(nt_io!=NULL, E_OBJ);
	EMSDP_NTSHELL_NETCONN_CHECK_EXP(nt_io->extra_info!=NULL, E_OBJ);

	nt_netconn = (NTSHELL_IO_NETCONN *)nt_io->extra_info;

	nt_netconn->local_conn = netconn_new(NETCONN_TCP);
	netconn_bind(nt_netconn->local_conn, NULL, 23);
	netconn_listen(nt_netconn->local_conn);

	nt_netconn->conn_status = CONN_NOTCONNECT;
	while (1) {
		ercd = netconn_accept(nt_netconn->local_conn, &temp_conn);
		nt_netconn->conn_status = CONN_NOTCONNECT;
		if (ercd == ERR_OK) {
			if (nt_netconn->remote_conn != NULL) { /* kill previous connection */
				sys_mbox_trypost(&nt_netconn->remote_conn->recvmbox, NULL);
				sys_msleep(200);
				netconn_close(nt_netconn->remote_conn);
				netconn_delete(nt_netconn->remote_conn);
				DBG("Delete old connection\n");
			}
			DBG("New connection connected\n");
			nt_netconn->remote_conn = temp_conn;
			nt_netconn->conn_status = CONN_SUCCESS;
			ntshell_netconn_nt_write(nt_netconn, p_dont_echo, 3);
		} else {
			nt_netconn->conn_status = CONN_FAILED;
		}
	}

error_exit:
	return;
}
static int32_t ntshell_netconn_nt_ioinit(NTSHELL_IO_NETCONN *nt_netconn)
{
	int32_t ercd = E_OK;

	EMSDP_NTSHELL_NETCONN_CHECK_EXP(nt_netconn!=NULL, E_OBJ);

	if (nt_netconn->conn_thread->thread_arg == NULL) {
		nt_netconn->conn_thread->thread_arg = (void *)nt_netconn;
	}
	sys_thread_new(nt_netconn->conn_thread->thread_name, nt_netconn->conn_thread->thread, \
		nt_netconn->conn_thread->thread_arg, nt_netconn->conn_thread->thread_stksz, \
		nt_netconn->conn_thread->thread_prio);

	while (nt_netconn->conn_status != CONN_SUCCESS) {
		sys_msleep(100);
	}
	ercd = E_OK;
error_exit:
	return ercd;
}

static int32_t netconn_nt_read_chr_raw(NTSHELL_IO_NETCONN *nt_netconn)
{
	int8_t rcv_chr;
	int ercd;
	if (nt_netconn->conn_status == CONN_SUCCESS) {
		if (nt_netconn->rcv_ofs >= nt_netconn->rcv_len) {
			if (nt_netconn->pbuf_rcv != NULL) { /* free it when valid */
				pbuf_free(nt_netconn->pbuf_rcv);
			}
			ercd = netconn_recv_tcp_pbuf(nt_netconn->remote_conn, &nt_netconn->pbuf_rcv);
			if (ercd == ERR_OK) {
				nt_netconn->rcv_ofs = 0;
				nt_netconn->rcv_len = nt_netconn->pbuf_rcv->tot_len;
			} else {
				nt_netconn->conn_status = CONN_LOST;
				return E_OBJ;
			}
		}
		if (nt_netconn->pbuf_rcv == NULL) {
			return E_OBJ;
		}
		pbuf_copy_partial(nt_netconn->pbuf_rcv, &rcv_chr, 1, nt_netconn->rcv_ofs);
		nt_netconn->rcv_ofs ++;
		return ((int32_t)rcv_chr) & 0xff;
	}
	/** error handling */
	if (nt_netconn->pbuf_rcv != NULL) { /* free it when valid */
		pbuf_free(nt_netconn->pbuf_rcv);
	}
	nt_netconn->rcv_ofs = 0;
	nt_netconn->rcv_len = 0;
	return E_OBJ;
}
static int32_t netconn_nt_read_chr(NTSHELL_IO_NETCONN *nt_netconn)
{
	int32_t ch, req;

	while ( (ch = netconn_nt_read_chr_raw(nt_netconn)) == TELNET_OPT_IAC \
			|| ch == '\0' && ch != E_OBJ) {
		if (ch != '\0') {
			switch (ch = netconn_nt_read_chr_raw(nt_netconn)) {
				case TELNET_OPT_WILL:
				case TELNET_OPT_WONT:
				case TELNET_OPT_DO:
				case TELNET_OPT_DONT:
					req = netconn_nt_read_chr_raw(nt_netconn);
					if (req != E_OBJ) {
						DBG("Telnet Request:%x\n\r", req);
					} else {
						return req;
					}
					break;
				case TELNET_OPT_SB:
					while ((ch = netconn_nt_read_chr_raw(nt_netconn)) != E_OBJ && ch != TELNET_OPT_IAC) {
						if ((ch = netconn_nt_read_chr_raw(nt_netconn)) == E_OBJ || ch == TELNET_OPT_SE) {
							break;
						}
					}
					break;
				default:
					break;
			}
		}
	}

	return ch;
}

static int32_t ntshell_netconn_nt_read(NTSHELL_IO_NETCONN *nt_netconn, void *buf, uint32_t cnt)
{
	int32_t ercd = E_OK;
	uint32_t reacnt = 0;
	uint8_t *p_charbuf = (uint8_t *)buf;

	EMSDP_NTSHELL_NETCONN_CHECK_EXP(nt_netconn!=NULL, E_OBJ);
	EMSDP_NTSHELL_NETCONN_CHECK_EXP(buf!=NULL, E_OBJ);

	while (reacnt < cnt) {
		ercd = netconn_nt_read_chr(nt_netconn);
		if (ercd == E_OBJ) {
			sys_msleep(10); /* yield for other process to run */
			break;
		} else {
			p_charbuf[reacnt] = (uint8_t)ercd & 0xff;
			reacnt ++;
		}
	}

	ercd = reacnt;

error_exit:
	return ercd;
}

static int32_t netconn_nt_write_chr(NTSHELL_IO_NETCONN *nt_netconn, int8_t chr)
{
	return netconn_write(nt_netconn->remote_conn, &chr, 1, NETCONN_COPY);
}

static int32_t ntshell_netconn_nt_write(NTSHELL_IO_NETCONN *nt_netconn, const void *buf, uint32_t cnt)
{
	int32_t ercd = E_OK;
	uint32_t wricnt = 0;
	uint8_t crlf_snd = 1, crlf_snd_save = 1;
	const uint8_t *p_charbuf = (const uint8_t *)buf;
	uint8_t sndchr = 0;

	EMSDP_NTSHELL_NETCONN_CHECK_EXP(nt_netconn!=NULL, E_OBJ);
	EMSDP_NTSHELL_NETCONN_CHECK_EXP(buf!=NULL, E_OBJ);

	while (wricnt < cnt) {
		crlf_snd_save = crlf_snd;
		sndchr = p_charbuf[wricnt];
		if (crlf_snd && sndchr == '\n' && nt_netconn->conn_ctrl & CONN_CTRL_CRLF) {
			crlf_snd = 0;
			sndchr = '\r';
		} else {
			crlf_snd = 1;
		}
		netconn_nt_write_chr(nt_netconn, sndchr);
		if (nt_netconn->conn_status != CONN_SUCCESS) {
			break;
		}
		wricnt += crlf_snd;
	}

	ercd = wricnt;
error_exit:
	return ercd;
}

static void ntshell_netconn_nt_printf(NTSHELL_IO_NETCONN *nt_netconn, const char *fmt, va_list arp)
{
	int32_t ercd = E_OK;
	void (*pf)(unsigned char);

	EMSDP_NTSHELL_NETCONN_CHECK_EXP(nt_netconn!=NULL, E_OBJ);

	if (nt_netconn->write_byte) {
		pf = xfunc_out;	 /* Save current output device */
		xfunc_out = nt_netconn->write_byte;	/* Switch output to specified device */
		xvprintf(fmt, arp);
		xfunc_out = pf;	 /* Restore output device */
	}

error_exit:
	return ;
}

static void ntshell_netconn_write_byte(NTSHELL_IO_NETCONN *nt_netconn, unsigned char byte)
{
	ntshell_netconn_nt_write(nt_netconn, (const void *)(&byte), 1);
}


#if USE_EMSDP_NTSHELL_NETCONN_0
static int32_t ntshell_netconn_0_nt_ioinit(void)
{
	return ntshell_netconn_nt_ioinit((NTSHELL_IO_NETCONN *)(ntshell_netconn_0.extra_info));
}

static int32_t ntshell_netconn_0_nt_read(void *buf, uint32_t cnt)
{
	return ntshell_netconn_nt_read((NTSHELL_IO_NETCONN *)(ntshell_netconn_0.extra_info), buf, cnt);
}

static int32_t ntshell_netconn_0_nt_write(const void *buf, uint32_t cnt)
{
	return ntshell_netconn_nt_write((NTSHELL_IO_NETCONN *)(ntshell_netconn_0.extra_info), buf, cnt);
}

static void ntshell_netconn_0_nt_printf(const char *fmt, ...)
{
	va_list arp;

	va_start(arp, fmt);
	ntshell_netconn_nt_printf((NTSHELL_IO_NETCONN *)(ntshell_netconn_0.extra_info), fmt, arp);
	va_end(arp);
}

static void ntshell_netconn_0_write_byte(unsigned char byte);

static NETCONN_THREAD netconn_0_thread = {
	"NT_NTCONN_0", NT_NETCONN_0_PRIO, 512, \
	(void *)(&ntshell_netconn_0), ntshell_netconn_thread
};

static ntshell_t  netconn_ntshell_0;
static NTSHELL_IO_NETCONN ntshell_netconn_0_info = {
	NULL, NULL, \
	0, 0, NULL, \
	23, CONN_NOTCONNECT, CONN_CTRL_ECHO|CONN_CTRL_CRLF, \
	&netconn_0_thread, ntshell_netconn_0_write_byte
};

static void ntshell_netconn_0_write_byte(unsigned char byte)
{
	ntshell_netconn_write_byte(&ntshell_netconn_0_info, byte);
}

NTSHELL_IO ntshell_netconn_0 = {
	&netconn_ntshell_0, \
	&help_cmd, \
	NULL, \
	"NETCONN0>", \
	(void *)(&ntshell_netconn_0_info), \
	ntshell_netconn_0_nt_ioinit, \
	ntshell_netconn_0_nt_read, \
	ntshell_netconn_0_nt_write, \
	ntshell_netconn_0_nt_printf
};

#endif /** USE_EMSDP_NTSHELL_NETCONN_0 */

#endif /** MID_NTSHELL MID_LWIP */

/** @} end of group BOARD_EMSDP_DRV_MID_NTSHELL_IO_NETCONN */
