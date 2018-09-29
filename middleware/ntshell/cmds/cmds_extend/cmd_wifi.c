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
* \brief operate PMOD WiFi module
*/
#include "cmds_extend_cfg.h"
#if NTSHELL_USE_CMDS_EXTEND_WIFI

#include "ntlibc.h"
#include "ntshell_common.h"

static NTSHELL_IO_PREDEF;

static void cmd_wifi_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION] <PARA>\r\n"
		"Wifi operations, such as set wifi ssid, connect the specified wifi\r\n"
		"  -s <start/stop/result>	Start/Stop scan network, or get scan result\r\n"
		"  -c 			Connect to wifi with default configured ssid and passwd\r\n"
		"  -d				Disconnect wifi\r\n"
		"  -w <ssid>		Set default wifi ssid\r\n"
		"  -p <passwd>	Set default passwd of ssid\r\n"
		"  -t <security_type>	Set wifi security type\r\n"
		"Available Security Type(%d:OPEN   %d:WPA    %d:WPA2    %d:WPA-AUTO)\r\n"
		"  -i 			1. init wifi device(when wifi is uninited) 2. show default ssid settings\r\n"
		"Examples: \r\n"
		"  wifi -c -w <ssid>    Set default wifi ssid, and connect\r\n"
		"  wifi -h    Show the help information\r\n"
		, cmd_name, \
		AUTH_SECURITY_OPEN,
		AUTH_SECURITY_WPA_WITH_PASS_PHRASE,
		AUTH_SECURITY_WPA2_WITH_PASS_PHRASE,
		AUTH_SECURITY_WPA_AUTO_WITH_PASS_PHRASE
		);

error_exit:
	return;
}

#define CMD_SCAN_START		1
#define CMD_SCAN_STOP		2
#define CMD_GET_SCAN_RST	3

#define TASK_WIFIPS_PRIO	(configMAX_PRIORITIES-1)
static TaskHandle_t task_wifips_handle = NULL;
static WNIC_SCAN_RESULT scan_result_data;

static void task_wifi_process(void *par)
{
	DEV_WNIC *pmwifi_wnic;

	pmwifi_wnic = (DEV_WNIC *)par;

	pmwifi_wnic->wnic_init(WNIC_NETWORK_TYPE_INFRASTRUCTURE);

	while (1) {
		pmwifi_wnic->period_process(NULL);
		vTaskDelay(10);
	}
}

int dump_wifi_security(int security, void *extobj)
{
	int ercd = 0;
	NTSHELL_IO_PREDEF;
	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	CMD_DEBUG("Security Type: ");

	switch (security) {
		case AUTH_SECURITY_OPEN                      :
			CMD_DEBUG(" %s\r\n", "OPEN                     ");
			break;

		case AUTH_SECURITY_WEP_40                    :
			CMD_DEBUG(" %s\r\n", "WEP_40                   ");
			break;

		case AUTH_SECURITY_WEP_104                   :
			CMD_DEBUG(" %s\r\n", "WEP_104                  ");
			break;

		case AUTH_SECURITY_WPA_WITH_KEY              :
			CMD_DEBUG(" %s\r\n", "WPA_WITH_KEY             ");
			break;

		case AUTH_SECURITY_WPA_WITH_PASS_PHRASE      :
			CMD_DEBUG(" %s\r\n", "WPA_WITH_PASS_PHRASE     ");
			break;

		case AUTH_SECURITY_WPA2_WITH_KEY             :
			CMD_DEBUG(" %s\r\n", "WPA2_WITH_KEY            ");
			break;

		case AUTH_SECURITY_WPA2_WITH_PASS_PHRASE     :
			CMD_DEBUG(" %s\r\n", "WPA2_WITH_PASS_PHRASE    ");
			break;

		case AUTH_SECURITY_WPA_AUTO_WITH_KEY         :
			CMD_DEBUG(" %s\r\n", "WPA_AUTO_WITH_KEY        ");
			break;

		case AUTH_SECURITY_WPA_AUTO_WITH_PASS_PHRASE :
			CMD_DEBUG(" %s\r\n", "WPA_AUTO_WITH_PASS_PHRASE");
			break;

		case AUTH_SECURITY_WPS_PUSH_BUTTON           :
			CMD_DEBUG(" %s\r\n", "WPS_PUSH_BUTTON          ");
			break;

		case AUTH_SECURITY_WPS_PIN                   :
			CMD_DEBUG(" %s\r\n", "WPS_PIN                  ");
			break;

		default:
			CMD_DEBUG(" Not existed\r\n");
			ercd = -1;
			break;
	}

error_exit:
	return ercd;
}

