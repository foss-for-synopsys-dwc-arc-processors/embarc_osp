#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_assert.h"

#include "container2.h"

/* Container 2 : the trusted keystore client */
CONTAINER_BSS(container2) SECRET_CONTEXT2 container2_context;

void trusted_ops(void)
{
	uint8_t data[SECRET_LEN];
	SECRET_CONTEXT2 *ctx;

	ctx = &container2_context;

	EMBARC_PRINTF("container 2 is trusted\r\n");
	/* data is on container's  stack, normally other containers can not access it. As
	container1 is a secure container, it can access container 2's stack */
	if(container_call(container1, operate_secret, NULL, GET_SECRET, data)) {
		EMBARC_PRINTF("get secret failed - someone tried to hack the system?\r\n");
		return;
	}

	if (! ctx->initialized) {
		EMBARC_PRINTF("container 2: got the secret for the first time and stored it for private use\r\n");
		memcpy(ctx->secret, data, SECRET_LEN);
		ctx->initialized = 1;
	} else if (strcmp((const char *)data, (const char *)ctx->secret) != 0) {
		EMBARC_PRINTF("container 2: the secret changed, updated my private copy!\r\n");
		memcpy(ctx->secret, data, SECRET_LEN);
	} else {
		EMBARC_PRINTF("got the secret, but not telling you since its secret and for use within this container only...\r\n");
	}

}