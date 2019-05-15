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
#ifndef _SMIC_EFLASH_H_
#define _SMIC_EFLASH_H_

#include "arc_exception.h"

#define EFLASH_CTRL_BASE 		0xF0009000
#define EFLASH_BASE_ADDR0		0x00000000
#define EFLASH_BASE_ADDR1		0x40000000

#define SMIC_EFLASH_SET_LOCK		0
#define SMIC_EFLASH_GET_LOCK		1
#define SMIC_EFLASH_PAGE_ERASE		2
#define SMIC_EFLASH_MACRO_ERASE		3
#define SMIC_EFLASH_GET_INFO		4

#define SMIC_EFLASH_PAGE_SIZE		512
#define SMIC_EFLASH_PAGE_CNT		512

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct smic_eflash_reg {
	//FMC Control Register
	uint32_t FMCCON;//(0x00)
	//FMC Address Register
	uint32_t FMCADR;//(0x04)
	//FMC Data Register
	uint32_t FMCDAT;//(0x08)
	//FMC command Register
	uint32_t FMCCMD;//(0x0C)
	//FMC Lock Register
	uint32_t FMCLOCK;//(0x10)
} SMIC_EFLASH_REG, *SMIC_EFLASH_REG_PTR;

typedef enum {
	FMC_LOCK	= 0,	// default
	FMC_UNLOCK	= 0x55AA6699
} E_FMC_LOCK;

typedef struct {
	SMIC_EFLASH_REG_PTR eflash_reg;
	uint8_t eflash_open_cnt;
	E_FMC_LOCK eflash_lock;
	uint16_t eflash_page_cnt;
	uint16_t eflash_page_size;
	uint32_t eflash_base_addr;
} SMIC_EFLASH_DEF, *SMIC_EFLASH_DEF_PTR;

typedef struct {
	uint16_t eflash_page_cnt;
	uint16_t eflash_page_size;
} SMIC_EFLASH_INFO;

#define EFLASH_DEFINE(NAME, REG_BASE) \
	SMIC_EFLASH_DEF __ ## NAME = { \
		.eflash_open_cnt = 0, \
		.eflash_reg = (SMIC_EFLASH_REG_PTR)REG_BASE, \
		.eflash_lock = FMC_LOCK, \
		.eflash_page_cnt = SMIC_EFLASH_PAGE_CNT, \
		.eflash_page_size = SMIC_EFLASH_PAGE_SIZE, \
		.eflash_base_addr = EFLASH_BASE_ADDR0 \
	}; \
	SMIC_EFLASH_DEF_PTR NAME = &__ ## NAME

extern int32_t smic_eflash_open(SMIC_EFLASH_DEF_PTR obj);
extern int32_t smic_eflash_close(SMIC_EFLASH_DEF_PTR obj);
extern int32_t smic_eflash_read(SMIC_EFLASH_DEF_PTR obj, uint32_t addr, uint32_t len, uint8_t *val);
extern int32_t smic_eflash_write(SMIC_EFLASH_DEF_PTR obj, uint32_t addr, uint32_t len, uint8_t *val);
extern int32_t smic_eflash_write_nocheck(SMIC_EFLASH_DEF_PTR obj, uint32_t addr, uint32_t len, uint8_t *val);
extern int32_t smic_eflash_control(SMIC_EFLASH_DEF_PTR obj, uint32_t ctrl_cmd, void *param);
extern int32_t smic_eflash_erase(SMIC_EFLASH_DEF_PTR obj, uint32_t addr, uint32_t len);


#ifdef __cplusplus
}
#endif

#endif /* _SMIC_EFLASH_H_ */
