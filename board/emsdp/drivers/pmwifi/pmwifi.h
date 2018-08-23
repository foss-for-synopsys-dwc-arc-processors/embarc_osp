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
#ifndef _RSWIFI_H_
#define _RSWIFI_H_

#if defined(MID_LWIP) && defined(MID_LWIP_CONTRIB)

#include "dev_wnic.h"

#define EMSDP_RSWIFI_0_ID		0

#define USE_EMSDP_RSWIFI_0		1

#define EMSDP_RSWIFI_0_MAC_ADDR0		(0x00)
#define EMSDP_RSWIFI_0_MAC_ADDR1		(0x1e)
#define EMSDP_RSWIFI_0_MAC_ADDR2		(0xc0)
#define EMSDP_RSWIFI_0_MAC_ADDR3		(0x0e)
#define EMSDP_RSWIFI_0_MAC_ADDR4		(0x71)
#define EMSDP_RSWIFI_0_MAC_ADDR5		(0xac)

#ifdef __cplusplus
extern "C" {
#endif

extern void rswifi_all_install(void);

#ifdef __cplusplus
}
#endif

#endif  /* MID_LWIP && MID_LWIP_CONTRIB */
#endif /* _RSWIFI_H_ */
