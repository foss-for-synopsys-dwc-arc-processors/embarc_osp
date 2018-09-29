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
* \brief read the PMOD temperature sensor data
*/
#include "cmds_peripheral_cfg.h"
#if NTSHELL_USE_CMDS_PERIPHERAL_TEMP
#include "ntlibc.h"
#include "ntshell_common.h"
#include "adt7420.h"

static NTSHELL_IO_PREDEF;

#define CELSIUS    1
#define FAHRENHEIT 0

static ADT7420_DEFINE(temp_sensor, BOARD_TEMP_SENSOR_IIC_ID, TEMP_I2C_SLAVE_ADDRESS);

static int32_t read_temp (int tmp_units);

/* show help of command temp */
static void cmd_temp_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]\r\n"
		"Show the current temperature, connect PMOD TMP2 to J2\r\n"
		"  -h/H/?    Show the help information\r\n"
		"  -c        Show the current temperatuen with Celsius\r\n"
		"  -f        Show the current temperatuen with Fathrenheit\r\n"
		"Examples: \r\n"
		"  temp -c    Show the current temperatuen with Celsius\r\n"
		"  temp -h    Show the help information\r\n", cmd_name);

error_exit:
	return;
}

static int cmd_temp(int argc, char **argv, void *extobj)
{
	int32_t unit;
	int32_t temp;
	int32_t ercd = E_OK;
	int32_t opt;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	if ((argc > 3) || (argc < 2)) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	}

	opterr = 0;
	optind = 1;

	while ((opt=getopt(argc, argv, "cfhH?")) != -1) {
		switch (opt) {
			case 'h':
			case '?':
			case 'H':
				cmd_temp_help(argv[0], extobj);
				goto error_exit;
				break;

			case 'c':
				unit = CELSIUS;
				break;

			case 'f':
				unit = FAHRENHEIT;
				break;

			default:
				CMD_DEBUG("%s: unrecognized option:%c\r\n", argv[0], opt);
				CMD_DEBUG("Try '%s -h' for more information\r\n",argv[0]);
				break;
		}
	}

	adt7420_sensor_init(temp_sensor);

	if ((temp=read_temp(unit)) != -1000) {
		CMD_DEBUG("Current Temperature:%d.%d", temp/10, temp%10);

		if (unit == FAHRENHEIT) {
			CMD_DEBUG("F\r\n");
		} else {
			CMD_DEBUG("C\r\n");
		}
	} else {
		CMD_DEBUG("ERROR: cannot read data from I2C device\r\n");
		return -1;
	}

	return 0;

error_exit:
	return ercd;
}

/*****************************************************************************/
/* I2C Temperature Sensor PmodTmp2 *
 * return F/C temperature (0.1 F/C)
 * -1000 fail */
/*****************************************************************************/
int32_t read_temp (int tmp_units)
{
	int32_t tempval;
	float tempf;

	//read from sensor
	if (adt7420_sensor_read(temp_sensor, &tempf) == E_OK) {
		tempval = (int32_t)(tempf * 10);

		if (tmp_units == FAHRENHEIT) {
			//conversion F = 32 + (9/5) * C
			//t is 0.1c unit
			tempval = 320 + ((9*tempval)/5); /* 0.1 F */
		}
	} else {
		return -1000;
	}

	return tempval;
}

static CMD_TABLE_T temp_cmd = {"temp", "Show current temperature", cmd_temp, NULL};
/**
 * register temp command
 */
CMD_TABLE_T *register_ntshell_cmd_temp(CMD_TABLE_T *prev)
{
	return ntshell_usrcmd_register(&temp_cmd, prev);
}

#endif /* NTSHELL_USE_CMDS_PERIPHERAL_TEMP */
