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
 * \version 2017.03
 * \date 2016-04-20
 * \author Qiang Gu(Qiang.Gu@synopsys.com)
--------------------------------------------- */

/**
 * \defgroup	EMBARC_APP_BAREMETAL_BOOTLOADER		embARC Bootloader Example
 * \ingroup	EMBARC_APPS_TOTAL
 * \ingroup	EMBARC_APPS_BAREMETAL
 * \ingroup	EMBARC_APPS_MID_NTSHELL
 * \ingroup	EMBARC_APPS_MID_FATFS
 * \ingroup	EMBARC_APPS_MID_PARSON
 * \ingroup	EMBARC_APPS_MID_IHEX
 * \brief	embARC example for secondary bootloader
 *
 * \details
 * ### Extra Required Tools
 *
 * ### Extra Required Peripherals
 *     - SDCard with boot binary(boot.hex or boot.bin)
 *
 * ### Design Concept
 *     This example is designed to work as a secondary bootloader for embARC, it will load boot.hex or boot.bin on SDCard and run that program.
 *
 * ### Usage Manual
 *     As shown in the following picture, when the EMSK configuration in SPI flash is loaded into the FPGA,
 *     a simple primary bootloader is also loaded in ICCM. Through the primary bootloader, the application or secondary bootloader can be
 *     loaded into external memory (DDR memory), bootloader start address is 0x17F00004, ram address is 0x17F00000.
 *
 *     For EMSK1.x, bootloader core configuration must be arcem6, for EMSK2.x, bootloader core configuration must be arcem7d.
 *     For EMSK 1.0, it can be upgraded to 1.1 by 1.1's firmware. For EMSK 2.0/2.1/2.2, it can be upgraded to 2.3 by 2.3's firmware.
 *
 *     \htmlonly
 *     <div class="imagebox">
 *         <div style="width: 500px">
 *             <img src="pic/bootloader.jpg" alt="Secondary Bootloader"/>
 *             <p>Secondary Bootloader</p>
 *         </div>
 *     </div>
 *     \endhtmlonly
 *
 *     Here are steps for how to program the secondary bootloader application into onboard SPIFlash(Take EMSK2.3 - ARC EM7D as example) and automatically load and run *boot.hex* or *boot.bin* in SDCard.
 *     - Generate a secondary bootloader binary file
 *        + cd <embARC>/example/baremetal/bootloader
 *        + Generate binary file: make TOOLCHAIN=gnu BD_VER=23 CUR_CORE=arcem7d bin
 *          - If the binary file is generated successfully, you will output as follows:
 *          \code
 *          "Compiling             : " ../../../middleware/ntshell/cmds/cmds_fs/ymodem/ymodemio.c
 *          "Compiling             : " ../../../middleware/ntshell/port/ntshell_usrcmd.c
 *          "Compiling             : " ../../../middleware/ntshell/port/ntshell_task.c
 *          "Archiving             : " obj_emsk_23/gnu_arcem7d/libmidntshell.a
 *          "Compiling             : " ../../../middleware/parson/parson.c
 *          "Archiving             : " obj_emsk_23/gnu_arcem7d/libmidparson.a
 *          "Compiling             : " ../../../arc/arc_timer.c
 *          "Compiling             : " ../../../arc/arc_cache.c
 *          "Compiling             : " ../../../arc/arc_exception.c
 *          "Compiling             : " ../../../arc/arc_udma.c
 *          "Assembling            : " ../../../arc/arc_exc_asm.s
 *          "Archiving             : " obj_emsk_23/gnu_arcem7d/libcpuarc.a
 *          "Compiling             : " ../../../library/clib/fatfs_dirent.c
 *          "Compiling             : " ../../../library/clib/malloc.c
 *          "Compiling             : " ../../../library/clib/embARC_sbrk.c
 *          "Compiling             : " ../../../library/clib/embARC_misc.c
 *          "Compiling             : " ../../../library/clib/embARC_syscalls.c
 *          "Compiling             : " ../../../library/clib/ya_getopt.c
 *          "Compiling             : " ../../../library/clib/embARC_target.c
 *          "Archiving             : " obj_emsk_23/gnu_arcem7d/liblibclib.a
 *          "Archiving             : " obj_emsk_23/gnu_arcem7d/libembarc.a
 *          "Linking               : " obj_emsk_23/gnu_arcem7d/emsk_bootloader_gnu_arcem7d.elf
 *          "Generating Binary obj_emsk_23/gnu_arcem7d/emsk_bootloader_gnu_arcem7d.bin"
 *          \endcode
 *     - Program generated secondary bootloader binary file into SPIFlash
 *        + Insert SDCard to your PC, and copy the binary file *obj_emsk_23/gnu_arcem7d/emsk_bootloader_gnu_arcem7d.bin* to SDCard Root, and rename it to *em7d_2bt.bin*
 *        + Insert the SDCard to EMSK Board, and build and run the <em><embARC>/example/baremetal/bootloader</em> example, please choose the right core configuration
 *        + Then use ntshell command *spirw* to program the *em7d_2bt.bin* into spiflash.
 *            - Firstly, run *spirw* to show help
 *            - Secondly, run *spirw -i* to check SPIFlash ID, it should be **Device ID = ef4018**
 *            - Thirdly, run *spirw -w em7d_2bt.bin 0x17f00000 0x17f00004* to program spiflash
 *            - Check the output message to see if if was programmed successfully.
 *            - ![ScreenShot of program secondary bootloader to spiflash](pic/images/example/emsk/emsk_bootloader_program2splflash.jpg)
 *        * If programmed successfully, when the board is reset, make sure Bit 4 of the onboard DIP switch is ON to enable secondary bootloader run.
 *        ![ScreenShot of secondary bootloader autoboot when board configuration is reloaded](pic/images/example/emsk/emsk_bootloader_onspiflash.jpg)
 *     - Generate *boot.bin* using any embARC example which ram start address should be 0x10000000 and use bootloader to run it
 *        * Here take <em><embARC>/example/freertos/kernel</em> for example
 *        * cd <embARC>/example/freertos/kernel
 *        * Build and generate binary file: *make TOOLCHAIN=gnu BD_VER=23 CUR_CORE=arcem7d bin*
 *        * Insert SDCard to PC, and copy generated binary file *obj_emsk_23/gnu_arcem7d/freertos_kernel_gnu_arcem7d.bin* to SDCard Root, and rename it to boot.bin
 *        * Insert SDCard back to EMSK, make sure bit 4 of DIP Switch is ON, and press re-configure button above letter **C**, and wait for autoload.
 *        * ![ScreenShot of secondary bootloader auto load boot.bin](pic/images/example/emsk/emsk_bootloader_loadbootbin.jpg)
 *     - Know Issues
 *        * Bootrom of EMSK1.x is not able to load secondary bootloader on SPIFlash, you need a modified EMSK1.x mcs file to enable this function, please send request in forum about this mcs file.
 *
 *  **The secondary bootloader is a complement of the primary bootloader, and provides the following functions:**
 *     - File operations on SD card
 *     - Operations on the EMSK, GPIO, I2C, SPI flash
 *     - Operations on ARC processors
 *     - Automatic boot from SD card, using following instructions:
 *     	  * burn the bin file of bootloader into EMSK spi flash using spirw command <b>spirw -w bootloader.bin 0x17f00000 0x17f00004 </b> with the help of JTAG
 *     	  * the primary bootloader should be able to load the secondary bootloader
 *     	  * put the file you want to boot in the root directory of SD card, name it boot.bin
 *     	  * plug in SD card
 *     - LED Status of loading application(boot.bin)
 *        * Start to load application: LED on board -> 0x0F
 *        * Load application finished: LED on board -> 0xFF, if application is running normally, LED will quickly change to 0x0
 *        * Load application failed: LED on board -> 0xAA
 *        * Skip loading application, and enter to NTShell runtime: LED on board -> 0x0
 *     - Type *help* command in ntshell to show the list of supported commands.
 *
 *  ![ScreenShot of bootloader under baremetal](pic/images/example/emsk/emsk_bootloader.jpg)
 *
 * ### Extra Comments
 *     Bootrom of EMSK1.x is not able to load secondary bootloader on SPIFlash,
 *     you need a modified EMSK1.x mcs file to enable this function,
 *     please send request in forum about this mcs file.
 */

