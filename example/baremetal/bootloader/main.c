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

#include "ntshell_common.h"
#include "ihex_load.h"
#include "parson.h"

#include "target_mem_config.h"

#define BOOT_CFG_FILE_NAME	"boot.json"
#define BOOT_FILE_NAME		"0:\\boot.bin"          /*!< default autoload full file name */

#if defined(BOARD_EMSK)
#define RAM_STARTADDRESS	0x10000000		/*!< default ram start address of boot.bin */
#define APP_CFG_ADDR		0x17f00000 		/*!< save the app configuration file name */
#elif defined(BOARD_IOTDK)
#define RAM_STARTADDRESS 	EXT_RAM_START
#define APP_CFG_ADDR 		ARC_X_MEM_START
#endif

#define PROMT_DELAY_S		(5)			/*!< default wait time for autoload */

typedef int (*fp_t)(void);

struct wifi_cfg_t {
	char* wifi_sel;
	char* SSID;
	char* PSK;
	uint8_t DHCP;
	uint8_t version;
	uint8_t address[4];
	uint8_t gateway[4];
	uint8_t mask[4];
	uint8_t MAC[5];
};

struct boot_cfg_t {
	uint8_t ntshell;
	const char *boot_file;
	uint32_t ram_startaddress;
	struct wifi_cfg_t *wifi;
	const char *app_cfg;
};

struct wifi_cfg_t wifi_cfg;
struct boot_cfg_t boot_cfg = {
	.ntshell = 0,
	.boot_file = BOOT_FILE_NAME,
	.ram_startaddress = RAM_STARTADDRESS,
};

static NTSHELL_IO *nt_io;

static FIL file;

/**
 * \brief	test bootloader
 */
