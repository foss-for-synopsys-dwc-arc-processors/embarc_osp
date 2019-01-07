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
#ifndef _TARGET_MEM_CONFIG_H_
#define _TARGET_MEM_CONFIG_H_

#include "arc_feature_config.h"

#ifdef USE_APPL_MEM_CONFIG
#include "appl_mem_config.h"
#endif

/**
 * The unit of XXXX_SIZE is Byte
 * For REGION_ROM, ICCM, EXT_ROM and EXT_RAM are available
 * For REGION_RAM, DCCM and EXT_RAM are available
 */
#ifdef ARC_FEATURE_ICCM_PRESENT
#ifndef ICCM_SIZE
#define ICCM_SIZE	ARC_FEATURE_ICCM_SIZE
#endif
#ifndef ICCM_START
#define ICCM_START	ARC_FEATURE_ICCM_BASE
#endif
#else
#ifndef ICCM_SIZE
#define ICCM_SIZE	0x20000
#endif
#ifndef ICCM_START
#define ICCM_START	0x60000000
#endif
#endif

#ifdef ARC_FEATURE_DCCM_PRESENT
#ifndef DCCM_SIZE
#define DCCM_SIZE	ARC_FEATURE_DCCM_SIZE
#endif
#ifndef DCCM_START
#define DCCM_START	ARC_FEATURE_DCCM_BASE
#endif
#else
#ifndef DCCM_SIZE
#define DCCM_SIZE	0x20000
#endif
#ifndef DCCM_START
#define DCCM_START	0x80000000
#endif
#endif

#ifdef ARC_FEATURE_XCCM_PRESENT
#ifndef REGION_XCCM_START
#define REGION_XCCM_START	ARC_FEATURE_XCCM_BASE
#endif
#ifndef REGION_XCCM_SIZE
#define REGION_XCCM_SIZE	ARC_FEATURE_XCCM_SIZE
#endif
#endif

#ifdef ARC_FEATURE_YCCM_PRESENT
#ifndef REGION_YCCM_START
#define REGION_YCCM_START	ARC_FEATURE_YCCM_BASE
#endif
#ifndef REGION_YCCM_SIZE
#define REGION_YCCM_SIZE	ARC_FEATURE_YCCM_SIZE
#endif
#endif

#ifndef EXT_ROM_START
#define EXT_ROM_START 	0x00000000
#endif

#ifndef EXT_ROM_SIZE
#define EXT_ROM_SIZE	0x40000
#endif

#ifndef EXT_RAM_START
#define EXT_RAM_START	0x10000000
#endif

#ifndef EXT_RAM_SIZE
#define EXT_RAM_SIZE	0x1000000
#endif

/**
 * The default regions assigned for application to use,
   by default, each region will use all the space
   of each memory device
 * User can config the start address and the size of
   the regions to limit the application using
 */
#ifndef REGION_ICCM_START
#define REGION_ICCM_START	ICCM_START
#define REGION_ICCM_SIZE	ICCM_SIZE
#endif

#ifndef REGION_DCCM_START
#define REGION_DCCM_START	DCCM_START
#define REGION_DCCM_SIZE	DCCM_SIZE
#endif

#ifndef REGION_EXT_RAM_START
#define REGION_EXT_RAM_START	EXT_RAM_START
#define REGION_EXT_RAM_SIZE	EXT_RAM_SIZE
#endif

#ifdef LIB_MCUBOOT
#error "EMSDP doesnot support MCUBOOT now"

#else /* !defined(LIB_MCUBOOT) */

#ifndef REGION_EXT_ROM_START
#define REGION_EXT_ROM_START	EXT_ROM_START
#define REGION_EXT_ROM_SIZE	EXT_ROM_SIZE
#endif

#define IMAGE_HEAD_SIZE 0x0
#endif /* LIB_MCUBOOT */


/**
 * The default regions used to generate link script
 * User can select region by configuring REGION_ROM and REGION_RAM
 * For REGION_ROM, REGION_ICCM, REGION_EXT_ROM are available
 * For REGION_RAM, REGION_DCCM and REGION_EXT_RAM are available
 */
#ifndef REGION_ROM
#ifdef ARC_FEATURE_ICACHE_PRESENT
#define REGION_ROM	REGION_EXT_RAM
#else
#define REGION_ROM	REGION_ICCM
#endif
#endif

#ifndef REGION_RAM
#ifdef ARC_FEATURE_DCACHE_PRESENT
#define REGION_RAM	REGION_EXT_RAM
#else
#define REGION_RAM	REGION_DCCM
#endif
#endif

#endif /* _TARGET_MEM_CONFIG_H_ */
