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
* \brief operate the LEDs through DIP switches and buttons
*/

#include "cmds_peripheral_cfg.h"
#if NTSHELL_USE_CMDS_PERIPHERAL_LEDSWBTN

#include "ntshell_common.h"

static NTSHELL_IO_PREDEF;

/* show help of command led */
static void cmd_led_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]... <VAL>\r\n"
	          "Open or close the 9 leds in the EMSK\r\n"
	          "  -h/H/?    Show the help information\r\n"
	          "  <VAL>     The low nine bits of <VAL> is effective, each bit can control one led, 1--open, 0--close\r\n"
	          "Examples: \r\n"
	          "  led 0x10F Open the 0~3th and 8th leds, close 4~7th leds\r\n"
	          "  led -h    Show the help information\r\n", cmd_name);
error_exit:
	return;
}

static int cmd_led(int argc, char **argv, void *extobj)
{
	uint32_t led_val;
	long val;
	uint32_t arg_err_flag = 0;
	char **argv_tp;
	int32_t ercd = E_OK;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	/* show the help information, if the option is "-h" */
	if (argc == 2 && ntlibc_strcmp(argv[1],"-h") == 0x00) {
		cmd_led_help(argv[0], extobj);
		goto error_exit;
	}

	/* the parameter checking */
	if (argc != 2) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	}

	argv_tp = (char **)(&argv[1]);

	/* parse 1th argument: led value */
	if (xatoi(&argv_tp[0], &val)) {
		led_val = (uint32_t) val;
	} else {
		arg_err_flag = arg_err_flag | FIRST_ARG_ERR;
	}

	if (arg_err_flag != 0) { /* input argument format error */
		parse_arg_err(arg_err_flag, extobj);
		ercd = E_SYS;
		goto error_exit;
	}

	CMD_DEBUG("Write LED With Value 0x%x\r\n", led_val & BOARD_LED_MASK);
	led_write(led_val, BOARD_LED_MASK);
	return E_OK;

error_exit:
	return ercd;
}

/* show help of command btn */
static void cmd_btn_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]\r\n"
	          "Read the button status, bit 0: the status of button 'L', bit 1: the button 'R'\r\n"
	          "  -h/H/?    Show the help information\r\n"
	          "Examples: \r\n"
	          "  btn       Show the button status, 1--press the button, \r\n"
	          "  btn -h    Show the help information\r\n", cmd_name);
error_exit:
	return;
}

static int cmd_btn(int argc, char **argv, void *extobj)
{
	uint32_t btn_val;
	int32_t ercd = E_OK;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	/* show the help information, if the option is "-h" */
	if (argc == 2 && ntlibc_strcmp(argv[1],"-h") == 0x00) {
		cmd_btn_help(argv[0], extobj);
		goto error_exit;
	}

	/* the parameter checking */
	if ((argc > 1) || (argc < 0)) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	}

	btn_val = button_read(BOARD_BTN_MASK);
	CMD_DEBUG("Button Read : 0x%x\r\n", btn_val);
	return E_OK;
error_exit:
	return ercd;
}

/* show help of command swt */
static void cmd_swt_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]\r\n"
	          "Read the DIP switch status\r\n"
	          "  -h/H/?    Show the help information\r\n"
	          "Examples: \r\n"
	          "  swt       Show the SIP switch status, 1--ON, 0--OFF\r\n"
	          "  swt -h    Show the help information\r\n", cmd_name);

error_exit:
	return;
}

static int cmd_swt(int argc, char **argv, void *extobj)
{
	uint32_t swt_val;
	int32_t ercd = E_OK;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	/* show the help information, if the option is "-h" */
	if (argc == 2 && ntlibc_strcmp(argv[1],"-h") == 0x00) {
		cmd_swt_help(argv[0], extobj);
		goto error_exit;
	}

	/* the parameter checking */
	if ((argc > 1) || (argc < 0)) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	}

	swt_val = switch_read(BOARD_SWT_MASK);
	CMD_DEBUG("Swicth Read : 0x%x\r\n", swt_val);
	return E_OK;
error_exit:
	return ercd;
}

/* show help of command ledswbtn */
static void cmd_ledswbtn_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]\r\n"
	          "Operate the LED through button and switch\r\n"
	          "  -h/H/?    Show the help information\r\n"
	          "Examples: \r\n"
	          "  ledswbtn       Operate the LED through button and switch\r\n"
	          "  ledswbtn -h    Show the help information\r\n", cmd_name);
error_exit:
	return;
}

static int cmd_ledswbtn(int argc, char **argv, void *extobj)
{
	uint32_t readval;
	int32_t ercd = E_OK;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	/* show the help information, if the option is "-h" */
	if (argc == 2 && ntlibc_strcmp(argv[1],"-h") == 0x00) {
		cmd_ledswbtn_help(argv[0], extobj);
		goto error_exit;
	}

	/* the parameter checking */
	if ((argc > 1) || (argc < 0)) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	}

	CMD_DEBUG("Enter LED Button Swicth Test\r\n");
	CMD_DEBUG("Button was mapped to LED 0 - %d \r\n", BOARD_BTN_CNT-1);
	CMD_DEBUG("Switch was mapped to LED %d - %d \r\n", BOARD_BTN_CNT, BOARD_BTN_CNT+BOARD_SWT_CNT-1);
	CMD_DEBUG("Press Botton A above letter 'A' of the ARC Logo to exit\r\n");

	do {
		readval = (switch_read(BOARD_SWT_MASK) << BOARD_BTN_CNT) \
		          | (button_read(BOARD_BTN_MASK));
		led_write(readval, BOARD_LED_MASK);
		board_delay_ms(10, OSP_DELAY_OS_COMPAT_ENABLE);
	} while (!(button_read(BOARD_BTN_MASK) & 0x4));

	CMD_DEBUG("Exit LED Button Swicth Test\r\n");
	led_write(0, BOARD_LED_MASK);

	return E_OK;
error_exit:
	return ercd;
}

static CMD_TABLE_T led_cmd      = {"led", "Write LED", cmd_led, NULL};
static CMD_TABLE_T btn_cmd      = {"btn", "Read button value", cmd_btn, NULL};
static CMD_TABLE_T swt_cmd      = {"swt", "Read DIP switch value", cmd_swt, NULL};
static CMD_TABLE_T ledswbtn_cmd = {"ledswbtn", "Operate the LED through button and switch", cmd_ledswbtn, NULL};

/**
* register led sw btn command
*/
CMD_TABLE_T *register_ntshell_cmd_ledswbtn(CMD_TABLE_T *prev)
{
	prev = ntshell_usrcmd_register(&led_cmd     , prev);
	prev = ntshell_usrcmd_register(&btn_cmd     , prev);
	prev = ntshell_usrcmd_register(&swt_cmd     , prev);
	return ntshell_usrcmd_register(&ledswbtn_cmd, prev);
}

#endif /* NTSHELL_USE_CMDS_PERIPHERAL_LEDSWBTN */