int main(void)
{
	uint8_t res;
	uint32_t cnt;
	void *ram;
	uint8_t load_flag = 1;
	uint8_t default_bt_flag = 0;
	uint32_t cur_ms = 0, cur_cnt = 0;
	uint32_t max_promt_ms = PROMT_DELAY_S * 1000;
	uint32_t boot_json[1000];
	fp_t fp;

	/* No USE_BOARD_MAIN */
	board_init();
	cpu_unlock();	/* unlock cpu to let interrupt work */
	boot_cfg.wifi = &wifi_cfg;
	//read the json to boot_json;
	res = f_open(&file, BOOT_CFG_FILE_NAME, FA_READ | FA_OPEN_EXISTING);

	if (res) {
		EMBARC_PRINTF("%s open error. use default bootloader\r\n", BOOT_CFG_FILE_NAME);
		default_bt_flag = 1;
	} else {
		res = f_read(&file, boot_json, file.fsize, &cnt);
		if (res) {
			EMBARC_PRINTF("read boot cfg file error\r\n");
			default_bt_flag = 1;
		}
		f_close(&file);
	}

	if (default_bt_flag == 0) {
		JSON_Value *user_data = json_parse_string((const char *)boot_json);
		if (user_data != NULL) {
			/*parse the boot.json*/
			const char *str;
			str = json_object_get_string(json_object(user_data), "boot_file");
			if (str != NULL) {
				boot_cfg.boot_file = str;
			}
			str = json_object_get_string(json_object(user_data), "app_cfg");
			if (str != NULL) {
				boot_cfg.app_cfg = str;
			}

			uint32_t num;
			num = json_object_get_number(json_object(user_data), "ram_startaddress");
			if (num != 0) {
				boot_cfg.ram_startaddress = num;
			}

			int32_t boo;
			boo = json_object_get_boolean(json_object(user_data), "ntshell");
			if (boo != -1) {
				boot_cfg.ntshell = boo;
			}

			JSON_Object *obj;
			obj = json_object_get_object(json_object(user_data), "wifi");
			if (obj != NULL) {
				/*TODO: parse wifi setting*/
			}

			EMBARC_PRINTF("boot_file:%s\napp_cfg:%s\nram:0x%x\nntshell:%d", \
			boot_cfg.boot_file, boot_cfg.app_cfg, boot_cfg.ram_startaddress, boot_cfg.ntshell);
		} else {
			EMBARC_PRINTF("Cannot parse boot.json, please check it. Now use default bootloader\n");
			default_bt_flag = 1;
		}
	}

	if (default_bt_flag == 1) {
		EMBARC_PRINTF("\r\nPress any button on board to stop auto boot in %d s\r\n", PROMT_DELAY_S);
		cur_ms = OSP_GET_CUR_MS();
		do {
			if (((OSP_GET_CUR_MS()-cur_ms)/1000) == cur_cnt) {
				cur_cnt ++;
				EMBARC_PRINTF(". ");
			}
			/* any button pressed */
			if (button_read(BOARD_BTN_MASK)) {
				load_flag = 0;
				break;
			}
		} while((OSP_GET_CUR_MS() - cur_ms) < max_promt_ms);
	}

	if (default_bt_flag == 0 && boot_cfg.ntshell == 1 || load_flag == 0) {
		EMBARC_PRINTF("\r\nStart NTShell Runtime...\r\n");
		led_write(0x00, 0xFF);
		nt_io = get_ntshell_io(BOARD_ONBOARD_NTSHELL_ID);
		/** enter ntshell command routine no return */
		ntshell_task((void *)nt_io);
	}

	EMBARC_PRINTF("\r\nStart loading %s from sdcard to 0x%x and run...\r\n", boot_cfg.boot_file, boot_cfg.ram_startaddress);
	led_write(0xF, 0xFF); /* Start to load application */
	if (boot_cfg.boot_file[strlen(boot_cfg.boot_file) - 3] == 'h') {
		ihex_start();
		res = ihex_load(IHEX_SD_CARD, (void *)boot_cfg.boot_file);
		if (res != IHEX_END) {
			EMBARC_PRINTF("%s open or read error\r\n", boot_cfg.boot_file);
			load_flag = 0;
		}
	} else if (boot_cfg.boot_file[strlen(boot_cfg.boot_file) - 3] == 'b') {
		res = f_open(&file, boot_cfg.boot_file, FA_READ | FA_OPEN_EXISTING);
		if (res) {
			EMBARC_PRINTF("%s open error\r\n", boot_cfg.boot_file);
			load_flag = 0;
		} else {
			ram = (void *)boot_cfg.ram_startaddress;
			res = f_read(&file, ram, file.fsize, &cnt);
			f_close(&file);
			if (res || ((uint32_t)cnt != file.fsize)) {
				EMBARC_PRINTF("%s read error\r\n", boot_cfg.boot_file);
				load_flag = 0;
			}
		}
	} else {
		EMBARC_PRINTF("%s unknown file type\r\n", boot_cfg.boot_file);
		load_flag = 0;
	}

	if (load_flag == 0) {
		EMBARC_PRINTF("\r\nStart NTShell Runtime...\r\n");
		led_write(0x00, 0xFF);
		nt_io = get_ntshell_io(BOARD_ONBOARD_NTSHELL_ID);
		/** enter ntshell command routine no return */
		ntshell_task((void *)nt_io);
	}
	led_write(0xF0, 0xFF); /* Load application finished */

	ram = (void *)APP_CFG_ADDR;
	memcpy(ram, boot_cfg.app_cfg, strlen(boot_cfg.app_cfg) + 1);
	cpu_lock();

	for (int i = NUM_EXC_CPU; i < NUM_EXC_ALL; i++) {
		int_disable(i);
	}
	/* set up an enviroment for application */
	if ((_arc_aux_read(AUX_BCR_I_CACHE) & 0x7) >= 0x2) {
		//icache_invalidate();
	}
	if ((_arc_aux_read(AUX_BCR_D_CACHE) & 0x7) >= 0x2) {
		//dcache_flush();
		dcache_invalidate();
	}
	led_write(0xFF, 0xFF); /* Start application */

	fp = (fp_t)(*((uint32_t *)boot_cfg.ram_startaddress));
	fp();	/* jump to program */
	return E_SYS;
}
