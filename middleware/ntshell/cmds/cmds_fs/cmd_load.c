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
 * \brief load .bin file from SD card
 */

#include "cmds_fs_cfg.h"

#if NTSHELL_USE_CMDS_FS_LOAD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "embARC.h"

#include "ntlibc.h"
#include "ntshell_common.h"

#include "embARC_debug.h"

#ifdef MID_IHEX
#include "ihex_load.h"
#endif

#ifndef USE_NTSHELL_EXTOBJ /* don't use ntshell extobj */
#define CMD_DEBUG(fmt, ...)			EMBARC_PRINTF(fmt, ##__VA_ARGS__)
#endif

static NTSHELL_IO_PREDEF;

static FIL load_file;

extern unsigned int _f_text;
extern unsigned int _e_heap;

#define DEFAULT_LOAD_ADDR 0x10000000

static uint32_t default_go_addr = 0;
static uint32_t prog_start = (uint32_t)(&_f_text);
static uint32_t prog_end = (uint32_t)(&_e_heap);

static int32_t load_application(char *file_name, uint32_t load_addr)
{
	int32_t ercd = E_OK;
	uint8_t res;
	uint32_t binsize = 0, load_size = 0;
	FIL *p_load_file = &load_file;

	if (load_addr & 0x3) {
		CMD_DEBUG("Please check the load address is 4 bytes align!\r\n");
		return E_PAR;
	}

	res = f_open(p_load_file, file_name, FA_READ | FA_OPEN_EXISTING);
	if (res != RES_OK) {
		CMD_DEBUG("Can't open %s, please check if fatfs is ready or file exists!\r\n", file_name);
		return E_OBJ;
	}
	binsize = f_size(p_load_file);

	CMD_DEBUG("Try to download %s at 0x%x\r\n", file_name, load_addr);

	if (((load_addr+binsize) >= prog_start) && (load_addr <= prog_end)) {
		CMD_DEBUG("Can't download program to the current program memory region(0x%x-0x%x).\r\n", prog_start, prog_end);
	} else {
		/* load binary to ram and check if success */
		res = f_read(p_load_file, (void *)load_addr, binsize, &load_size);
		if ((res != RES_OK) || (load_size != binsize))	{
			CMD_DEBUG("Binary file read error, binary file load failed\r\n");
			ercd = E_OBJ;
		} else {
			CMD_DEBUG("Binary file load successfully.\r\n");
			ercd = E_OK;
		}
	}
	f_close(p_load_file);

	return ercd;
}

typedef int (*fp_t)(void);

static int32_t go_application(uint32_t go_addr)
{
	fp_t fp;

	if (go_addr & 0x3) {
		CMD_DEBUG("Please check the load address is 4 bytes align!\r\n");
		return E_PAR;
	}

	CMD_DEBUG("Start Program at 0x%x...\r\n", go_addr);
	fp = (fp_t)(go_addr);

	cpu_lock();
	for (int i = NUM_EXC_CPU; i < NUM_EXC_ALL; i++) {
		int_disable(i);
	}
	if ((_arc_aux_read(AUX_BCR_I_CACHE) & 0x7) >= 0x2) {
		//icache_invalidate();
	}
	if ((_arc_aux_read(AUX_BCR_D_CACHE) & 0x7) >= 0x2) {
		//dcache_flush();
		dcache_invalidate();
	}
	fp();	/* jump to program */
	return E_OK;
}

#define MAX_LINE_STRCNT			(256)

#define SKIP_STR_WHITESPACES(str)	while((*(str) == ' ') || (*(str) == '\t') || (*(str) == '\0')) {(str)++; };

static int32_t next_substr(char **linestr, char *substr)
{
	if ((linestr == NULL) || (*linestr == NULL) || (substr == NULL)) {
		return -1;
	}
	if (**linestr == '\0') return -1;
	int32_t len = 0;

	SKIP_STR_WHITESPACES(*linestr);
	while((*(*linestr) != ' ') && (*(*linestr) != '\t') && (*(*linestr) != '\0')) {
		substr[len] = **linestr;
		(*linestr) ++;
		len ++;
	}
	substr[len] = '\0';
	return len;
}

