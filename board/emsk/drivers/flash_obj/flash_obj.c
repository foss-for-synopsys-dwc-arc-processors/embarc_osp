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

#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "arc_exception.h"


#include "target_mem_config.h"
#include "flash_obj.h"
#include <string.h>

#define DW_FLASH_CHECK_EXP(EXPR, ERROR_CODE)		CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

void emsk_flash_obj_all_install(void);

#if (USE_EMSK_DDR_RAM)
#define EMSK_DDR_RAM_BEGIN_ADDR 0x10000000
#define EMSK_DDR_RAM_TOTAL_SIZE 0x8000000
static DEV_FLASH emsk_ddr_ram_obj;

static int32_t emsk_ddr_ram_open()
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &emsk_ddr_ram_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	info_ptr->open_cnt++;
	if (info_ptr->open_cnt > 1) {
		return E_OPNED;
	}

error_exit:
	return ercd;
}

static int32_t emsk_ddr_ram_close()
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &emsk_ddr_ram_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	info_ptr->open_cnt--;
	if (info_ptr->open_cnt == 0) {
		return E_OK;
	} else {
		ercd = E_OPNED;
	}

error_exit:
	return ercd;
}

static int32_t emsk_ddr_ram_control(uint32_t cmd, void *param)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &emsk_ddr_ram_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt > 0, E_CLSED);

	switch (cmd) {
		case FLASH_CMD_GET_INFO:
			((DEV_FLASH_INFO *)param)->begin_addr = info_ptr->begin_addr;
			((DEV_FLASH_INFO *)param)->total_size = info_ptr->total_size;
		break;
	}



error_exit:
	return ercd;
}

static int32_t emsk_ddr_ram_read(uint32_t addr, void *dst, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &emsk_ddr_ram_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt > 0, E_CLSED);
	DW_FLASH_CHECK_EXP(info_ptr->begin_addr <= addr, E_PAR);

	if ((addr + len) > (info_ptr->begin_addr + info_ptr->total_size)) {
		len = info_ptr->begin_addr + info_ptr->total_size - len;
	}

	memcpy((void *)dst, (const void *)addr, len);

	return len;

error_exit:
	return ercd;
}

static int32_t emsk_ddr_ram_write(uint32_t addr, void *src, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &emsk_ddr_ram_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt > 0, E_CLSED);
	DW_FLASH_CHECK_EXP(info_ptr->begin_addr <= addr, E_PAR);

	if ((addr + len) > (info_ptr->begin_addr + info_ptr->total_size)) {
		len = info_ptr->begin_addr + info_ptr->total_size - len;
	}

	memcpy((void *)addr, (const void *)src, len);

	return len;

error_exit:
	return ercd;
}

static int32_t emsk_ddr_ram_write_nocheck(uint32_t addr, void *src, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &emsk_ddr_ram_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt > 0, E_CLSED);
	DW_FLASH_CHECK_EXP(info_ptr->begin_addr <= addr, E_PAR);

	if ((addr + len) > (info_ptr->begin_addr + info_ptr->total_size)) {
		len = info_ptr->begin_addr + info_ptr->total_size - len;
	}

	memcpy((void *)addr, (const void *)src, len);

	return len;

error_exit:
	return ercd;
}

static int32_t emsk_ddr_ram_erase(uint32_t addr, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &emsk_ddr_ram_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt > 0, E_CLSED);
	DW_FLASH_CHECK_EXP(info_ptr->begin_addr <= addr, E_PAR);

	if ((addr + len) > (info_ptr->begin_addr + info_ptr->total_size)) {
		len = info_ptr->begin_addr + info_ptr->total_size - len;
	}

	memset((void *)addr, 0xFF, len);

	return len;

error_exit:
	return ercd;
}
static void emsk_ddr_ram_install(void)
{
	DEV_FLASH_PTR obj_ptr = &emsk_ddr_ram_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	info_ptr->begin_addr = EMSK_DDR_RAM_BEGIN_ADDR;
	info_ptr->total_size = EMSK_DDR_RAM_TOTAL_SIZE;
	info_ptr->page_size = 0;
	info_ptr->page_cnt = 0;
	info_ptr->align_size = 1;
	info_ptr->open_cnt = 0;

	obj_ptr->flash_open = emsk_ddr_ram_open;
	obj_ptr->flash_close = emsk_ddr_ram_close;
	obj_ptr->flash_control = emsk_ddr_ram_control;
	obj_ptr->flash_write = emsk_ddr_ram_write;
	obj_ptr->flash_write_nocheck = emsk_ddr_ram_write_nocheck;
	obj_ptr->flash_read = emsk_ddr_ram_read;
	obj_ptr->flash_erase = emsk_ddr_ram_erase;
}
#endif



DEV_FLASH_PTR flash_get_dev(int32_t flash_id)
{
	static uint32_t install_flag = 0;

	/* install device objects */
	if (install_flag == 0) {
		install_flag = 1;
		emsk_flash_obj_all_install();
	}

	switch (flash_id) {
#if (USE_EMSK_DDR_RAM)

		case EMSK_DDR_RAM_ID:
			return &emsk_ddr_ram_obj;
			break;
#endif
		default:
			break;
	}

	return NULL;
}


void emsk_flash_obj_all_install(void)
{
#if (USE_EMSK_DDR_RAM)
	emsk_ddr_ram_install();
#endif
}
