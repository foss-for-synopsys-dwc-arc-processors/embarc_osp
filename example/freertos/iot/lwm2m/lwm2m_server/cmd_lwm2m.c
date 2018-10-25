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
 * \file
 * \ingroup	EMBARC_APP_LWIP_FREERTOS_LWM2M_SERVER
 * \brief	NTShell Command:lwm2m_server and lwm2m_op implementation
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "embARC.h"
#include "embARC_debug.h"

#include "lwm2mserver.h"
#include "lwip_wifi.h"

#ifndef USE_NTSHELL_EXTOBJ /* don't use ntshell extobj */
#define CMD_DEBUG(fmt, ...)			EMBARC_PRINTF(fmt, ##__VA_ARGS__)
#endif

#define TSKPRI_LWM2M_SERVER	(TCPIP_THREAD_PRIO-1)		/**< lwm2m task priority */

/* lwM2M server info */
static lwm2m_server_info c_info;
static int lwm2m_server_conn_stat = 0;
static int lwm2m_server_start_flag = 0;
static TaskHandle_t task_lwm2m_server_handle = NULL;

#define LWM2M_CMP	strcmp

void task_lwm2m_server(void *par)
{
	unsigned int cpu_status;
	while (1) {
		lwm2m_server_conn_stat = 1;
		if (lwm2mserver(&c_info) == 0) {
			EMBARC_PRINTF("LwM2M server end successfully\r\n");
		} else {
			EMBARC_PRINTF("LwM2M server end failed\r\n");
		}
		cpu_status = cpu_lock_save();
		if (c_info.serverPort != 0) c_info.serverPort = 0;
		lwm2m_server_conn_stat = 0;
		cpu_unlock_restore(cpu_status);
		vTaskSuspend(NULL);
	}
}

#define min(x,y) ((x) < (y) ? (x) : (y))

/* show help of command */
static void cmd_lwm2m_server_help(char *cmd_name, void *extobj)
{
	NTSHELL_IO_PREDEF;

	VALID_EXTOBJ_NORTN(extobj);
	NTSHELL_IO_GET(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}
	CMD_DEBUG("usage: %s \r\n"
		"	-q 			quit server\r\n"
		"	-p <server port>	set the LwM2M server port\r\n", cmd_name);

error_exit:
	return;
}

static int cmd_lwm2m_server(int argc, char **argv, void *extobj)
{
	int ercd = E_OK;
	int opt;
	int opt_enter_cnt = 0;
	int c_quit = 0;
	long p_port = -1;

	NTSHELL_IO_PREDEF;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	opterr = 0;
	optind = 1;

	if (argc < 1) {
		cmd_lwm2m_server_help(argv[0], extobj);
		return E_OK;
	}

	if (!lwip_wifi_isup()) {
		CMD_DEBUG("Wifi is not ready for lwM2M server.\r\n");
		goto error_exit;
	}
	while ((opt=getopt(argc, argv, "p:qhH?")) != -1) {
		opt_enter_cnt ++;
		switch (opt) {
			case 'h':
			case '?':
			case 'H':
				cmd_lwm2m_server_help(argv[0], extobj);
				return E_OK;
				break;
			case 'q':
				c_quit = 1;
				break;
			case 'p':
				if (xatoi((char **)(&optarg), (long *)(&p_port)) != 1) {
					CMD_DEBUG("Please check the port number is valid!\r\n");
					return E_PAR;
				}
				break;
			default:
				opt_enter_cnt --;
				CMD_DEBUG("unrecognized option:%c\r\n", opt);
				break;
		}
	}
	if (c_quit == 1) {
		CMD_DEBUG("Try to exit existing server.\r\n");
		handle_sigint(2);
		goto error_exit;
	}
	if (lwm2m_server_conn_stat == 1) {
		CMD_DEBUG("LwM2M server is already running.\r\n");
		goto error_exit;
	}

	c_info.serverPort = p_port;

	lwm2m_server_start_flag = 1;
	CMD_DEBUG("Start LwM2M server.");

	if (task_lwm2m_server_handle == NULL) {
		if (xTaskCreate(task_lwm2m_server, "lwm2m server", 8192, \
			(void *)1, TSKPRI_LWM2M_SERVER, &task_lwm2m_server_handle) != pdPASS) {
			EMBARC_PRINTF("create lwm2m server failed\r\n");
		}
	} else {
		vTaskResume(task_lwm2m_server_handle);
	}

	return E_OK;

error_exit:
	return ercd;
}

/* show help of command */
static void cmd_lwm2m_op_help(char *cmd_name, void *extobj)
{
	NTSHELL_IO_PREDEF;

	VALID_EXTOBJ_NORTN(extobj);
	NTSHELL_IO_GET(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}
	CMD_DEBUG("usage: %s \r\n"
		"	-o <operation method>	set operation method(list/read/write/exec/create/delete/observe/cancel)\r\n"
		"	-c <client ID>		set client ID to be operated\r\n"
		"	-r <resource URI>	set URI of the resource or instance\r\n"
		"	-d <resource DATA>	set data to be used\r\n", cmd_name);

error_exit:
	return;
}

