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
#ifndef _SMIC_BOOTSPI_H_
#define _SMIC_BOOTSPI_H_

#include "arc_exception.h"
#define BOOTSPI_CTRL_BASE 		0xF0003000

#define SMIC_BOOTSPI_RESET			0
#define SMIC_BOOTSPI_READ_ID		1
#define SMIC_BOOTSPI_CHIP_ERASE		2
#define SMIC_BOOTSPI_BLK_ERASE		3
#define SMIC_BOOTSPI_SEC_ERASE		4

#define SMIC_BOOTSPI_PAGE_SIZE		0x100		//256 bytes
#define SMIC_BOOTSPI_SEC_SIZE		0x1000		//4K  bytes
#define SMIC_BOOTSPI_BLK_SIZE		0x10000		//64K bytes

#define SMIC_BOOTSPI_BLKS_PER_CHIP 	32
#define SMIC_BOOTSPI_SECS_PER_CHIP 	512

//#define SMIC_BOOTSPI_PAGES_PER_SEC	(SMIC_BOOTSPI_SECTOR_SIZE/SMIC_BOOTSPI_PAGE_SIZE)
//#define SMIC_BOOTSPI_SECS_PER_BLK	(SMIC_BOOTSPI_BLK_SIZE/SMIC_BOOTSPI_SEC_SIZE)


#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct smic_bootspi_reg {
	//Manual Mode Data Read Register
	uint32_t MMDR;//(0x00)
	//Manual Mode Data Write Register
	uint32_t MMDW;//(0x04)
	//Manual Mode Control Register
	uint32_t MMC;//(0x08)
	//Manual Mode Status Register
	uint32_t MMS;//(0x0C)
	//Manual Mode Enable Register
	uint32_t MME;//(0x10)
} SMIC_BOOTSPI_REG, *SMIC_BOOTSPI_REG_PTR;

typedef struct {
	SMIC_BOOTSPI_REG_PTR bootspi_reg;
	uint8_t bootspi_open_cnt;
} SMIC_BOOTSPI_DEF, *SMIC_BOOTSPI_DEF_PTR;

#define BOOTSPI_DEFINE(NAME, REG_BASE) \
	SMIC_BOOTSPI_DEF __ ## NAME = { \
		.bootspi_open_cnt = 0, \
		.bootspi_reg = (SMIC_BOOTSPI_REG_PTR)REG_BASE, \
	}; \
	SMIC_BOOTSPI_DEF_PTR NAME = &__ ## NAME

extern int32_t smic_bootspi_open(SMIC_BOOTSPI_DEF_PTR obj);
extern int32_t smic_bootspi_close(SMIC_BOOTSPI_DEF_PTR obj);
extern int32_t smic_bootspi_read(SMIC_BOOTSPI_DEF_PTR obj, uint32_t addr, uint32_t len, uint8_t *val);
extern int32_t smic_bootspi_write(SMIC_BOOTSPI_DEF_PTR obj, uint32_t addr, uint32_t len, uint8_t *val);
extern int32_t smic_bootspi_write_nocheck(SMIC_BOOTSPI_DEF_PTR obj, uint32_t addr, uint32_t len, uint8_t *val);
extern int32_t smic_bootspi_control(SMIC_BOOTSPI_DEF_PTR obj, uint32_t ctrl_cmd, void *param);
extern int32_t smic_bootspi_erase(SMIC_BOOTSPI_DEF_PTR obj, uint32_t addr, uint32_t len);
#ifdef __cplusplus
}
#endif

#endif /* _SMIC_BOOTSPI_H_ */