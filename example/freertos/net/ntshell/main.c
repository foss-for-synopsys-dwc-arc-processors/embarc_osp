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

static TaskHandle_t net_ntshell_handle = NULL;
#define TSKPRI_NET_NTSHELL	(configMAX_PRIORITIES-3)	  /**< net ntshell task priority */

/* according to the board, configure the right ntshell id */

#define NTSHELL_ID_NET		1

int main(void)
{
	NTSHELL_IO *nt_io;

	EMBARC_PRINTF("NT-Shell for FreeRTOS + LwIP + FatFs\r\n");
	EMBARC_PRINTF("Now please telnet to the IPAddr @ Port %d, to run ntshell ver telnet\r\n", 23);
	EMBARC_PRINTF("Command line like this: telnet ipaddr port\r\n");

	nt_io = get_ntshell_io(NTSHELL_ID_NET);
	if (net_ntshell_handle == NULL) {
		xTaskCreate(ntshell_task, "net_ntshell", 2048, (void *)nt_io, TSKPRI_NET_NTSHELL, &net_ntshell_handle);
	}

	return 0;
}
