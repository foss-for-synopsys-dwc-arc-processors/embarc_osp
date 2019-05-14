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

#include "smic_bootspi.h"
#include "string.h"
#include "stdlib.h"

// QSPI status
#define SMIC_BOOTSPI_CHECK_EXP_NORTN(EXPR)		CHECK_EXP_NOERCD(EXPR, error_exit)
#define SMIC_BOOTSPI_CHECK_EXP(EXPR, ERROR_CODE)	CHECK_EXP(EXPR, ercd, ERROR_CODE, error_exit)

#define SMIC_BOOTSPI_MMA_MASK		(1<<0)
#define SMIC_BOOTSPI_NBR_MASK		(1<<1)
#define SMIC_BOOTSPI_RMMCS_MASK		(1<<7)

// read command
#define SMIC_BOOTSPI_NOR_CMD_SREAD	0x03	// Normal read
#define SMIC_BOOTSPI_NOR_CMD_DREAD	0x3B	// Dual read
#define SMIC_BOOTSPI_NOR_CMD_QREAD	0x6B	// quad read

// program command
#define SMIC_BOOTSPI_NOR_CMD_PP		0x02	// Page Program

// erase command
#define SMIC_BOOTSPI_NOR_CMD_SE		0x20	// Sector Erase
#define SMIC_BOOTSPI_NOR_CMD_BE_32K	0x52	// 32KB Block Erase
#define SMIC_BOOTSPI_NOR_CMD_BE_64K	0xD8	// 64KB Block Erase
#define SMIC_BOOTSPI_NOR_CMD_CE		0x60	// Chip Erase(0x60/0xC7)

#define SMIC_BOOTSPI_NOR_RD_ID		0x9F	// JEDEC ID

// nor flash common command
#define SMIC_BOOTSPI_NOR_CMD_RSTEN	0x66	// Reset enable
#define SMIC_BOOTSPI_NOR_CMD_RST	0x99	// Reset memory

#define SMIC_BOOTSPI_NOR_CMD_RDSR	0x05	// Read status register(macornix)
// Read Status Register-1(winbond)
// Read Status Register-1(issi)

#define SMIC_BOOTSPI_NOR_CMD_WREN	0x06	// Write Enable
#define SMIC_BOOTSPI_NOR_CMD_WRDI	0x04	// Write Disable
#define SMIC_BOOTSPI_NOR_CMD_WRSR	0x01	// Write status register

static void bootspi_flush(SMIC_BOOTSPI_DEF_PTR obj)
{
	uint32_t reg_val;
	obj->bootspi_reg->MMS = SMIC_BOOTSPI_RMMCS_MASK;

	// check the NBR
	while (1) {
		reg_val = obj->bootspi_reg->MMS;

		if ((reg_val & SMIC_BOOTSPI_MMA_MASK) != SMIC_BOOTSPI_MMA_MASK) {
			break;
		}
	}
}

static void bootspi_write_data(SMIC_BOOTSPI_DEF_PTR obj, uint8_t data)
{
	uint32_t reg_val;

	obj->bootspi_reg->MMDW = data;

	// check the NBR
	while (1) {
		reg_val = obj->bootspi_reg->MMS;

		if (reg_val & SMIC_BOOTSPI_NBR_MASK) {
			break;
		}
	}
}

static uint8_t bootspi_read_data(SMIC_BOOTSPI_DEF_PTR obj)
{
	bootspi_write_data(obj, 0x00);
	return (obj->bootspi_reg->MMDR & 0xFF);
}

static uint8_t bootspi_read_status(SMIC_BOOTSPI_DEF_PTR obj)
{
	uint8_t st;

	bootspi_flush(obj);
	bootspi_write_data(obj, SMIC_BOOTSPI_NOR_CMD_RDSR);
	st = bootspi_read_data(obj);
	bootspi_flush(obj);

	return st;
}

static uint8_t bootspi_chk_status(SMIC_BOOTSPI_DEF_PTR obj)
{
	uint8_t status;

	while (1) {
		status = bootspi_read_status(obj);

		if (!(status & 0x1)) {
			return status;
		}
	}
}

