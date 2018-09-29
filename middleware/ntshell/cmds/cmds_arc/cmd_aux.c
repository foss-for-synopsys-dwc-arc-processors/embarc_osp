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
* \brief the command to access auxiliary registers
*/


#include "cmds_arc_cfg.h"
#if NTSHELL_USE_CMDS_ARC_AUX

#include "ntshell_common.h"

static NTSHELL_IO_PREDEF;

#define MAXROWS 4

#define MIN_AUX_REG_IDX	 0x2	 /* aux reg min index */
#define MAX_AUX_REG_IDX	 0x701   /* aux reg max index */

static void _auxiliary_register_dump(uint32_t start_addr, uint32_t end_addr, void *extobj);

static void cmd_lr_help(char *cmd_name, void *extobj);
static int cmd_lr(int argc, char **argv, void *extobj);
static void cmd_sr_help(char *cmd_name, void *extobj);
static int cmd_sr(int argc, char **argv, void *extobj);

static void _auxiliary_register_dump(uint32_t start_addr, uint32_t end_addr, void *extobj)
{
	uint32_t x;
	uint32_t p;
	uint32_t np;
	uint32_t brk = 0;
	uint32_t val;

	VALID_EXTOBJ_NORTN(extobj);

	p = start_addr;

	while (p <= end_addr) {
		CMD_DEBUG("0x%08x: ", p);

		for (x = 0; x < MAXROWS; x++) {
			val = _arc_aux_read(p);	/* read aux register */
			CMD_DEBUG("0x%08x  ", val);
			np = p + 1;

			if (np < p) {
				brk = 1;
				break;
			}

			p = np;

			if (p > end_addr) {
				break;
			}
		}

		CMD_DEBUG("\r\n");

		if (brk) {
			break;
		}
	}

error_exit:
	return;
}

/* show help of command lr */
static void cmd_lr_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]... <ADD> <VAL> [NUM]\r\n"
		"Read auxiliary register\r\n"
		"  -h/H/?    Show the help information\r\n"
		"  <ADD> [NUM]    Read [NUM] data from the auxiliary register, the beginning address: <ADD>, the number: [NUM] (the default value is 1)\r\n"
		"Examples: \r\n"
		"  lr 0x02 2    Read two data from the auxiliary register, the beginning address: <ADD>\r\n"
		"  lr -h    Show the help information\r\n", cmd_name);
error_exit:
	return;
}

/**
 * \brief  ntshell command : lr (read auxiliary register)
 * \param[in]  argc argument number count
 * \param[in]  argv arguments list
 * \retval
 *		  0   success
 *		  -1  failed
 */
