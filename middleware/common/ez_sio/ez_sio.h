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
#ifndef _EZ_SIO_H_
#define _EZ_SIO_H_

#include "stdint.h"
#include "ringbuffer.h"
#include "dev_uart.h"

/** easy serial io structure */
typedef struct ez_sio {
	DEV_UART *sio_uart_obj;
	RINGBUFFER snd_rb;
	RINGBUFFER rcv_rb;
} EZ_SIO;

extern EZ_SIO *ez_sio_open(uint32_t uart_id, uint32_t baudrate, uint32_t tx_bufsz, uint32_t rx_bufsz);
extern void ez_sio_close(EZ_SIO *sio);
extern int32_t ez_sio_read(EZ_SIO *sio, char *buf, uint32_t cnt);
extern int32_t ez_sio_write(EZ_SIO *sio, char *buf, uint32_t cnt);

#endif /* _EZ_SIO_H_ */
