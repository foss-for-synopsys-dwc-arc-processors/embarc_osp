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
 * \brief process the error of arguments
 */
#include "embARC_debug.h"

#include "ntshell_common.h"

#include "embARC_error.h"
#include "ntshell_task.h"

/** Parse ntshell argument error */
void parse_arg_err(unsigned int errorflag, void *extobj)
{
	int i = 0;
	NTSHELL_IO_PREDEF;

	NTSHELL_IO_GET(extobj);

	if (errorflag == 0) {
		return; /* no error*/
	} else {
		CMD_DEBUG("Input Arguments:< ");
	}

	while (i < 8) {
		switch (errorflag & (1<<i)) {
			case FIRST_ARG_ERR   :
				CMD_DEBUG("1st, ");
				break;

			case SECOND_ARG_ERR  :
				CMD_DEBUG("2nd, ");
				break;

			case THIRD_ARG_ERR   :
				CMD_DEBUG("3rd, ");
				break;

			case FOURTH_ARG_ERR  :
				CMD_DEBUG("4th, ");
				break;

			case FIFTH_ARG_ERR   :
				CMD_DEBUG("5th, ");
				break;

			case SIXTH_ARG_ERR   :
				CMD_DEBUG("6th, ");
				break;

			case SEVENTH_ARG_ERR :
				CMD_DEBUG("7th, ");
				break;

			case EIGHTH_ARG_ERR  :
				CMD_DEBUG("8th  ");
				break;

			default:
				break;
		}

		i++;
	}

	CMD_DEBUG("> Format Error!\r\n");
}

/*----------------------------------------------*/
/* Dump a line of binary dump                   */
/*----------------------------------------------*/
void cmds_put_dump (
    const void *buffer,		/* Pointer to the array to be dumped */
    unsigned long addr,		/* Heading address value */
    int32_t len,			/* Number of items to be dumped */
    int32_t width,			/* Size of the items (DF_CHAR, DF_SHORT, DF_LONG) */
    void *extobj
)
{
	int32_t ercd = E_OK;
	int32_t i;
	const unsigned char *bp;
	const unsigned short *sp;
	const unsigned long *lp;

	NTSHELL_IO_PREDEF;
	VALID_EXTOBJ(extobj, -1);
	NTSHELL_IO_GET(extobj);

	CMD_DEBUG("0x%08lX ", addr);				/* address */

	switch (width) {
		case DW_CHAR:
			bp = buffer;

			for (i = 0; i < len; i++) {		/* Hexdecimal dump */
				CMD_DEBUG(" %02X", bp[i]);
			}

			CMD_DEBUG("  ");

			for (i = 0; i < len; i++) {		/* ASCII dump */
				CMD_DEBUG("%c", (bp[i] >= ' ' && bp[i] <= '~') ? bp[i] : '.');
			}

			break;

		case DW_SHORT:
			sp = buffer;

			do {					/* Hexdecimal dump */
				CMD_DEBUG(" %04X", *sp++);
			} while (--len);

			break;

		case DW_LONG:
			lp = buffer;

			do {					/* Hexdecimal dump */
				CMD_DEBUG(" %08LX", *lp++);
			} while (--len);

			break;
	}

	CMD_DEBUG("\r\n");
	return;

error_exit:
	return;
}