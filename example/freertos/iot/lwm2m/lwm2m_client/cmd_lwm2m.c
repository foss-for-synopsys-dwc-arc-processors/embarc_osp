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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "embARC.h"
#include "embARC_debug.h"

#include "lwm2mclient.h"
#include "lwip_wifi.h"

#ifndef USE_NTSHELL_EXTOBJ /* don't use ntshell extobj */
#define CMD_DEBUG(fmt, ...)	EMBARC_PRINTF(fmt, ##__VA_ARGS__)
#endif

#define TSKPRI_LWM2M_CLIENT	(configMAX_PRIORITIES-2)	/**< lwm2m task priority */

/* lwM2M client info */
static lwm2m_client_info c_info;
static int lwm2m_client_conn_stat = 0;
static int lwm2m_client_start_flag = 0;
static TaskHandle_t task_lwm2m_client_handle = NULL;

#if defined(BOARD_EMSK)
#define BOOTLOADER_STARTADDRESS		0x17f00000
typedef int (*fp_t)(void);

void go_bootloader(void)
{
	fp_t fp;
	fp = (fp_t)(*((uint32_t *)BOOTLOADER_STARTADDRESS));
	cpu_lock();

	for (int i = NUM_EXC_CPU; i < NUM_EXC_ALL; i++) {
		int_disable(i);
	}

	fp();

}

void task_lwm2m_update(void *par)
{
	handle_sigint(2);
	vTaskDelay(2000 / portTICK_RATE_MS);
	go_bootloader();
}
#endif


void task_lwm2m_client(void *par)
{
	unsigned int cpu_status;

	while (1) {
		lwm2m_client_conn_stat = 1;

		if (lwm2mclient(&c_info) == 0) {
			EMBARC_PRINTF("LwM2M client end successfully\r\n");
		} else {
			EMBARC_PRINTF("LwM2M client end failed\r\n");
		}

		cpu_status = cpu_lock_save();

		if (c_info.server != NULL) {
			free((void *)(c_info.server));
		}

		if (c_info.ep_name != NULL) {
			free((void *)(c_info.ep_name));
		}

		if (c_info.serverPort != NULL) {
			free((void *)(c_info.serverPort));
		}

		lwm2m_client_conn_stat = 0;
		cpu_unlock_restore(cpu_status);
		vTaskSuspend(NULL);
	}
}

#define min(x,y) ((x) < (y) ? (x) : (y))

/* show help of command */
static void cmd_lwm2m_client_help(char *cmd_name, void *extobj)
{
	NTSHELL_IO_PREDEF;

	VALID_EXTOBJ_NORTN(extobj);
	NTSHELL_IO_GET(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("usage: %s \r\n"
	          "	-q 			quit client\r\n"
	          "	-s <server_url>		set the lwM2M server URL\r\n"
	          "	-c <client_name>	set the lwM2M client endpoint name\r\n"
	          "	-p <server port>	set the LwM2M server port\r\n", cmd_name);

error_exit:
	return;
}

static char *malloc_and_copy(char *chr)
{
	int len = 0;
	char *p_str = NULL;

	len = strlen(chr) + 1;
	p_str = (char *)malloc(len);

	if (p_str == NULL) {
		EMBARC_PRINTF("Memory malloc failed.\r\n");
	} else {
		memcpy(p_str, chr, len);
	}

	return p_str;
}

static int cmd_lwm2m_client(int argc, char **argv, void *extobj)
{
	int ercd = E_OK;
	int opt;
	int opt_enter_cnt = 0;
	int c_quit = 0;
	char *p_server = NULL;
	char *p_client_name = NULL;
	char *p_port = NULL;

	NTSHELL_IO_PREDEF;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	opterr = 0;
	optind = 1;

	if (argc < 2) {
		cmd_lwm2m_client_help(argv[0], extobj);
		return E_OK;
	}

	if (!lwip_wifi_isup()) {
		CMD_DEBUG("Wifi is not ready for lwM2M client.\r\n");
		goto error_exit;
	}

	while ((opt=getopt(argc, argv, "s:c:p:qhH?")) != -1) {
		opt_enter_cnt ++;

		switch (opt) {
			case 'h':
			case '?':
			case 'H':
				cmd_lwm2m_client_help(argv[0], extobj);
				goto error_exit;
				break;

			case 'q':
				c_quit = 1;
				break;

			case 's':
				if (p_server == NULL) {
					p_server = malloc_and_copy(optarg);
				}

				break;

			case 'c':
				if (p_client_name == NULL) {
					p_client_name = malloc_and_copy(optarg);
				}

				break;

			case 'p':
				if (p_port == NULL) {
					p_port = malloc_and_copy(optarg);
				}

				break;

			default:
				opt_enter_cnt --;
				CMD_DEBUG("unrecognized option:%c\r\n", opt);
				break;
		}
	}

	if (c_quit == 1) {
		CMD_DEBUG("Try to exit existing client.\r\n");
		handle_sigint(2);
		goto error_exit;
	}

	if (lwm2m_client_conn_stat == 1) {
		CMD_DEBUG("LwM2M client is already running.\r\n");
		goto error_exit;
	}

	if (p_server == NULL) {
		CMD_DEBUG("Server Url is not specified, please check it.\r\n");
		goto error_exit;
	}

	c_info.server = p_server;
	c_info.ep_name = p_client_name;
	c_info.serverPort = p_port;

	lwm2m_client_start_flag = 1;
	CMD_DEBUG("Start LwM2M client.");

	if (task_lwm2m_client_handle == NULL) {
		if (xTaskCreate(task_lwm2m_client, "lwm2m client", 4096, (void *)1,
		                TSKPRI_LWM2M_CLIENT, &task_lwm2m_client_handle) != pdPASS) {
			EMBARC_PRINTF("create lwm2m client failed\r\n");
		}
	} else {
		vTaskResume(task_lwm2m_client_handle);
	}

	return E_OK;

error_exit:

	if (p_server != NULL) {
		free((void *)p_server);
	}

	if (p_client_name != NULL) {
		free((void *)p_client_name);
	}

	if (p_port != NULL) {
		free((void *)p_port);
	}

	return ercd;
}

static CMD_TABLE_T lwm2m_client_cmd = {"lwm2m_client", "connect to LwM2M server", cmd_lwm2m_client, NULL};
/**
 * register lwm2m_client command
 */
CMD_TABLE_T *register_ntshell_cmd_lwm2m_client(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&lwm2m_client_cmd, prev);
}