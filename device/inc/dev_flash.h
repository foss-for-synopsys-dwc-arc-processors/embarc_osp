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
#ifndef _DEV_FLASH_H_
#define _DEV_FLASH_H_

#include "dev_common.h"

#define DEV_SET_FLASH_SYSCMD(cmd)	DEV_SET_SYSCMD((cmd))

#define FLASH_CMD_GET_INFO		DEV_SET_FLASH_SYSCMD(0)

#define FLASH_CMD_PAGE_ERASE		DEV_SET_FLASH_SYSCMD(1)

#define FLASH_CMD_CHIP_ERASE		DEV_SET_FLASH_SYSCMD(2)


typedef struct dev_flash_info {
	uint32_t begin_addr;
	uint32_t total_size;
	uint32_t align_size;
	uint32_t page_size;
	uint32_t page_cnt;
	uint32_t open_cnt;
} DEV_FLASH_INFO, *DEV_FLASH_INFO_PTR;

typedef struct dev_flash {
	DEV_FLASH_INFO flash_info;
	int32_t (*flash_open) (void);
	int32_t (*flash_close) (void);
	int32_t (*flash_control) (uint32_t ctrl_cmd, void *param);
	int32_t (*flash_write) (uint32_t addr, void *src, uint32_t len);
	int32_t (*flash_write_nocheck) (uint32_t addr, void *src, uint32_t len);
	int32_t (*flash_read) (uint32_t addr, void *dst, uint32_t len);
	int32_t (*flash_erase) (uint32_t addr, uint32_t len);
} DEV_FLASH, *DEV_FLASH_PTR;


#ifdef __cplusplus
extern "C" {
#endif

extern DEV_FLASH_PTR flash_get_dev(int32_t flash_id);

#ifdef __cplusplus
}
#endif

#endif /* _DEV_PINMUX_H_ */