static lwm2m_handler get_op_handler(char *op_method, lwm2m_op_info *op_info)
{
	if ((op_method == NULL) || (op_info == NULL)) {
		return NULL;
	} else {
		if (LWM2M_CMP(op_method, LWM2M_OP_METHOD_LIST) == 0) {
			return (lwm2m_handler)lwm2m_server_output_clients;
		} else if (LWM2M_CMP(op_method, LWM2M_OP_METHOD_READ) == 0) {
			if (op_info->clientId < 0) return NULL;
			if (op_info->r_uri == NULL) return NULL;
			return (lwm2m_handler)lwm2m_server_read_client;
		} else if (LWM2M_CMP(op_method, LWM2M_OP_METHOD_WRITE) == 0) {
			if (op_info->clientId < 0) return NULL;
			if (op_info->r_uri == NULL) return NULL;
			if (op_info->r_data == NULL) return NULL;
			return (lwm2m_handler)lwm2m_server_write_client;
		} else if (LWM2M_CMP(op_method, LWM2M_OP_METHOD_EXEC) == 0) {
			if (op_info->clientId < 0) return NULL;
			if (op_info->r_uri == NULL) return NULL;
			return (lwm2m_handler)lwm2m_server_exec_client;
		} else if (LWM2M_CMP(op_method, LWM2M_OP_METHOD_CREATE) == 0) {
			if (op_info->clientId < 0) return NULL;
			if (op_info->r_uri == NULL) return NULL;
			if (op_info->r_data == NULL) return NULL;
			return (lwm2m_handler)lwm2m_server_create_client;
		} else if (LWM2M_CMP(op_method, LWM2M_OP_METHOD_DELETE) == 0) {
			if (op_info->clientId < 0) return NULL;
			if (op_info->r_uri == NULL) return NULL;
			return (lwm2m_handler)lwm2m_server_delete_client;
		} else if (LWM2M_CMP(op_method, LWM2M_OP_METHOD_OBSERVE) == 0) {
			if (op_info->clientId < 0) return NULL;
			if (op_info->r_uri == NULL) return NULL;
			return (lwm2m_handler)lwm2m_server_observe_client;
		} else if (LWM2M_CMP(op_method, LWM2M_OP_METHOD_CANCEL) == 0) {
			if (op_info->clientId < 0) return NULL;
			if (op_info->r_uri == NULL) return NULL;
			return (lwm2m_handler)lwm2m_server_cancel_client;
		} else {
			return NULL;
		}
	}
}

static int cmd_lwm2m_op(int argc, char **argv, void *extobj)
{
	int ercd = E_OK;
	int opt;
	int opt_enter_cnt = 0;
	char *op_method = NULL;
	lwm2m_op_info op_info_local = {-1, NULL, NULL};
	lwm2m_op_info *op_info = &op_info_local;
	lwm2m_handler op_handler;

	NTSHELL_IO_PREDEF;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	opterr = 0;
	optind = 1;

	if (argc < 2) {
		cmd_lwm2m_op_help(argv[0], extobj);
		return E_OK;
	}

	if (!lwip_wifi_isup()) {
		CMD_DEBUG("Wifi is not ready for lwM2M server.\r\n");
		goto error_exit;
	}
	if (lwm2m_server_conn_stat == 0) {
		CMD_DEBUG("lwm2m server is not running, please run it first using command:lwm2m_server.\r\n");
		goto error_exit;
	}
	while ((opt=getopt(argc, argv, "o:c:r:d:hH?")) != -1) {
		opt_enter_cnt ++;
		switch (opt) {
			case 'h':
			case '?':
			case 'H':
				cmd_lwm2m_op_help(argv[0], extobj);
				return E_OK;
				break;
			case 'o':
				op_method = optarg;
				break;
			case 'c':
				if (xatoi((char **)(&optarg), (long *)(&op_info->clientId)) != 1) {
					CMD_DEBUG("Please check the client ID is valid!\r\n");
					return E_PAR;
				}
				break;
			case 'r':
				op_info->r_uri = optarg;
				break;
			case 'd':
				op_info->r_data = optarg;
				break;
			default:
				opt_enter_cnt --;
				CMD_DEBUG("unrecognized option:%c\r\n", opt);
		}
	}

	op_handler = get_op_handler(op_method, op_info);
	if (op_handler == NULL) {
		CMD_DEBUG("Selected operation is not supported, or miss some other data.\r\n");
	} else {
		op_handler(op_info);
	}

	return E_OK;

error_exit:
	return ercd;
}

static CMD_TABLE_T lwm2m_server_cmd = {"lwm2m_server", "Start LwM2M server", cmd_lwm2m_server, NULL};
static CMD_TABLE_T lwm2m_op_cmd = {"lwm2m_op", "do some operations when LwM2M server is online", cmd_lwm2m_op, NULL};

/**
 * register lwm2m server related command
 */
CMD_TABLE_T * register_ntshell_cmd_lwm2m_server(CMD_TABLE_T *prev)
{
	prev = ntshell_usrcmd_register(&lwm2m_server_cmd, prev);
	return ntshell_usrcmd_register(&lwm2m_op_cmd, prev);
}

