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

/**
 * \file
 * \ingroup	CORE_ERROR
 * \brief header files wrapper
 */
#ifndef _EMBARC_H_
#define _EMBARC_H_

/* toolchain definition */
#include "embARC_toolchain.h"

/* error code related definitions */
#include "embARC_error.h"

/* ARC HAL */
#include "arc/arc_builtin.h"
#include "arc/arc_timer.h"
#include "arc/arc_exception.h"
#if ARC_FEATURE_ICACHE_PRESENT || ARC_FEATURE_DCACHE_PRESENT
#include "arc/arc_cache.h"
#endif
#if ARC_FEATURE_DMAC_PRESENT
#include "arc/arc_udma.h"
#endif
#if ARC_FEATURE_MPU_PRESENT
#include "arc/arc_mpu.h"
#endif
#if ARC_FEATURE_CONNECT_PRESENT
#include "arc/arc_connect.h"
#endif
#if ARC_FEATURE_MP_NUM_CPUS > 1
#include "arc/arc_mp.h"
#endif
#if ARC_FEATURE_XY_MEM
#include "arc/arc_xy_agu.h"
#endif

#include "board.h"

#ifdef ENABLE_OS
#include "os_hal_inc.h"
#endif

#ifdef MID_COMMON
#include "console_io.h"
#include "xprintf.h"
#endif

/* FatFS header files */
#ifdef MID_FATFS
#include "ff.h"
#include "diskio.h"
#endif

/* ntshell header files */
#ifdef MID_NTSHELL
#include "ntshell_task.h"
#endif

/* \todo: lwIP header files */
#ifdef MID_LWIP
#endif

#ifdef LIB_CLIB
#include "embARC_syscalls.h"
#endif

/* LIB_SECURESHIELD should be at the end */
#ifdef LIB_SECURESHIELD
#include "secureshield_lib.h"
#endif

#endif  /* _EMBARC_H_ */