static int32_t parse_application(char *linestr, const char *app_name, char *app_path, uint32_t *load_addr, uint32_t *go_addr)
{
	if (!(linestr && app_name && app_path && load_addr && go_addr)) {
		return -1;
	}
	char *substr = NULL;
	char *pstr;
	int32_t len = 0;
	int32_t ercd = -1;
	long res;

	substr = (char *)malloc(MAX_LINE_STRCNT);
	if (substr == NULL) {
		return -2;
	}

	/* Compare application name */
	if ((len = next_substr(&linestr, substr)) <= 0) {
		goto error_exit;
	}
	if (strcmp(app_name, substr) != 0) {
		goto error_exit;
	}
	/* Get application path */
	len = next_substr(&linestr, app_path);
	if (len <= 0) {
		goto error_exit;
	}
	/* Get application load address */
	if ((len = next_substr(&linestr, substr)) <= 0) {
		goto error_exit;
	}
	pstr = substr;
	if (xatoi((char **)(&pstr), (long *)(&res)) != 1) {
		goto error_exit;
	} else {
		*load_addr = res;
	}
	/* Get application go address */
	if ((len = next_substr(&linestr, substr)) <= 0) {
		goto error_exit;
	}
	pstr = substr;
	if (xatoi((char **)(&pstr), (long *)(&res)) != 1) {
		goto error_exit;
	} else {
		*go_addr = res;
	}
	ercd = 0;

error_exit:
	free(substr);
	return ercd;
}

static int32_t find_application(const char *boot_cfg, const char *app_name, char *app_path, uint32_t *load_addr, uint32_t *go_addr)
{
	/** Check parameters */
	if (!(boot_cfg && app_name && app_path && load_addr && go_addr)) {
		return -1;
	}

	int32_t found = -3;
	int8_t res;
	char *linestr = NULL;
	FIL *p_bootcfg = &load_file;

	res = f_open(p_bootcfg, boot_cfg, FA_READ | FA_OPEN_EXISTING);
	if (res != RES_OK) {
		return -1;
	}
	linestr = (char *)malloc(MAX_LINE_STRCNT);
	if (linestr == NULL) {
		f_close(p_bootcfg);
		return -2;
	}
	do {
		if (f_gets(linestr, MAX_LINE_STRCNT, p_bootcfg) == NULL) {
			/** end of file */
			goto error_exit;
		}
		if (parse_application(linestr, app_name, app_path, load_addr, go_addr) == 0) {
			found = 0;
			break;
		}

	} while(1);

error_exit:
	free(linestr);
	f_close(p_bootcfg);

	return found;
}

static void cmd_load_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}
	CMD_DEBUG("Usage: %s [OPTION]... <FILE_NAME> <ADD>\r\n"
		"Load .bin(default) or .hex file from SD card to ram at specified address\r\n"
		"  -h/H/?    Show the help information\r\n"
		"  -i    Load the Intel hex (.hex) file\r\n"
		"  <FILE_NAME> <ADD>    Load <FILE_NAME> in SD card to ram from the address: <ADD>\r\n"
		"Examples: \r\n"
		"  load boot.bin 0x10000000    load boot.bin in the root directory of SD card to the address 0x1000000 in the ram\r\n"
		"  load -i boot.hex    load boot.hex in the root directory of SD card to specified address\r\n"
		"  load -h    Show the help information\r\n", cmd_name);
error_exit:
	return;
}

static int cmd_load(int argc, char **argv, void *extobj)
{
	int32_t ercd = E_OK;
	char *file_name;
	uint32_t load_addr = 0;
	int32_t opt;
	int32_t use_ihex = 0;
	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	if(argc < 2 || argc > 3) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	}

	opterr = 0;
	optind = 1;
	while ((opt=getopt(argc, argv, "i:hH?")) != -1) {
		switch (opt) {
			case 'h':
			case '?':
			case 'H':
				cmd_load_help(argv[0], extobj);
				goto error_exit;
				break;
			case 'i':
				file_name = optarg;
				use_ihex = 1;
				break;
			default:
				CMD_DEBUG("%s: unrecognized option:%c\r\n", argv[0], opt);
				CMD_DEBUG("Try '%s -h' for more information\r\n",argv[0]);
				break;
		}
	}

	if(use_ihex == 0){
		file_name = argv[1];
		if (argc == 3) {
			if (xatoi((char **)(&argv[2]), (long *)(&load_addr)) != 1) {
				CMD_DEBUG("Please check the load address is valid!\r\n");
				return E_PAR;
			}
		} else {
			load_addr = DEFAULT_LOAD_ADDR;
		}

		ercd = load_application(file_name, load_addr);
		if (ercd == E_OK) {
			default_go_addr = load_addr + 4;
		}
	} else {
#ifdef MID_IHEX
		ihex_start();
		if(ihex_load(IHEX_SD_CARD, file_name) == IHEX_END){
			default_go_addr = DEFAULT_LOAD_ADDR + 4;
			CMD_DEBUG("Load %s successfully!\r\n", file_name);
		} else {
			CMD_DEBUG("Can't find %s or %s is invalid!\r\n", file_name, file_name);
		}
#else
		CMD_DEBUG("The IHEX middleware is not enabled!\r\n");
#endif
	}
error_exit:
	return ercd;
}

