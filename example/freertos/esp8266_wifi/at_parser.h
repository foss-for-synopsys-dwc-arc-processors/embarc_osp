/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

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

#ifndef _AT_PARSER_H_
#define _AT_PARSER_H_

#include "ez_sio.h"
#include "embARC_error.h"

#define AT_OK		0
#define AT_ERROR	-1
#define AT_OK_STR	"OK"
#define AT_ERROR_STR	"ERROR"

#define AT_RX_BUFSIZE	512
#define AT_TX_BUFSIZE	128

#define AT_NORMAL_TIMEOUT	100
#define AT_LONG_TIMEOUT		5000
#define AT_EXTRA_TIMEOUT	20000

typedef enum {
	AT_LIST,
	AT_READ,
	AT_WRITE,
	AT_EXECUTE
} AT_MODE;

typedef char *AT_STRING;

/** AT_PARSER type */
typedef struct {
	uint32_t uart_id;
	EZ_SIO *psio;
} AT_PARSER_DEF, *AT_PARSER_DEF_PTR;

#define AT_PARSER_DEFINE(NAME, UART_ID) \
	AT_PARSER_DEF __ ## NAME = { \
	                             .uart_id = UART_ID, \
	                             .psio = NULL, \
	                           }; \
	AT_PARSER_DEF_PTR NAME = &__ ## NAME

int32_t at_parser_init(AT_PARSER_DEF_PTR obj, uint32_t baudrate);
void at_parser_deinit(AT_PARSER_DEF_PTR obj);
int32_t at_read(AT_PARSER_DEF_PTR obj, char *buf, uint32_t cnt);
int32_t at_write(AT_PARSER_DEF_PTR obj, char *buf, uint32_t cnt);
int32_t at_send_cmd(AT_PARSER_DEF_PTR obj, AT_MODE mode, AT_STRING command, ...);
int32_t at_get_reply(AT_PARSER_DEF_PTR obj, char *buf, uint32_t timeout);

int32_t at_test(AT_PARSER_DEF_PTR obj);

#endif /*_AT_PARSER_H_*/