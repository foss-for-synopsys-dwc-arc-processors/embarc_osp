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

#include "iotdk/drivers/smic/smic_eflash/smic_eflash.h"
#include "iotdk/drivers/smic/smic_bootspi/smic_bootspi.h"

#include "target_mem_config.h"
#include "flash_obj.h"
#include <string.h>

#define DW_FLASH_CHECK_EXP(EXPR, ERROR_CODE)		CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

#if (USE_IOTDK_EFLASH)
static DEV_FLASH iotdk_eflash_obj;
EFLASH_DEFINE(eflash, EFLASH_CTRL_BASE);

static int32_t iotdk_eflash_open(void)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &iotdk_eflash_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt == 0, E_OPNED);

	info_ptr->open_cnt++;
	ercd = smic_eflash_open(eflash);

error_exit:
	return ercd;
}

static int32_t iotdk_eflash_close(void)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &iotdk_eflash_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt != 0, E_OK);

	info_ptr->open_cnt--;
	ercd = smic_eflash_close(eflash);

error_exit:
	return ercd;
}

static int32_t iotdk_eflash_control(uint32_t cmd, void *param)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &iotdk_eflash_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt > 0, E_CLSED);

	switch (cmd) {
		case FLASH_CMD_GET_INFO:
			((DEV_FLASH_INFO *)param)->begin_addr = info_ptr->begin_addr;
			((DEV_FLASH_INFO *)param)->total_size = info_ptr->total_size;
			break;

		case FLASH_CMD_PAGE_ERASE:
			ercd = smic_eflash_control(eflash, SMIC_EFLASH_PAGE_ERASE, param);
			break;

		case FLASH_CMD_CHIP_ERASE:
			ercd = smic_eflash_control(eflash, SMIC_EFLASH_MACRO_ERASE, param);
			break;

		default:
			ercd = E_NOSPT;
			break;
	}

error_exit:
	return ercd;
}

static int32_t iotdk_eflash_read(uint32_t addr, void *dst, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &iotdk_eflash_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt > 0, E_CLSED);
	DW_FLASH_CHECK_EXP(info_ptr->begin_addr <= addr, E_PAR);

	if ((addr + len) > (info_ptr->begin_addr + info_ptr->total_size)) {
		len = info_ptr->begin_addr + info_ptr->total_size - len;
	}

	ercd = smic_eflash_read(eflash, addr, len, dst);

error_exit:
	return ercd;
}

static int32_t iotdk_eflash_write(uint32_t addr, void *src, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &iotdk_eflash_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt > 0, E_CLSED);
	DW_FLASH_CHECK_EXP(info_ptr->begin_addr <= addr, E_PAR);

	if ((addr + len) > (info_ptr->begin_addr + info_ptr->total_size)) {
		len = info_ptr->begin_addr + info_ptr->total_size - len;
	}

	ercd = smic_eflash_write(eflash, addr, len, src);

error_exit:
	return ercd;
}

static int32_t iotdk_eflash_write_nocheck(uint32_t addr, void *src, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &iotdk_eflash_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt > 0, E_CLSED);
	DW_FLASH_CHECK_EXP(info_ptr->begin_addr <= addr, E_PAR);

	if ((addr + len) > (info_ptr->begin_addr + info_ptr->total_size)) {
		len = info_ptr->begin_addr + info_ptr->total_size - len;
	}

	ercd = smic_eflash_write_nocheck(eflash, addr, len, src);

error_exit:
	return ercd;
}

static int32_t iotdk_eflash_erase(uint32_t addr, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &iotdk_eflash_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt > 0, E_CLSED);
	DW_FLASH_CHECK_EXP(info_ptr->begin_addr <= addr, E_PAR);

	if ((addr + len) > (info_ptr->begin_addr + info_ptr->total_size)) {
		len = info_ptr->begin_addr + info_ptr->total_size - len;
	}

	ercd = smic_eflash_erase(eflash, addr, len);

error_exit:
	return ercd;
}

static void iotdk_eflash_install(void)
{
	DEV_FLASH_PTR obj_ptr = &iotdk_eflash_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	info_ptr->begin_addr = 0;
	info_ptr->total_size = SMIC_EFLASH_PAGE_SIZE * SMIC_EFLASH_PAGE_CNT;
	info_ptr->page_size = SMIC_EFLASH_PAGE_SIZE;
	info_ptr->page_cnt = SMIC_EFLASH_PAGE_CNT;
	info_ptr->align_size = 1;
	info_ptr->open_cnt = 0;

	obj_ptr->flash_open = iotdk_eflash_open;
	obj_ptr->flash_close = iotdk_eflash_close;
	obj_ptr->flash_control = iotdk_eflash_control;
	obj_ptr->flash_write = iotdk_eflash_write;
	obj_ptr->flash_write_nocheck = iotdk_eflash_write_nocheck;
	obj_ptr->flash_read = iotdk_eflash_read;
	obj_ptr->flash_erase = iotdk_eflash_erase;
}
#endif

#if (USE_IOTDK_BOOT_SPI_FLASH)
static DEV_FLASH iotdk_bootspi_obj;
BOOTSPI_DEFINE(bootspi, BOOTSPI_CTRL_BASE);

static int32_t iotdk_bootspi_open(void)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &iotdk_bootspi_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt == 0, E_OPNED);

	info_ptr->open_cnt++;
	ercd = smic_bootspi_open(bootspi);

error_exit:
	return ercd;
}

static int32_t iotdk_bootspi_close(void)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &iotdk_bootspi_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt != 0, E_OK);

	info_ptr->open_cnt--;
	ercd = smic_bootspi_close(bootspi);

