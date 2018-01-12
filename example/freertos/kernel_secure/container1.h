/*------------------------------------------
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
 * \ingroup	EMBARC_APP_FREERTOS_KERNEL_SECURE
 * \brief	freertos kernel secure example container1 head file
 */
#ifndef CONTAINER1_H
#define CONTAINER1_H

// memory region for secret data shared between container 1 and 2
#define CONTAINER_12_SHARED_ORIGIN	((uint32_t)_f_data_container12_shared)
#define CONTAINER_12_SHARED_LENGTH	2048

#define SECRET_LEN	64
#define PWD_LEN		6

#define GET_SECRET	1
#define SET_PWD		2
#define SET_SECRET	3

typedef struct {
	uint8_t secret[SECRET_LEN+1];
	uint8_t pwd[PWD_LEN+1];
	uint8_t initialized;
	uint8_t fails;
} SECRET_CONTEXT;


extern uint8_t _f_data_container12_shared[];

extern int32_t init_secret(void);
extern int32_t operate_secret(char* pwd, uint32_t cmd, char * data);
#endif
