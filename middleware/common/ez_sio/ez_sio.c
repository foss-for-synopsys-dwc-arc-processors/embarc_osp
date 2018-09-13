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
--------------------------------------------- */
#include <stdlib.h>

#include "embARC_error.h"
#include "arc_exception.h"
#include "ez_sio.h"

#define BUFSZ_LOCAL	32

const uint32_t ez_disabled = DEV_DISABLED;
const uint32_t ez_enabled = DEV_ENABLED;

static void sio_tx_callback(void *ptr)
{
	DEV_UART *uart_obj;
	DEV_UART_INFO *uart_info;
	EZ_SIO *sio;
	uint32_t wr_avail, wr_cnt, poped_cnt;
	uint8_t rbbuf[BUFSZ_LOCAL];

	if (ptr == NULL) return;

	uart_obj = (DEV_UART *)ptr;
	uart_info = &(uart_obj->uart_info);
	sio = (EZ_SIO *)DEV_UART_INFO_GET_EXTRA_OBJECT(uart_info);

	if (sio == NULL) return;


	RINGBUFFER *snd_rb;
	snd_rb = &(sio->snd_rb);
	uart_obj->uart_control(UART_CMD_GET_TXAVAIL, (void *)(&wr_avail));

	while (wr_avail > 0) {
		wr_cnt = (wr_avail < BUFSZ_LOCAL) ? wr_avail : BUFSZ_LOCAL;
		poped_cnt = rb_pop(snd_rb, (rb_buftype *)rbbuf, wr_cnt);
		if (poped_cnt > 0) {
			uart_obj->uart_write((void *)rbbuf, poped_cnt);
		} else { /* no more items in ringbuffer, disable tx interrupt */
			uart_obj->uart_control(UART_CMD_SET_TXINT, (void *)(DEV_DISABLED));
		}
		if (poped_cnt != wr_cnt) {
			break;
		}
		uart_obj->uart_control(UART_CMD_GET_TXAVAIL, (void *)(&wr_avail));
	}
}

static void sio_rx_callback(void *ptr)
{
	DEV_UART *uart_obj;
	DEV_UART_INFO *uart_info;
	EZ_SIO *sio;
	uint32_t rd_avail, rd_cnt, pushed_cnt;
	uint8_t rbbuf[BUFSZ_LOCAL];

	if (ptr == NULL) return;

	uart_obj = (DEV_UART *)ptr;
	uart_info = &(uart_obj->uart_info);
	sio = (EZ_SIO *)DEV_UART_INFO_GET_EXTRA_OBJECT(uart_info);

	if (sio == NULL) return;


	RINGBUFFER *rcv_rb;
	rcv_rb = &(sio->rcv_rb);
	uart_obj->uart_control(UART_CMD_GET_RXAVAIL, (void *)(&rd_avail));
	while (rd_avail > 0) {
		rd_cnt = (rd_avail < BUFSZ_LOCAL) ? rd_avail : BUFSZ_LOCAL;
		if (rb_isfull(rcv_rb) == 0) {
			uart_obj->uart_read((void *)rbbuf, rd_cnt);
			pushed_cnt = rb_push(rcv_rb, (rb_buftype *)rbbuf, rd_cnt);
		} else {
			pushed_cnt = 0;
		}
		if (pushed_cnt != rd_cnt) {
			break;
		}
		uart_obj->uart_control(UART_CMD_GET_RXAVAIL, (void *)(&rd_avail));
	}

}