/**
 * \file
 * \ingroup	EMBARC_APP_BAREMETAL_BOOTLOADER
 * \brief	example of secondary bootloader after the bootrom of EMSK
 * \todo
 */

/**
 * \addtogroup	EMBARC_APP_BAREMETAL_BOOTLOADER
 * @{
 */

#include "embARC.h"
#include "embARC_debug.h"

#include "ntshell_common.h"
#include "ihex_load.h"
#include "parson.h"

#define BOOT_CFG_FILE_NAME	"boot.json"
#define BOOT_FILE_NAME		"0:\\boot.bin"          /*!< default autoload full file name */
#define RAM_STARTADDRESS	0x10000000		/*!< default ram start address of boot.bin */
#define APP_CFG_ADDR		0x17f00000 		/*!< save the app configuration file name */
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
		f_read(&file, boot_json, file.fsize, &cnt);
		f_close(&file);
		// EMBARC_PRINTF("fsize:%d\tcnt:%d\n", file.fsize, cnt);
		JSON_Value *user_data = json_parse_string((const char *)boot_json);
		if(user_data != NULL){
			/*parse the boot.json*/
			const char *str;
			str = json_object_get_string(json_object(user_data), "boot_file");
			if(str != NULL){
				boot_cfg.boot_file = str;
			}
			str = json_object_get_string(json_object(user_data), "app_cfg");
			if(str != NULL){
				boot_cfg.app_cfg = str;
			}
			uint32_t num;
			num = json_object_get_number(json_object(user_data), "ram_startaddress");
			if(num != 0){
				boot_cfg.ram_startaddress = num;
			}
			int32_t boo;
			boo = json_object_get_boolean(json_object(user_data), "ntshell");
			if(boo != -1){
				boot_cfg.ntshell = boo;
			}
			JSON_Object *obj;
			obj = json_object_get_object(json_object(user_data), "wifi");
			if(obj != NULL){
				/*TODO: parse wifi setting*/
			}
		} else {
			EMBARC_PRINTF("Cannot parse boot.json, please check it. Now use default bootloader\n");
			default_bt_flag = 1;
		}
		EMBARC_PRINTF("boot_file:%s\napp_cfg:%s\nram:0x%x\nntshell:%d", \
			boot_cfg.boot_file, boot_cfg.app_cfg, boot_cfg.ram_startaddress, boot_cfg.ntshell);
	}
	if(default_bt_flag == 1){
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
	if(default_bt_flag == 0 && boot_cfg.ntshell == 1 || load_flag == 0){
		EMBARC_PRINTF("\r\nStart NTShell Runtime...\r\n");
		led_write(0x00, 0xFF); /* Load application finished */
		nt_io = get_ntshell_io(BOARD_ONBOARD_NTSHELL_ID);
		/** enter ntshell command routine no return */
		ntshell_task((void *)nt_io);
	}
	EMBARC_PRINTF("\r\nStart loading %s from sdcard to 0x%x and run...\r\n", boot_cfg.boot_file, boot_cfg.ram_startaddress);
	led_write(0xF, 0xFF); /* Start to load application */
	if(boot_cfg.boot_file[strlen(boot_cfg.boot_file) - 3] == 'h'){
		ihex_start();
		res = ihex_load(IHEX_SD_CARD, (void *)boot_cfg.boot_file);
		if (res != IHEX_END){
			EMBARC_PRINTF("%s open or read error\r\n", boot_cfg.boot_file);
			load_flag = 0;
		}
	}else if(boot_cfg.boot_file[strlen(boot_cfg.boot_file) - 3] == 'b'){
		res = f_open(&file, boot_cfg.boot_file, FA_READ | FA_OPEN_EXISTING);
		if (res) {
			EMBARC_PRINTF("%s open error\r\n", boot_cfg.boot_file);
			load_flag = 0;
		}else{
			ram = (void *)boot_cfg.ram_startaddress;
			res = f_read(&file, ram, file.fsize, &cnt);
			f_close(&file);
			if (res || ((uint32_t)cnt != file.fsize))
			{
				EMBARC_PRINTF("%s read error\r\n", boot_cfg.boot_file);
				load_flag = 0;
			}
		}
	}else{
		EMBARC_PRINTF("%s unknown file type\r\n", boot_cfg.boot_file);
		load_flag = 0;
	}
	if(load_flag == 0){
		EMBARC_PRINTF("\r\nStart NTShell Runtime...\r\n");
		led_write(0x00, 0xFF); /* Load application finished */
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

	fp = (fp_t)(boot_cfg.ram_startaddress + 4);
	fp();	/* jump to program */
	return E_SYS;
}

/** @} */