static void bootspi_reset(SMIC_BOOTSPI_DEF_PTR obj)
{
	bootspi_flush(obj);
	// issue write enable command
	bootspi_write_data(obj, SMIC_BOOTSPI_NOR_CMD_WREN);
	// issue reset enable command
	bootspi_write_data(obj, SMIC_BOOTSPI_NOR_CMD_RSTEN);
	// issue reset command
	bootspi_write_data(obj, SMIC_BOOTSPI_NOR_CMD_RST);
	// check status
	bootspi_chk_status(obj);
	bootspi_flush(obj);
}

static void bootspi_read_id(SMIC_BOOTSPI_DEF_PTR obj, uint8_t *p_id, uint32_t size)
{
	uint32_t i;

	bootspi_flush(obj);
	bootspi_write_data(obj, SMIC_BOOTSPI_NOR_RD_ID);

	for (i = 0; i < size; i++) {
		p_id[i] = bootspi_read_data(obj);
	}

	bootspi_flush(obj);
}

static void bootspi_chip_erase(SMIC_BOOTSPI_DEF_PTR obj)
{
	bootspi_flush(obj);
	// issue write enable command
	bootspi_write_data(obj, SMIC_BOOTSPI_NOR_CMD_WREN);
	bootspi_flush(obj);
	// issue chip erase command
	bootspi_write_data(obj, SMIC_BOOTSPI_NOR_CMD_CE);
	// check status
	bootspi_chk_status(obj);
	bootspi_flush(obj);
}

static void bootspi_sec_erase(SMIC_BOOTSPI_DEF_PTR obj, uint32_t addr)
{
	int32_t i;

	bootspi_flush(obj);
	// issue write enable command
	bootspi_write_data(obj, SMIC_BOOTSPI_NOR_CMD_WREN);
	bootspi_flush(obj);
	// issue sector erase command
	bootspi_write_data(obj, SMIC_BOOTSPI_NOR_CMD_SE);

	// issue address
	for (i = 2; i >= 0; i--) {
		bootspi_write_data(obj, (addr >> (i<<3)) & 0xFF);
	}

	// check status
	bootspi_chk_status(obj);
	bootspi_flush(obj);
}

static void bootspi_blk_erase(SMIC_BOOTSPI_DEF_PTR obj, uint32_t addr)
{
	int32_t i;

	bootspi_flush(obj);
	// issue write enable command
	bootspi_write_data(obj, SMIC_BOOTSPI_NOR_CMD_WREN);
	bootspi_flush(obj);
	// issue sector erase command
	bootspi_write_data(obj, SMIC_BOOTSPI_NOR_CMD_BE_64K);

	// issue address
	for (i = 2; i >= 0; i--) {
		bootspi_write_data(obj, (addr >> (i<<3)) & 0xFF);
	}

	// check status
	bootspi_chk_status(obj);
	bootspi_flush(obj);
}

static void bootspi_write(SMIC_BOOTSPI_DEF_PTR obj, uint32_t addr, uint32_t len, uint8_t *val)
{
	int32_t i;
	uint8_t st;
	bootspi_flush(obj);
	// issue write enable command
	bootspi_write_data(obj, SMIC_BOOTSPI_NOR_CMD_WREN);
	bootspi_flush(obj);
	// issue page program command
	bootspi_write_data(obj, SMIC_BOOTSPI_NOR_CMD_PP);

	// issue address
	for (i = 2; i >= 0; i--) {
		bootspi_write_data(obj, (addr >> (i<<3)) & 0xFF);
	}

	// write data
	for (i = 0; i < len; i++) {
		bootspi_write_data(obj, val[i]);
	}

	bootspi_flush(obj);
	// check status
	st = bootspi_chk_status(obj);
	bootspi_flush(obj);
}

int32_t smic_bootspi_open(SMIC_BOOTSPI_DEF_PTR obj)
{
	int32_t ercd = E_OK;

	SMIC_BOOTSPI_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_BOOTSPI_CHECK_EXP(obj->bootspi_open_cnt == 0, E_OPNED);
	obj->bootspi_open_cnt++;
	// //Sclk 1.17 MHz, Fsystem/64 (low rate)
	// obj->bootspi_reg->MMC = 0x00;
	//Sclk 18.75 MHz, Fsystem/16 (high rate)
	obj->bootspi_reg->MMC = 0x05;
	//Manual Mode Enable
	obj->bootspi_reg->MME = 0x01;

error_exit:
	return ercd;
}

