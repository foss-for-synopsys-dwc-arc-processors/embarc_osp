/*------------------------------------------
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
 * \file
 * \ingroup	EMBARC_APP_BAREMETAL_SECURESHIELD_SECRET_V2_SID
 * \brief	secureshield secure container 1
 */
#undef LIB_SECURESHIELD_OVERRIDES
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_assert.h"

#include "container1.h"
#include "secureshield_sys.h"
#include "secureshield_int.h"


/* Container 1 : the keystore component */

/* non-secure containers cannot access pwd and secret, as they are in secure container data section through the use of the SECURE_DATA */
uint8_t container1_pwd[PWD_LEN+1] = {'e','m','b','a','r','c'};
uint8_t container1_secret[SECRET_LEN+1] = {'i', ' ','l', 'o', 'v', 'e', ' ', 'e', 'm',
	'b', 'a', 'r', 'c'};


SECRET_CONTEXT container1_context;


static void timer1_interrupt1(void *p_exinf)
{
	uint32_t val;
	val = _arc_aux_read(AUX_TIMER1_CTRL);
	val &= ~TIMER_CTRL_IP;
	_arc_aux_write(AUX_TIMER1_CTRL, val);
	//Asm("kflag 1");
}



int32_t init_secret(void)
{
	SECRET_CONTEXT *ctx;

	ctx = &container1_context;

	if (ctx->initialized == 0) {
		ctx->initialized = 1;
		ctx->fails = 0;
		ctx->pwd = container1_pwd;
		ctx->secret = container1_secret;


		_arc_aux_write(AUX_TIMER1_CTRL, 0);
		_arc_aux_write(AUX_TIMER1_LIMIT, 5000);
		_arc_aux_write(AUX_TIMER1_CTRL, TIMER_CTRL_IE|TIMER_CTRL_NH);
		_arc_aux_write(AUX_TIMER1_CNT, 0);

		secure_int_handler_install(INTNO_TIMER1, timer1_interrupt1);
		secure_int_pri_set(INTNO_TIMER1, INT_PRI_MIN);
		secure_int_enable(INTNO_TIMER1);
	}

	secure_int_sw_trigger(INTNO_SWI);

	return 0;
}

// Note that some parameters of this function are pointers. Care should be taken that Container 1 has access to the memory pointed at
int32_t operate_secret(char* pwd, uint32_t cmd, char * data)
{
	SECRET_CONTEXT *ctx;
	uint32_t len;
	int32_t caller_container;

	ctx = &container1_context;

	if (ctx->initialized == 0) {
		return -1;
	}

	// protect against brute force password guessing; for better protection, number of fails should be stored persistently.
	if (ctx->fails++ > 3) {
		EMBARC_PRINTF("Too many failed attempts to obtain secret.\r\n");
		// optionally, the secret could be erased here
		return -1;
	}

	caller_container = secure_container_id_caller();

	if (caller_container < 0) {
		EMBARC_PRINTF("containre_id_caller error\r\n");
		return -1;
	} else if (caller_container == 0) { // Check if caller is authorized to access the secret
		if (strcmp(pwd, (const char *)ctx->pwd) != 0) {
			return -1;
		}
		memset(pwd, 0, strlen(pwd));
	} else {
		EMBARC_PRINTF("container2 is trusted, no need to check password\r\n");
	}

	// Caller is trusted now, perform the requested operation
	ctx->fails = 0;
	if (cmd == GET_SECRET) {
		// note that data is copied with Container 1 privileges to the memory pointer to by the 'data' argument
		// this could be a security risk, but since the caller is trusted the data pointer is trusted as well
		memcpy(data, ctx->secret, SECRET_LEN);
	} else if (cmd == SET_SECRET) {
		memset(ctx->secret, 0, SECRET_LEN);
		len = strlen(data);
		if (len > SECRET_LEN) {
			len = SECRET_LEN;
		}
		memcpy(ctx->secret, data, len);
		memset(data, 0, len);
	} else if (cmd == SET_PWD) {
		memset(ctx->pwd, 0, PWD_LEN);
		len = strlen(data);
		if (len > PWD_LEN) {
			len = PWD_LEN;
		}
		memcpy(ctx->pwd, data, len);
		memset(data, 0, len);
	} else {
		return -1;
	}

	return 0;
}