EZ_SIO *ez_sio_open(uint32_t uart_id, uint32_t baudrate, uint32_t tx_bufsz, uint32_t rx_bufsz)
{
	EZ_SIO *sio = NULL;
	rb_buftype *ptr;
	uint32_t bufsz = tx_bufsz + rx_bufsz;

	if (bufsz == 0) return NULL;

	DEV_UART *uart_obj = uart_get_dev(uart_id);
	if (uart_obj == NULL) return NULL;

	int32_t ercd = uart_obj->uart_open(baudrate);
	if ((ercd != E_OK) && (ercd != E_OPNED))  return NULL;
	/**open uart port, if already opened, then set baudrate*/
	if (ercd == E_OPNED) {
		ercd = uart_obj->uart_control(UART_CMD_SET_BAUD, (void *)(baudrate));
		if (ercd != E_OK)  return NULL;
	}

	sio = (EZ_SIO *)malloc(sizeof(EZ_SIO));
	if (sio) {
		ptr = (rb_buftype *)malloc(bufsz * sizeof(rb_buftype));
		if (ptr) {
			sio->sio_uart_obj = uart_obj;

			rb_init(&(sio->snd_rb), &ptr[0], tx_bufsz);
			rb_init(&(sio->rcv_rb), &ptr[tx_bufsz], rx_bufsz);

			/* disable all interrupt first */
			uart_obj->uart_control(UART_CMD_SET_TXINT, (void *)(DEV_DISABLED));
			uart_obj->uart_control(UART_CMD_SET_RXINT, (void *)(DEV_DISABLED));

			/* set callback functions and enable tx/rx interrupts */
			uart_obj->uart_control(UART_CMD_SET_TXINT_BUF, NULL);
			uart_obj->uart_control(UART_CMD_SET_RXINT_BUF, NULL);
			uart_obj->uart_control(UART_CMD_SET_TXCB, (void *)sio_tx_callback);
			uart_obj->uart_control(UART_CMD_SET_RXCB, (void *)sio_rx_callback);

			DEV_UART_INFO_SET_EXTRA_OBJECT(&(uart_obj->uart_info), sio);
			uart_obj->uart_control(UART_CMD_SET_RXINT, (void *)(DEV_ENABLED));
		} else {
			free(sio);
			uart_obj->uart_close();
			sio = NULL;
		}
	} else {
		uart_obj->uart_close();
	}

	return sio;
}

void ez_sio_close(EZ_SIO *sio)
{
	if (sio == NULL) return;

	DEV_UART *uart_obj = sio->sio_uart_obj;

	uart_obj->uart_control(UART_CMD_SET_TXINT, (void *)(DEV_DISABLED));
	uart_obj->uart_control(UART_CMD_SET_RXINT, (void *)(DEV_DISABLED));
	uart_obj->uart_control(UART_CMD_SET_TXCB, NULL);
	uart_obj->uart_control(UART_CMD_SET_RXCB, NULL);
	uart_obj->uart_close();
	if (sio->snd_rb.buf) {
		free(sio->snd_rb.buf);
	}
	free(sio);
}

int32_t ez_sio_read(EZ_SIO *sio, char *buf, uint32_t cnt)
{
	int32_t rd_cnt = 0;

	if (sio == NULL) return rd_cnt;

	uint32_t cpu_status;
	RINGBUFFER *rcv_rb;

	rcv_rb = &(sio->rcv_rb);
	cpu_status = cpu_lock_save();
	rd_cnt = rb_pop(rcv_rb, (rb_buftype *)buf, cnt);
	cpu_unlock_restore(cpu_status);

	return rd_cnt;
}

int32_t ez_sio_write(EZ_SIO *sio, char *buf, uint32_t cnt)
{
	int32_t wr_cnt = 0;
	int32_t wr_avail = 0;

	if (sio == NULL) return wr_cnt;

	uint32_t cpu_status;
	RINGBUFFER *snd_rb;
	DEV_UART *uart_obj = sio->sio_uart_obj;

	cpu_status = cpu_lock_save();
	snd_rb = &(sio->snd_rb);

	if ((rb_isempty(snd_rb) == 1) && (cnt > 0)) {
		uart_obj->uart_control(UART_CMD_GET_TXAVAIL, (void *)(&wr_avail));
		if (wr_avail > 0) {
			wr_cnt = (wr_avail > cnt) ? cnt : wr_avail;
			uart_obj->uart_write(buf, wr_cnt);
			cnt = (wr_avail > cnt) ? 0 :(cnt - wr_avail);
		}
	}
	if (cnt > 0) {
		wr_cnt += rb_push(snd_rb, (rb_buftype *)(&buf[wr_cnt]), cnt);
		uart_obj->uart_control(UART_CMD_SET_TXINT, (void *)(DEV_ENABLED));
	}
	cpu_unlock_restore(cpu_status);

	return wr_cnt;
}
