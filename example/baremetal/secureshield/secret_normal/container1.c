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

#include <string.h>
#include <stdio.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_assert.h"

#include "container1.h"

extern SECRET_CONTEXT * challenge;

/* For secure initialization of secret and password data, these are stored in a separate section that other containers have no access to.
 * In an application with secure persistent storage like EEPROM or flash, it would be stored there.
 */
CONTAINER_RODATA(container1) const uint8_t secret_const_data[256] = {
     'e', 'm', 'b', 'a', 'r', 'c', 0, 'i', ' ','l', 'o', 'v', 'e', ' ', 'e', 'm', 'b', 'a', 'r', 'c', 0,};
CONTAINER_BSS(container1) SECRET_CONTEXT container1_context;

int32_t init_secret(void)
{
	SECRET_CONTEXT *ctx;
	int i, j;

	ctx = &container1_context;

	if (ctx->initialized == 0) {
		ctx->initialized = 1;
		ctx->fails = 0;

		// Copy initialization data to private context
		i = 0;
		while ((i<PWD_LEN) && (secret_const_data[i] != 0)) {
			ctx->pwd[i] = secret_const_data[i];
			i++;
		}
		memset(&(ctx->pwd[i]), 0, PWD_LEN-i);

		i++;
		j = 0;
		while ((j<SECRET_LEN) && (secret_const_data[i] != 0)) {
			ctx->secret[j++] = secret_const_data[i];
			i++;
		}
		memset(&(ctx->secret[j]), 0, SECRET_LEN-j);
	}

	// simulate the case of memory leakage; make available the pointer to the secret context through a global variable
	challenge = &container1_context;

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
		// Erase the secret
		memset(ctx->secret, 0, SECRET_LEN);
		return -1;
	}

	caller_container = secureshield_container_id_caller();

	if (caller_container < 0) {
		EMBARC_PRINTF("secureshield_container_id_caller error.\r\n");
		return -1;
	} else if (caller_container == 0) { // Check if caller is authorized to access the secret
		if (strcmp(pwd, (const char *)ctx->pwd) != 0) {
			return -1;
		}
		memset(pwd, 0, strlen(pwd));
	} else {
		EMBARC_PRINTF("container2 is trusted, no need to check password\r\n");
		// In this example container 2 data pointer should be in the special shared private region
		// Though container 2 is trusted, good practice to check anyway; addresses are defined in the linker file
		if (((uint32_t)data<CONTAINER_12_SHARED_ORIGIN ) || ((uint32_t)data+SECRET_LEN>CONTAINER_12_SHARED_ORIGIN+CONTAINER_12_SHARED_LENGTH ) ) {
			EMBARC_PRINTF("container2 provided an incorrect data pointer for storing the secret\r\n");
			return -1;
		}
	}

	// Caller is trusted now, perform the requested operation
	ctx->fails = 0;
	if (cmd == GET_SECRET) {
		// note that data is copied with Container 1 privileges to the memory pointer to by the 'data' argument
		// this could be a security risk, but since the caller is trusted the data pointer is trusted as well
		// (for container 2, the value of the pointer was checked as well above).
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
