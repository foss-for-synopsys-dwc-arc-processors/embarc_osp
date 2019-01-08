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

#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_assert.h"

#include "container1.h"
#include "container2.h"

// In this example, for communicating secrets with container 1 a special section is used. Shared between container 1 and 2, others cannot access.
CONTAINER_BSS(container12_shared) uint8_t private_shared_data[SECRET_LEN];
CONTAINER_BSS(container2) SECRET_CONTEXT2 container2_context;

void trusted_ops(void)
{
	SECRET_CONTEXT2 *ctx;

	ctx = &container2_context;

	EMBARC_PRINTF("container 2 is trusted\r\n");
	if(container_call(container1, operate_secret, NULL, GET_SECRET, private_shared_data)) {
		EMBARC_PRINTF("get secret failed - someone tried to hack the system?\r\n");
		return;
	}

	if (! ctx->initialized) {
		EMBARC_PRINTF("container 2: got the secret for the first time and stored it for private use\r\n");
		memcpy(ctx->secret, private_shared_data, SECRET_LEN);
		ctx->initialized = 1;
	} else if (strcmp((const char *)private_shared_data, (const char *)ctx->secret) != 0) {
		EMBARC_PRINTF("container 2: the secret changed, updated my private copy!\r\n");
		memcpy(ctx->secret, private_shared_data, SECRET_LEN);
	} else {
		EMBARC_PRINTF("got the secret, but not telling you since its secret and for use within this container only...\r\n");
	}

	// though protected, don't leave secrets in shared memory if not needed
	memset(private_shared_data, 0, SECRET_LEN);
}
