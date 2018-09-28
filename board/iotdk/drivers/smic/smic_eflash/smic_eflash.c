/* ------------------------------------------
 * Copyright (c) 2018, Synopsys, Inc. All rights reserved.

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

#include "arc.h"
#include "arc_builtin.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC_debug.h"

#include "smic_eflash.h"

#define SMIC_EFLASH_CHECK_EXP_NORTN(EXPR)	CHECK_EXP_NOERCD(EXPR, error_exit)
#define SMIC_EFLASH_CHECK_EXP(EXPR, ERROR_CODE) CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

#define FMC_BUSY_MASK   (1<<5)

typedef enum {
	FMC_CMD_DEFT        = 0, // default
	FMC_CMD_READ        = 1, // Flash Read
	FMC_CMD_PROG        = 2, // Flash Program
	FMC_CMD_PAGE_ERASE  = 3, // Code Flash page erase (512bytes per page)
	FMC_CMD_MACRO_ERASE = 7  // Erase whole code flash
} E_FMC_CMD;


static void smic_eflash_set_lock(SMIC_EFLASH_DEF_PTR obj, E_FMC_LOCK lock)
{
	obj->eflash_reg->FMCLOCK = lock;
}

static void smic_eflash_macro_erase(SMIC_EFLASH_DEF_PTR obj)
{
	obj->eflash_reg->FMCADR = EFLASH_BASE_ADDR + 0;
	obj->eflash_reg->FMCCMD = FMC_CMD_MACRO_ERASE;

	while (1) {
		if ((obj->eflash_reg->FMCCON & FMC_BUSY_MASK) != FMC_BUSY_MASK) {
			break;
		}
	}
}

static void smic_eflash_page_erase(SMIC_EFLASH_DEF_PTR obj, uint32_t ofst)
{
	obj->eflash_reg->FMCADR = EFLASH_BASE_ADDR + ofst;
	obj->eflash_reg->FMCCMD = FMC_CMD_PAGE_ERASE;

	while (1) {
		if ((obj->eflash_reg->FMCCON & FMC_BUSY_MASK) != FMC_BUSY_MASK) {
			break;
		}
	}
}

int32_t smic_eflash_open(SMIC_EFLASH_DEF_PTR obj)
{
	int32_t ercd = E_OK;

	SMIC_EFLASH_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_EFLASH_CHECK_EXP(obj->eflash_open_cnt == 0, E_OPNED);

	obj->eflash_open_cnt++;
	smic_eflash_set_lock(obj, FMC_UNLOCK);
	obj->eflash_lock = FMC_UNLOCK;

error_exit:
	return ercd;
}

int32_t smic_eflash_close(SMIC_EFLASH_DEF_PTR obj)
{
	int32_t ercd = E_OK;

	SMIC_EFLASH_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_EFLASH_CHECK_EXP(obj->eflash_open_cnt != 0, E_OPNED);

	obj->eflash_open_cnt = 0;
	smic_eflash_set_lock(obj, FMC_LOCK);
	obj->eflash_lock = FMC_LOCK;

error_exit:
	return ercd;
}

int32_t smic_eflash_read(SMIC_EFLASH_DEF_PTR obj, uint32_t addr, uint32_t len, uint8_t *val)
{
	int32_t ercd = E_OK;
	uint32_t i;
	uint8_t *p_addr = (uint8_t *)(EFLASH_BASE_ADDR + addr);

	SMIC_EFLASH_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_EFLASH_CHECK_EXP(obj->eflash_open_cnt != 0, E_OPNED);

	for (i = 0; i < len; i++) {
		*val++ = *p_addr++;
	}

error_exit:
	return ercd;
}

int32_t smic_eflash_write(SMIC_EFLASH_DEF_PTR obj, uint32_t addr, uint32_t len, uint8_t *val)
{
	int32_t ercd = E_OK;
	uint32_t i;
	uint8_t *p_addr = (uint8_t *)(EFLASH_BASE_ADDR + addr);

	SMIC_EFLASH_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_EFLASH_CHECK_EXP(obj->eflash_open_cnt != 0, E_OPNED);

	for (i = 0; i < len; i++) {
		*p_addr++ = *val++;

		while (1) {
			if ((obj->eflash_reg->FMCCON & FMC_BUSY_MASK) != FMC_BUSY_MASK) {
				break;
			}
		}
	}

error_exit:
	return ercd;
}

int32_t smic_eflash_control(SMIC_EFLASH_DEF_PTR obj, uint32_t ctrl_cmd, void *param)
{
	int32_t ercd = E_OK;

	SMIC_EFLASH_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_EFLASH_CHECK_EXP(obj->eflash_open_cnt != 0, E_OPNED);

	if (ctrl_cmd != SMIC_EFLASH_SET_LOCK && ctrl_cmd != SMIC_EFLASH_GET_LOCK &&
	    ctrl_cmd != SMIC_EFLASH_PAGE_ERASE && ctrl_cmd != SMIC_EFLASH_MACRO_ERASE &&
	    ctrl_cmd != SMIC_EFLASH_GET_INFO) {
		ercd = E_PAR;
		goto error_exit;
	}

	switch (ctrl_cmd) {
		case SMIC_EFLASH_SET_LOCK:
			SMIC_EFLASH_CHECK_EXP(*((E_FMC_LOCK *)param) != FMC_LOCK && \
			                      *((E_FMC_LOCK *)param) != FMC_UNLOCK, E_PAR);
			obj->eflash_reg->FMCLOCK = *((E_FMC_LOCK *)param);
			obj->eflash_lock = *((E_FMC_LOCK *)param);
			break;

		case SMIC_EFLASH_GET_LOCK:
			*((E_FMC_LOCK *)param) = obj->eflash_lock;
			break;

		case SMIC_EFLASH_PAGE_ERASE:
			smic_eflash_page_erase(obj, *((uint32_t *)param));
			break;

		case SMIC_EFLASH_MACRO_ERASE:
			smic_eflash_macro_erase(obj);
			break;

		case SMIC_EFLASH_GET_INFO:
			((SMIC_EFLASH_INFO *)param)->eflash_page_cnt = obj->eflash_page_cnt;
			((SMIC_EFLASH_INFO *)param)->eflash_page_size = obj->eflash_page_size;
			break;

		default:
			break;
	}

error_exit:
	return ercd;
}