error_exit:
	return ercd;
}

static int32_t iotdk_bootspi_control(uint32_t cmd, void *param)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &iotdk_bootspi_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt > 0, E_CLSED);

	switch (cmd) {
		case FLASH_CMD_GET_INFO:
			((DEV_FLASH_INFO *)param)->begin_addr = info_ptr->begin_addr;
			((DEV_FLASH_INFO *)param)->total_size = info_ptr->total_size;
			break;

		case FLASH_CMD_PAGE_ERASE:
			ercd = smic_bootspi_control(bootspi, SMIC_BOOTSPI_SEC_ERASE, param);
			break;

		case FLASH_CMD_CHIP_ERASE:
			ercd = smic_bootspi_control(bootspi, SMIC_BOOTSPI_CHIP_ERASE, param);
			break;

		default:
			ercd = E_NOSPT;
			break;
	}

error_exit:
	return ercd;
}

static int32_t iotdk_bootspi_read(uint32_t addr, void *dst, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &iotdk_bootspi_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt > 0, E_CLSED);
	DW_FLASH_CHECK_EXP(info_ptr->begin_addr <= addr, E_PAR);

	if ((addr + len) > (info_ptr->begin_addr + info_ptr->total_size)) {
		len = info_ptr->begin_addr + info_ptr->total_size - len;
	}
	addr -= info_ptr->begin_addr;

	ercd = smic_bootspi_read(bootspi, addr, len, dst);

error_exit:
	return ercd;
}

static int32_t iotdk_bootspi_write(uint32_t addr, void *src, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &iotdk_bootspi_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt > 0, E_CLSED);
	DW_FLASH_CHECK_EXP(info_ptr->begin_addr <= addr, E_PAR);

	if ((addr + len) > (info_ptr->begin_addr + info_ptr->total_size)) {
		len = info_ptr->begin_addr + info_ptr->total_size - len;
	}
	addr -= info_ptr->begin_addr;
	ercd = smic_bootspi_write(bootspi, addr, len, src);

error_exit:
	return ercd;
}

static int32_t iotdk_bootspi_write_nocheck(uint32_t addr, void *src, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &iotdk_bootspi_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt > 0, E_CLSED);
	DW_FLASH_CHECK_EXP(info_ptr->begin_addr <= addr, E_PAR);

	if ((addr + len) > (info_ptr->begin_addr + info_ptr->total_size)) {
		len = info_ptr->begin_addr + info_ptr->total_size - len;
	}
	addr -= info_ptr->begin_addr;
	ercd = smic_bootspi_write_nocheck(bootspi, addr, len, src);

error_exit:
	return ercd;
}

static int32_t iotdk_bootspi_erase(uint32_t addr, uint32_t len)
{
	int32_t ercd = E_OK;
	DEV_FLASH_PTR obj_ptr = &iotdk_bootspi_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	DW_FLASH_CHECK_EXP(info_ptr->open_cnt > 0, E_CLSED);
	DW_FLASH_CHECK_EXP(info_ptr->begin_addr <= addr, E_PAR);

	if ((addr + len) > (info_ptr->begin_addr + info_ptr->total_size)) {
		len = info_ptr->begin_addr + info_ptr->total_size - len;
	}
	addr -= info_ptr->begin_addr;
	ercd = smic_bootspi_erase(bootspi, addr, len);

error_exit:
	return ercd;
}

static void iotdk_bootspi_install(void)
{
	DEV_FLASH_PTR obj_ptr = &iotdk_bootspi_obj;
	DEV_FLASH_INFO_PTR info_ptr = &(obj_ptr->flash_info);

	info_ptr->begin_addr = 0x10000000;
	info_ptr->total_size = SMIC_BOOTSPI_SEC_SIZE * SMIC_BOOTSPI_SECS_PER_CHIP;
	info_ptr->page_size = SMIC_BOOTSPI_SEC_SIZE;
	info_ptr->page_cnt = SMIC_BOOTSPI_SECS_PER_CHIP;
	info_ptr->align_size = 1;
	info_ptr->open_cnt = 0;

	obj_ptr->flash_open = iotdk_bootspi_open;
	obj_ptr->flash_close = iotdk_bootspi_close;
	obj_ptr->flash_control = iotdk_bootspi_control;
	obj_ptr->flash_write = iotdk_bootspi_write;
	obj_ptr->flash_write_nocheck = iotdk_bootspi_write_nocheck;
	obj_ptr->flash_read = iotdk_bootspi_read;
	obj_ptr->flash_erase = iotdk_bootspi_erase;
}
#endif

static void iotdk_flash_obj_all_install(void)
{
#if (USE_IOTDK_EFLASH)
	iotdk_eflash_install();
#endif

#if (USE_IOTDK_BOOT_SPI_FLASH)
	iotdk_bootspi_install();
#endif
}

DEV_FLASH_PTR flash_get_dev(int32_t flash_id)
{
	static uint32_t install_flag = 0;

	/* intall device objects */
	if (install_flag == 0) {
		install_flag = 1;
		iotdk_flash_obj_all_install();
	}

	switch (flash_id) {
#if (USE_IOTDK_EFLASH)
		case IOTDK_EFLASH_ID:
			return &iotdk_eflash_obj;
			break;
#endif

#if (USE_IOTDK_BOOT_SPI_FLASH)
		case IOTDK_BOOT_SPI_FLASH_ID:
			return &iotdk_bootspi_obj;
			break;
#endif
		default:
			break;
	}
	return NULL;
}