static int cmd_lr(int argc, char **argv, void *extobj)
{
	long val;
	uint32_t start_addr, end_addr, size;
	uint32_t arg_err_flag = 0;
	char **argv_tp;
	int ercd = E_OK;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	/* show the help information, if the option is "-h" */
	if (argc == 2 && ntlibc_strcmp(argv[1],"-h") == 0x00) {
		cmd_lr_help(argv[0], extobj);
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

	/* parse 1th argument: start aux reg index */
	if (xatoi(&argv_tp[0], &val)) {
		start_addr = (uint32_t) val;
	} else {
		arg_err_flag = arg_err_flag | FIRST_ARG_ERR;
	}

	/* parse 2nd argument: aux reg size that you want to display */
	if (argc == 3) {
		if (xatoi(&argv_tp[1], &val)) {
			size = (uint32_t) val;
		} else {
			arg_err_flag = arg_err_flag | SECOND_ARG_ERR;
		}
	} else {
		size = 1; /* default only 1 aux reg displayed */
	}

	if (arg_err_flag != 0) { /* input argument format error */
		parse_arg_err(arg_err_flag, extobj);
		ercd = E_SYS;
		goto error_exit;
	}

	/* calc end address */
	end_addr = start_addr + size - 1;

	if ((end_addr < start_addr) && (size > 1)) { /* wrap? */
		end_addr = 0xfffffffc;
	}

	/* check address available ? */
	if ((start_addr < MIN_AUX_REG_IDX) || (end_addr > MAX_AUX_REG_IDX)) {
		CMD_DEBUG("input aux reg region was exceeded, min:0x%x, max:0x%x!\r\n",	MIN_AUX_REG_IDX, MAX_AUX_REG_IDX);
		ercd = E_SYS;
		goto error_exit;
	}

	_auxiliary_register_dump (start_addr, end_addr, extobj);

error_exit:
	return ercd;
}

/* show help of command sr */
static void cmd_sr_help(char *cmd_name, void *extobj)
{
	VALID_EXTOBJ_NORTN(extobj);

	if (cmd_name == NULL) {
		/* cmd_name not valid */
		return;
	}

	CMD_DEBUG("Usage: %s [OPTION]... <ADD> <VAL> [NUM]\r\n"
		"Write auxiliary register\r\n"
		"  -h/H/?    Show the help information\r\n"
		"  <ADD> <VAL>[NUM]    Write <VAL> to the auxiliary register, the begining address: <ADD>, the number: [NUM]/4 (the default value is 1)\r\n"
		"Examples: \r\n"
		"  sr 0x02 0x08 0x02    Write two 0x08 to the auxiliary register from the address <ADD>\r\n"
		"  sr -h    Show the help information\r\n", cmd_name);
error_exit:
	return;
}

/**
 * \brief  ntshell command : sr (write auxiliary register)
 * \param  argc[in] argument number count
 * \param  argv[in] arguments list
 * \return 0 success, -1 failure
 */
static int cmd_sr(int argc, char **argv, void *extobj)
{
	long val;
	uint32_t start_addr, end_addr, size, reg_val;
	uint32_t aux_addr;
	uint32_t arg_err_flag = 0;
	char **argv_tp;
	int32_t ercd = E_OK;

	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	/* show the help information, if the option is "-h" */
	if (argc == 2 && ntlibc_strcmp(argv[1],"-h") == 0x00) {
		cmd_sr_help(argv[0], extobj);
		goto error_exit;
	}

	/* the parameter checking */
	if ((argc > 4) || (argc < 3)) {
		ercd = E_SYS;
		CMD_DEBUG("command error!\r\n");
		CMD_DEBUG("Try '%s -h' for more information\r\n", argv[0]);
		goto error_exit;
	}

	argv_tp = (char **)(&argv[1]);

	/* parse 1th argument: start aux reg index */
	if (xatoi(&argv_tp[0], &val)) {
		start_addr = (uint32_t) val;
	} else {
		arg_err_flag = arg_err_flag | FIRST_ARG_ERR;
	}

	/* parse 2nd argument: reg value */
	if (xatoi(&argv_tp[1], &val)) {
		reg_val = (uint32_t) val;
	} else {
		arg_err_flag = arg_err_flag | SECOND_ARG_ERR;
	}

	/* parse 3rd argument: reg amount to write */
	if (argc == 4) {
		if (xatoi(&argv_tp[2], &val)) {
			size = (uint32_t) val;
		} else {
			arg_err_flag = arg_err_flag | THIRD_ARG_ERR;
		}
	} else {
		size = 1; /* default only 1 aux reg to be written */
	}

	if (arg_err_flag != 0) { /* input argument format error */
		parse_arg_err(arg_err_flag, extobj);
		ercd = E_SYS;
		goto error_exit;
	}

	/* calc end address */
	end_addr = start_addr + size - 1;

	if (end_addr < start_addr) { /* wrap? */
		end_addr = 0xfffffffc;
	}

	/* check address available ? */
	if ((start_addr < MIN_AUX_REG_IDX) || (end_addr > MAX_AUX_REG_IDX)) {
		CMD_DEBUG("input aux reg region was exceeded, min:0x%x, max:0x%x!\r\n",	MIN_AUX_REG_IDX, MAX_AUX_REG_IDX);
		ercd = E_SYS;
		goto error_exit;
	}

	CMD_DEBUG("Write auxiliary register from 0x%x to 0x%x with 0x%08x\r\n",	start_addr, end_addr, reg_val);

	/* perform aux register fill */
	for (aux_addr = start_addr; aux_addr <= end_addr; aux_addr += 1) {
		_arc_aux_write(aux_addr, reg_val);
	}

error_exit:
	return ercd;
}

static CMD_TABLE_T lr_cmd = {"lr", "Read auxiliary register", cmd_lr, NULL};
static CMD_TABLE_T sr_cmd = {"sr", "Write auxiliary register", cmd_sr, NULL};
/**
 * register aux register write or read command
 */
CMD_TABLE_T *register_ntshell_cmd_aux(CMD_TABLE_T *prev)
{
	prev = ntshell_usrcmd_register(&lr_cmd, prev);
	return ntshell_usrcmd_register(&sr_cmd, prev);
}

#endif /* NTSHELL_USE_CMDS_ARC_AUX */
