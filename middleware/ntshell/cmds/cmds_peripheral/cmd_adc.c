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
* \brief PMOD ADC sensor demo commands
*/


#include "cmds_peripheral_cfg.h"
#if NTSHELL_USE_CMDS_PERIPHERAL_ADC

#include "ntshell_common.h"

static NTSHELL_IO_PREDEF;

#define I2C_SLAVE_ADDR  0x28

#define CHANNEL_0  0x10
#define CHANNEL_1  0x20
#define CHANNEL_2  0x40
#define CHANNEL_3  0x80

#define MAX_DELAY	10000 /* max period ms */
#define MAX_TIMES	1000  /* max repeated times */

#define TIMEOUT 	0x10000


static const char ad_channels[] = { CHANNEL_0, CHANNEL_1, CHANNEL_2, CHANNEL_3 };
static char write_buf[20];
static char read_buf[20];

static DEV_IIC *adc_iic;

/* show help of command adc */
static void cmd_adc_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (_arc_rarely((cmd_name == NULL))) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]... <PERIOD(ms)> [TIMES]\r\n"
		"Read the result from the PMOD ADC sensor, connect PMOD ADC to J2\r\n"
		"  -h/H/?    Show the help information\r\n"
		"  <NUM1> [NUM2]    Read the result every <NUM1> ms, exit after reading [NUM2] times(the default value is 1)\r\n"
		"Examples: \r\n"
		"  adc  100 2    Show the result every 100ms, exit after reading 2 times\r\n"
		"  adc -h    Show the help information\r\n", cmd_name);
error_exit:
	return;
}

static int cmd_adc(int argc, char **argv, void *extobj)
{
	uint32_t i;
	uint32_t res;
	uint32_t adc[4];
	uint32_t count = 0;
	uint32_t period_ms, measure_times;
	uint32_t param;
	long val;
	uint32_t arg_err_flag = 0;
	static int32_t running = 0;
	char **argv_tp;
	int32_t ercd = E_OK;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	/* show the help information, if the option is "-h" */
	if (argc == 2 && ntlibc_strcmp(argv[1],"-h") == 0x00) {
		cmd_adc_help(argv[0], extobj);
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

	if (running == 1) {
		CMD_DEBUG("ERRPR: ADC example is running already!\r\n");
		ercd = E_SYS;
		goto error_exit;
	}

	if (measure_times == 0) {
		CMD_DEBUG("ERROR: Measure times is 0. exit ADC Sample\r\n");
		ercd = E_SYS;
		goto error_exit;
	}

	if (period_ms > MAX_DELAY) {
		CMD_DEBUG("WARN: Max Measure Period is %d!\r\n", MAX_DELAY);
		period_ms = MAX_DELAY;
	}

	if (measure_times > MAX_TIMES) {
		CMD_DEBUG("WARN: Max Measure Times is %d!\r\n", MAX_TIMES);
		measure_times = MAX_TIMES;
	}

	/** get adc iic object */
	adc_iic = iic_get_dev(DW_IIC_0_ID);
	/** open as master device in poll method @ standard speed */
	adc_iic->iic_open(DEV_MASTER_MODE, IIC_SPEED_STANDARD);
	// print header
	CMD_DEBUG("\r\n*********************************\r\n");
	CMD_DEBUG("*    Read data from PmodAD2 ADC *\r\n");
	CMD_DEBUG("*    Period:%5dms  Times:%4d *\r\n", period_ms, measure_times);
	CMD_DEBUG("*********************************\r\n");
	CMD_DEBUG("Wait a while to exit. \r\n");

	param = I2C_SLAVE_ADDR;
	adc_iic->iic_control(IIC_CMD_MST_SET_TAR_ADDR, CONV2VOID(param));

	while (1) {

		for (i = 0; i < 4; i++) {

			adc[i] = 0;

			write_buf[0] = ad_channels[i];
			res = adc_iic->iic_write(write_buf, 1);

			// read data from i2c
			if (adc_iic->iic_read(read_buf, 2) < 0) {
				CMD_DEBUG("ERROR: cannot read data from I2C device\r\n");
				CMD_DEBUG("\r\n*** Exit ADC Sample***\r\n");
				running = 0;
				adc_iic->iic_close();
				goto error_exit;
			} else {
				res = read_buf[0];
				res = ((res << 8) & 0x0F00) | read_buf[1];
				adc[i] = res;
			}
		}

		CMD_DEBUG("ch0: data = 0x%-3X, ch1: data = 0x%-3X, ch2: data = 0x%-3X, ch3: data = 0x%-3X \r\n",
		          adc[0], adc[1], adc[2], adc[3]);

		board_delay_ms(period_ms, OSP_DELAY_OS_COMPAT_ENABLE);

		if (((++count) >= measure_times)) {
			CMD_DEBUG("\r\n*** Exit ADC Sample ***\r\n");
			running = 0;
			break;
		}
	}

	adc_iic->iic_close();
	return E_OK;
error_exit:
	return ercd;
}

static CMD_TABLE_T adc_cmd = {"adc", "PMOD ADC sensor demo command", cmd_adc, NULL};
/**
 * register adc example command
 */
CMD_TABLE_T *register_ntshell_cmd_adc(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&adc_cmd, prev);
}
#endif /* NTSHELL_USE_CMDS_PERIPHERAL_ADC */