static int cmd_wifi(int argc, char **argv, void *extobj)
{
	int ercd = E_OK;
	int opt;
	/* wifi operation request */
	int i = 0;
	int scan_request = 0;
	int connect_request = 0;
	int disconnect_request = 0;
	int init_request = 0;
	int scan_rst_cnt = 0;

	int opt_enter_cnt = 0;

	static uint8_t wifi_init_status = 0;
	static uint8_t during_scan = 0;
	static uint8_t ssid_is_set = 0;
	static uint8_t default_ssid[WNIC_SSID_MAX_LEN] = "\0";
	static uint8_t key_is_set = 0;
	static uint8_t default_key[20];
	static WNIC_AUTH_KEY default_auth_key = {default_key, 0, 0};
	static uint32_t security_type = AUTH_SECURITY_OPEN;
	uint32_t security_type_temp;
	DEV_WNIC *pmwifi_wnic;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	pmwifi_wnic = wnic_get_dev(BOARD_PMWIFI_0_ID);

	opterr = 0;
	optind = 1;

	if (argc < 2) {
		cmd_wifi_help(argv[0], extobj);
		return E_OK;
	}

	while ((opt=getopt(argc, argv, "w:p:t:cdhH?is:")) != -1) {
		opt_enter_cnt ++;

		switch (opt) {
			case 'i':
				init_request = 1;
				break;

			case 's':
				if (strcmp("start", optarg) == 0) {
					scan_request = CMD_SCAN_START;
				} else if (strcmp("stop", optarg) == 0) {
					scan_request = CMD_SCAN_STOP;
				} else if (strcmp("result", optarg) == 0) {
					scan_request = CMD_GET_SCAN_RST;
				} else {
					scan_request = 0;
				}

				break;

			case 'c':
				connect_request = 1;
				break;

			case 'd':
				disconnect_request = 1;
				break;

			case 'w':
				strcpy((char *)default_ssid, optarg);
				ssid_is_set = 1;
				CMD_DEBUG("SSID is updated:%s\r\n", optarg);
				break;

			case 'p':
				strcpy((char *)default_key, optarg);
				key_is_set = 1;
				CMD_DEBUG("Password is updated:%s\r\n", optarg);
				break;

			case 't':
				security_type_temp = security_type;
				security_type = atoi(optarg);
				CMD_DEBUG("Security Type is updated");

				if (security_type == AUTH_SECURITY_OPEN) {
				} else if (security_type == AUTH_SECURITY_WPA_WITH_PASS_PHRASE) {
				} else if (security_type == AUTH_SECURITY_WPA2_WITH_PASS_PHRASE) {
				} else if (security_type == AUTH_SECURITY_WPA_AUTO_WITH_PASS_PHRASE) {
				} else {
					CMD_DEBUG(" Unchanged");
					security_type = security_type_temp;
				}

				CMD_DEBUG("\r\n");
				dump_wifi_security(security_type, extobj);
				break;

			case 'h':
			case '?':
			case 'H':
				cmd_wifi_help(argv[0], extobj);
				return E_OK;
				break;

			default:
				opt_enter_cnt --;
				CMD_DEBUG("unrecognized option:%c\r\n", opt);
				break;
		}

		/* any request break the loop */
		if (scan_request | connect_request | disconnect_request | init_request) {
			break;
		}
	}

	/* no option is selected */
	if (opt_enter_cnt <= 0) {
		cmd_wifi_help(argv[0], extobj);
		return E_OK;
	}

	if (init_request) {
		if (!wifi_init_status) {
			wifi_init_status = 1;
			CMD_DEBUG("Start Init WIFI Device\r\n");
			xTaskCreate(task_wifi_process, "task_wifi_process", 256, (void *)pmwifi_wnic, TASK_WIFIPS_PRIO, &task_wifips_handle);
		} else {
			CMD_DEBUG("Wifi is already initialized\r\n");
			CMD_DEBUG("--WIFI Default settings--\r\n");
			CMD_DEBUG("SSID ");

			if (ssid_is_set) {
				CMD_DEBUG(" : %s\r\n", default_ssid);
			} else {
				CMD_DEBUG(" is not set\r\n");
			}

			CMD_DEBUG("Password ");

			if (key_is_set) {
				CMD_DEBUG(" : %s\r\n", default_key);
			} else {
				CMD_DEBUG(" is not set\r\n");
			}

			dump_wifi_security(security_type, extobj);
		}
	}

	if (wifi_init_status == 0) {
		CMD_DEBUG("WIFI is not initialized, nothing can be done, run wifi -i to init wifi\r\n");
		return E_OK;
	}

	if (scan_request) {
		switch (scan_request) {
			case CMD_SCAN_START:
				if (!during_scan) {
					during_scan = 1;
					pmwifi_wnic->start_scan();
					CMD_DEBUG("Start scan network\r\n");
				} else {
					CMD_DEBUG("Scan is already started!\r\n");
					CMD_DEBUG("When scan is finished, it will output one message\r\n");
					CMD_DEBUG("If you want to scan again, you can stop it first or read scan result successfully\r\n");
				}

				break;

			case CMD_SCAN_STOP:
				pmwifi_wnic->stop_scan();
				during_scan = 0;
				CMD_DEBUG("Stop scan network\r\n");
				break;

			case CMD_GET_SCAN_RST:
				if ((scan_rst_cnt = pmwifi_wnic->get_scan_result_cnt()) <= 0) {
					CMD_DEBUG("Scan result is not ready\r\n");
				} else {
					during_scan = 0;

					for (i = 0; i < scan_rst_cnt; i ++) {
						if (pmwifi_wnic->get_scan_result(i, &scan_result_data) == E_OK) {
							CMD_DEBUG("----WIFI HOTSPOT FOUND %d----\r\n", i);
							scan_result_data.ssid[scan_result_data.ssidlen] = '\0';
							CMD_DEBUG("SSID:%s\r\n", scan_result_data.ssid);
							CMD_DEBUG("AP Config: %08b\r\n", scan_result_data.ap_config.apConfig);
							CMD_DEBUG("Security Type:");

							if (scan_result_data.ap_config.ap_cfg.privacy == 0) {
								CMD_DEBUG("Open\r\n");
							} else {
								if (scan_result_data.ap_config.ap_cfg.wpa2) {
									CMD_DEBUG("WPA2 Encrypted\r\n");
								} else if (scan_result_data.ap_config.ap_cfg.wpa) {
									CMD_DEBUG("WPA Encrypted\r\n");
								} else {
									CMD_DEBUG("WEP Encrypted\r\n");
								}
							}

							CMD_DEBUG("RSSI:%d\r\n", scan_result_data.rssi);
							CMD_DEBUG("BSS Type:%d\r\n", scan_result_data.bsstype);
							CMD_DEBUG("Channel Number:%d\r\n", scan_result_data.channel);
						}
					}
				}

				break;
		}
	}

	if (connect_request) {
		if (ssid_is_set) {
			if (security_type != AUTH_SECURITY_OPEN && !key_is_set) {
				CMD_DEBUG("Please set password first, when you want to connect to a secured network\r\n");
			}

			if (pmwifi_wnic->wnic_connect(security_type, default_ssid, &default_auth_key) == E_OK) {
				CMD_DEBUG("Start Connect to SSID : %s, ", default_ssid);

				if (security_type == AUTH_SECURITY_OPEN) {
					CMD_DEBUG("Open Network\r\n");
				} else {
					CMD_DEBUG("Secured Network\r\n");
				}
			} else {
				CMD_DEBUG("Can't Start Connect to wifi hotspot\r\n");
			}
		} else {
			CMD_DEBUG("SSID is not set\r\n");
		}
	}

	if (disconnect_request) {
		pmwifi_wnic->wnic_disconnect();
		CMD_DEBUG("Disconnected\r\n");
	}

	return 0;

error_exit:
	return ercd;
}


static CMD_TABLE_T wifi_cmd = {"wifi", "wifi operations", cmd_wifi, NULL};
/**
 * register wifi command
 */
CMD_TABLE_T *register_ntshell_cmd_wifi(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&wifi_cmd, prev);
}

#endif /* NTSHELL_USE_CMDS_EXTEND_WIFI */
