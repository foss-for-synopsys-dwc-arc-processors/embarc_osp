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
* \brief LED flash command
*/


#include "cmds_peripheral_cfg.h"
#if NTSHELL_USE_CMDS_PERIPHERAL_LEDFLASH

#include "ntshell_common.h"

static NTSHELL_IO_PREDEF;

#define FLASHING_CYCLES		(3)
#define MAX_DELAY	1000  /* max period ms */
#define MAX_TIMES	100   /* max repeated times */

static uint32_t leds = 0x55;

//***************************************************************/
//	LOCAL FUNCTIONS PROTOTYPES
//***************************************************************/

/* show help of command ledflash */
static void cmd_ledflash_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]... <PERIOD(ms)> [TIMES]\r\n"
	          "Run \r\n"
	          "  -h/H/?    Show the help information\r\n"
	          "  <NUM1> [NUM]    Set the flashing period: <NUM1> ms, run [NUM2] times\r\n"
	          "Examples: \r\n"
	          "  ledflash  500 0x08    Set the flashing period: 500ms, run 8 times\r\n"
	          "  ledflash -h    Show the help information\r\n", cmd_name);
error_exit:
	return;
}

static int cmd_ledflash(int argc, char **argv, void *extobj)
{
	uint32_t i, cycle;
	uint32_t period_ms, measure_times;
	long val;
	uint32_t arg_err_flag = 0;
	char **argv_tp;
	int32_t ercd = E_OK;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	/* show the help information, if the option is "-h" */
	if (argc == 2 && ntlibc_strcmp(argv[1],"-h") == 0x00) {
		cmd_ledflash_help(argv[0], extobj);
		goto error_exit;
	}

	/* the parameter checking */
	if ((argc > 3) || (argc < 2)) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	}

	argv_tp = (char **)(&argv[1]);

	/* parse 1th argument: measure period */
	if (xatoi(&argv_tp[0], &val)) {
		period_ms = (uint32_t) val;
	} else {
		arg_err_flag = arg_err_flag | FIRST_ARG_ERR;
	}

	/* parse 2nd argument: measure times */
	if (argc == 3) {
		if (xatoi(&argv_tp[1], &val)) {
			measure_times = (uint32_t) val;
		} else {
			arg_err_flag = arg_err_flag | SECOND_ARG_ERR;
		}
	} else {
		measure_times = 1; /* default 1 times */
	}

	if (arg_err_flag != 0) { /* input argument format error */
		parse_arg_err(arg_err_flag, extobj);
		ercd = E_SYS;
		goto error_exit;
	}

	if (measure_times == 0) {
		CMD_DEBUG("ERROR: repeat times is 0. exit LED Sample\r\n");
		ercd = E_SYS;
		goto error_exit;
	}

	if (period_ms > MAX_DELAY) {
		CMD_DEBUG("WARN: Max Period is %d!\r\n", MAX_DELAY);
		period_ms = MAX_DELAY;
	}

	if (measure_times > MAX_TIMES) {
		CMD_DEBUG("WARN: Max Times is %d!\r\n", MAX_TIMES);
		measure_times = MAX_TIMES;
	}

	CMD_DEBUG("\r\n********************************\r\n");
	CMD_DEBUG(    "*    Flashing leds demo        *\r\n");
	CMD_DEBUG(    "*    Period:%4dms  Times:%4d *\r\n", period_ms, measure_times);
	CMD_DEBUG(    "********************************\r\n");

	for (cycle = 0; cycle < measure_times; cycle++) {
		leds = 0x155;

		for (i= 0; i < 9; i++) {
			board_delay_ms(period_ms, OSP_DELAY_OS_COMPAT_ENABLE);
			leds ^= BOARD_LED_MASK;
			led_write(leds, BOARD_LED_MASK);
		}

		leds = 0x01;

		for (i= 0; i < 9; i++) {
			board_delay_ms(period_ms, OSP_DELAY_OS_COMPAT_ENABLE);
			leds = leds << 1;
			led_write(leds, BOARD_LED_MASK);
		}

		leds = 0x100;

		for (i= 0; i < 9; i++) {
			board_delay_ms(period_ms, OSP_DELAY_OS_COMPAT_ENABLE);
			leds = BOARD_LED_MASK & ((leds >> 1));
			led_write(leds, BOARD_LED_MASK);
		}

		CMD_DEBUG("Led Flash Cycle %d\r\n", cycle);
		// switch off all leds
		led_write(0, BOARD_LED_MASK);
	}

	CMD_DEBUG("*** Exit LED Example ***\r\n");
	return E_OK;

error_exit:
	return ercd;
}

static CMD_TABLE_T ledflash_cmd = {"ledflash", "Run led flash example", cmd_ledflash, NULL};
/**
* register led flash command
*/
CMD_TABLE_T *register_ntshell_cmd_ledflash(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&ledflash_cmd, prev);
}

#endif /* NTSHELL_USE_CMDS_PERIPHERAL_LEDFLASH */