static void cmd_go_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}
	CMD_DEBUG("Usage: %s [OPTION]... <ADD>\r\n"
		"CPU is forced to jump to the address <ADD> in the ram, usually used with \"load\"\r\n"
		"  -h/H/?    Show the help information\r\n"
		"  <ADD>     Jump to the address <ADD>, and run\r\n"
		"Examples: \r\n"
		"  go 0x10000000    Jump to 0x10000000, and run\r\n"
		"  go -h    Show the help information\r\n", cmd_name);
error_exit:
	return;
}

static int cmd_go(int argc, char **argv, void *extobj)
{
	int32_t ercd = E_OK;
	uint32_t go_addr = 0;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	/* show the help information, if the option is "-h" */
	if(argc == 2 && ntlibc_strcmp(argv[1],"-h") == 0x00) {
		cmd_go_help(argv[0], extobj);
		goto error_exit;
	}
	/* the parameter checking */
	if (argc < 1) {
		ercd = E_PAR;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	}

	if (argc > 1) {
		if (xatoi((char **)(&argv[1]), (long *)(&go_addr)) != 1) {
			CMD_DEBUG("Please check the go address is valid!\r\n");
			return E_PAR;
		}
	} else {
		go_addr = default_go_addr;
	}

	go_application(go_addr);
	return E_OK;

error_exit:
	return ercd;
}

#define BOOT_CFG_NAME		"boot.cfg"

/*
 * boot.cfg FORMAT(new entry added with newline)
 * boot_app location_in_sdcard load_addr_in_ram go_addr_in_ram
 * For example:
 * aws boot/aws.bin 0x10000000 0x10000004
 */
static void cmd_boot_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}
	CMD_DEBUG("Usage: %s [OPTION]... -c <CFG> <APP>\r\n"
		"Load selected application from sdcard and run, application configurations usually defined in boot.txt file\r\n"
		"  -h/H/?    Show the help information\r\n"
		"  -c <CFG>  Choose boot configuration file\r\n"
		"  -a <APP>  Select boot application and load it and run\r\n"
		"Examples: \r\n"
		"  boot -a aws   Boot aws application defined in %s\r\n"
		"  boot -h    Show the help information\r\n", cmd_name, BOOT_CFG_NAME);
error_exit:
	return;
}

static int cmd_boot(int argc, char **argv, void *extobj)
{
	int32_t ercd = E_OBJ;
	char app_path[128];
	char *app_name = NULL;
	char *boot_cfg = BOOT_CFG_NAME;
	uint32_t load_addr, go_addr;
	int32_t opt;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	if(argc < 2) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	}

	opterr = 0;
	optind = 1;

	while ((opt=getopt(argc, argv, "c:a:hH?")) != -1) {
		switch (opt) {
			case 'h':
			case '?':
			case 'H':
				cmd_boot_help(argv[0], extobj);
				goto error_exit;
				break;
			case 'c':
				boot_cfg = optarg;
				break;
			case 'a':
				app_name = optarg;
				break;
			default:
				CMD_DEBUG("%s: unrecognized option:%c\r\n", argv[0], opt);
				CMD_DEBUG("Try '%s -h' for more information\r\n",argv[0]);
				break;
		}
	}
	if (app_name == NULL) {
		CMD_DEBUG("No application selected, please check your command!\r\n");
		goto error_exit;
	}
	if (find_application(boot_cfg, app_name, app_path, &load_addr, &go_addr) != 0) {
		CMD_DEBUG("Application %s not found in %s\r\n", app_name, boot_cfg);
		goto error_exit;
	} else {
		CMD_DEBUG("Try to load application %s from %s\r\n", app_name, app_path);
		if (load_application(app_path, load_addr) == E_OK) {
			CMD_DEBUG("Load application %s from %s successfully!\r\n", app_name, app_path);
			if (go_application(go_addr) != E_OK) {
				CMD_DEBUG("Application run failed!\r\n");
			}
		}
	}

	return E_OK;

error_exit:
	return ercd;
}

static CMD_TABLE_T load_cmd = {"load", "Load .bin file from SD card to ram at specified address", cmd_load, NULL};
static CMD_TABLE_T go_cmd = {"go", "Run the program at the specified address", cmd_go, NULL};
static CMD_TABLE_T boot_cmd = {"boot", "Load application specified in a boot.cfg file from sdcard and run", cmd_boot, NULL};

/**
 * register load & go command
 */
CMD_TABLE_T * register_ntshell_cmd_load(CMD_TABLE_T *prev)
{
	prev = ntshell_usrcmd_register(&load_cmd, prev);
	prev = ntshell_usrcmd_register(&boot_cmd, prev);
	return ntshell_usrcmd_register(&go_cmd, prev);
}

#endif /* NTSHELL_USE_CMDS_FS_LOAD */