int32_t smic_bootspi_close(SMIC_BOOTSPI_DEF_PTR obj)
{
	int32_t ercd = E_OK;

	SMIC_BOOTSPI_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_BOOTSPI_CHECK_EXP(obj->bootspi_open_cnt != 0, E_OPNED);
	//Manual Mode Disable
	obj->bootspi_reg->MME = 0x00;
	obj->bootspi_open_cnt = 0;

error_exit:
	return ercd;
}

int32_t smic_bootspi_read(SMIC_BOOTSPI_DEF_PTR obj, uint32_t addr, uint32_t len, uint8_t *val)
{
	int32_t ercd = E_OK;
	int32_t i;

	SMIC_BOOTSPI_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_BOOTSPI_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_BOOTSPI_CHECK_EXP(obj->bootspi_open_cnt != 0, E_OPNED);

	bootspi_flush(obj);
	// issue read command
	bootspi_write_data(obj, SMIC_BOOTSPI_NOR_CMD_SREAD);

	// issue address
	for (i = 2; i >= 0; i--) {
		bootspi_write_data(obj, (addr >> (i<<3)) & 0xFF);
	}

	// read data
	for (i = 0; i < len; i++) {
		val[i] = bootspi_read_data(obj);
	}

	bootspi_flush(obj);

	return (int32_t)(len);
error_exit:
	return ercd;
}

int32_t smic_bootspi_write_nocheck(SMIC_BOOTSPI_DEF_PTR obj, uint32_t addr, uint32_t len, uint8_t *val)
{
	int32_t ercd = E_OK;
	uint32_t first = 0;
	uint32_t size_orig = len;

	SMIC_BOOTSPI_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_BOOTSPI_CHECK_EXP(obj->bootspi_open_cnt != 0, E_OPNED);

	first = SMIC_BOOTSPI_PAGE_SIZE - (addr & (SMIC_BOOTSPI_PAGE_SIZE - 1));

	do {
		first = first < len ? first : len;
		bootspi_write(obj, addr, first, val);
		len -= first;
		addr += first;
		val += first;
		first = SMIC_BOOTSPI_PAGE_SIZE;
	} while (len);

	return (int32_t)(size_orig);

error_exit:
	return ercd;
}

int32_t smic_bootspi_write(SMIC_BOOTSPI_DEF_PTR obj, uint32_t addr, uint32_t len, uint8_t *val)
{
	int32_t ercd = E_OK;
	uint32_t i;
	uint32_t size_orig = len;
	uint16_t sec_pos;
	uint16_t sec_off;
	uint16_t sec_remain;
	uint8_t *m_buf;

	SMIC_BOOTSPI_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_BOOTSPI_CHECK_EXP(obj->bootspi_open_cnt != 0, E_OPNED);

	m_buf = malloc(SMIC_BOOTSPI_SEC_SIZE);

	SMIC_BOOTSPI_CHECK_EXP(m_buf != NULL, E_NOMEM);

	sec_pos = addr / SMIC_BOOTSPI_SEC_SIZE;
	sec_off = addr % SMIC_BOOTSPI_SEC_SIZE;
	sec_remain = SMIC_BOOTSPI_SEC_SIZE - sec_off;

	if (len <= sec_remain) {
		sec_remain = len;
	}

	while (1) {
		smic_bootspi_read(obj, sec_pos * SMIC_BOOTSPI_SEC_SIZE, SMIC_BOOTSPI_SEC_SIZE, m_buf);
		for (i = 0; i < sec_remain; i++) {
			if (m_buf[sec_off + i] != 0xFF) break;
		}
		if (i < sec_remain) {
			smic_bootspi_control(obj, SMIC_BOOTSPI_SEC_ERASE, (void *)(sec_pos * SMIC_BOOTSPI_SEC_SIZE));
			for (i = 0; i < sec_remain; i++) {
				m_buf[sec_off + i] = val[i];
			}
			smic_bootspi_write_nocheck(obj, sec_pos * SMIC_BOOTSPI_SEC_SIZE, SMIC_BOOTSPI_SEC_SIZE, m_buf);
		} else {
			smic_bootspi_write_nocheck(obj, addr, sec_remain, val);
		}
		if (len == sec_remain) {
			break;
		} else {
			sec_pos++;
			sec_off = 0;
			val += sec_remain;
			addr += sec_remain;
			len -= sec_remain;
			if (len > SMIC_BOOTSPI_SEC_SIZE) {
				sec_remain = SMIC_BOOTSPI_SEC_SIZE;
			} else {
				sec_remain = len;
			}
		}
	}
	free(m_buf);

	return (int32_t)(size_orig);
error_exit:
	return ercd;
}

