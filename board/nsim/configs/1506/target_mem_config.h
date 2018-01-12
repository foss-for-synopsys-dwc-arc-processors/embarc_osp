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

#ifndef _TARGET_MEM_CONFIG_H_
#define _TARGET_MEM_CONFIG_H_

#ifdef USE_APPL_MEM_CONFIG
#include "appl_mem_config.h"
#endif

#ifdef CORE_MEM_CONFIG_FILE
#include CORE_MEM_CONFIG_FILE

#else /* CORE_MEM_CONFIG_FILE not defined case */
#warning "CORE_MEM_CONFIG_FILE is not defined, use default core memory configurations."

#ifndef ICCM_SIZE
#define ICCM_SIZE	0x80000
#endif

#ifndef DCCM_SIZE
#define DCCM_SIZE	0x80000
#endif

#ifndef ICCM_START
#define ICCM_START	0x0
#endif

#ifndef DCCM_START
#define DCCM_START	0x80000000
#endif

#ifndef REGION_ROM
#define REGION_ROM	ICCM
#endif

#ifndef REGION_RAM
#define REGION_RAM	DCCM
#endif
#endif

#endif /* _TARGET_MEM_CONFIG_H_ */