int32_t smic_bootspi_erase(SMIC_BOOTSPI_DEF_PTR obj, uint32_t addr, uint32_t len)
{
	int32_t ercd = E_OK;
	uint32_t i;
	uint32_t size_orig = len;
	uint16_t sec_pos;
	uint16_t sec_off;
	uint16_t sec_remain;
	uint8_t *m_buf;

	SMIC_BOOTSPI_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_BOOTSPI_CHECK_EXP(obj->bootspi_open_cnt != 0, E_OPNED);

	m_buf = malloc(SMIC_BOOTSPI_SEC_SIZE);

	SMIC_BOOTSPI_CHECK_EXP(m_buf != NULL, E_NOMEM);

	sec_pos = addr / SMIC_BOOTSPI_SEC_SIZE;
	sec_off = addr % SMIC_BOOTSPI_SEC_SIZE;
	sec_remain = SMIC_BOOTSPI_SEC_SIZE -sec_off;

	if (len <= sec_remain) {
		sec_remain = len;
	}

	while (1) {
		smic_bootspi_read(obj, sec_pos*SMIC_BOOTSPI_SEC_SIZE, SMIC_BOOTSPI_SEC_SIZE, m_buf);
		for (i = 0; i < sec_remain; i++) {
			if (m_buf[sec_off + i] != 0xFF) break;
		}
		if (i < sec_remain) {
			smic_bootspi_control(obj, SMIC_BOOTSPI_SEC_ERASE, (void *)(sec_pos*SMIC_BOOTSPI_SEC_SIZE));
			for (i = 0; i < sec_remain; i++) {
				m_buf[sec_off + i] = 0xFF;
			}
			smic_bootspi_write_nocheck(obj, sec_pos*SMIC_BOOTSPI_SEC_SIZE, SMIC_BOOTSPI_SEC_SIZE, m_buf);
		}
		if (len == sec_remain) {
			break;
		} else {
			sec_pos++;
			sec_off = 0;
			addr += sec_remain;
			len -= sec_remain;
			if (len > SMIC_BOOTSPI_SEC_SIZE) {
				sec_remain = SMIC_BOOTSPI_SEC_SIZE;
			} else {
				sec_remain = len;
			}
		}
	}
	free(m_buf);

	return (int32_t)(size_orig);
error_exit:
	return ercd;
}

int32_t smic_bootspi_control(SMIC_BOOTSPI_DEF_PTR obj, uint32_t ctrl_cmd, void *param)
{
	int32_t ercd = E_OK;
	uint32_t addr = 0;

	SMIC_BOOTSPI_CHECK_EXP(obj != NULL, E_OBJ);
	SMIC_BOOTSPI_CHECK_EXP(obj->bootspi_open_cnt != 0, E_OPNED);

	switch (ctrl_cmd) {
		case SMIC_BOOTSPI_RESET:
			bootspi_reset(obj);
			break;

		case SMIC_BOOTSPI_READ_ID:
			bootspi_read_id(obj, (uint8_t *)param, 3);
			break;

		case SMIC_BOOTSPI_CHIP_ERASE:
			bootspi_chip_erase(obj);
			break;

		case SMIC_BOOTSPI_BLK_ERASE:
			addr = (uint32_t)param;
			addr &= ~(SMIC_BOOTSPI_BLK_SIZE-1);
			bootspi_blk_erase(obj, addr);
			break;

		case SMIC_BOOTSPI_SEC_ERASE:
			addr = (uint32_t)param;
			addr &= ~(SMIC_BOOTSPI_SEC_SIZE-1);
			bootspi_sec_erase(obj, addr);
			break;

		default:
			ercd = E_PAR;
			break;
	}

error_exit:
	return ercd;
}
