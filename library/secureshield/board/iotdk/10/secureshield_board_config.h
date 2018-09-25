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
#ifndef _SECURESHIELD_BOARD_CONFIG_H_
#define _SECURESHIELD_BOARD_CONFIG_H_

#include "container_config.h"

#include "secureshield_appl_config.h"

#ifndef CONTAINER_ADDRESS_ALIGNMENT
#define CONTAINER_ADDRESS_ALIGNMENT	2048
#endif

/* rom area for normal world */
#ifndef NORMAL_ROM_START
#define NORMAL_ROM_START 0x20000000
#endif

#ifndef NORMAL_ROM_SIZE
#define NORMAL_ROM_SIZE 0x20000
#endif

/* rom area for secure world */
#ifndef SECURE_ROM_START
#define SECURE_ROM_START 0x20020000
#endif

#ifndef SECURE_ROM_SIZE
#define SECURE_ROM_SIZE 0x20000
#endif

/* ram area for normal world */
#ifndef NORMAL_RAM_START
#define NORMAL_RAM_START 0x80000000
#endif

#ifndef NORMAL_RAM_SIZE
#define NORMAL_RAM_SIZE 0x10000
#endif

/* ram area for secure world */
#ifndef SECURE_RAM_START
#define	SECURE_RAM_START 0x80010000
#endif

#ifndef SECURE_RAM_SIZE
#define SECURE_RAM_SIZE 0x10000
#endif


/* secure ram reserved for secure bin file */
#ifndef SECURESHIELD_SECURE_RAM_USAGE
#define SECURESHIELD_SECURE_RAM_USAGE	(SECURE_HEAPSIZE+SECURE_STACKSIZE+10240)
#endif


#endif /* _SECURESHIELD_BOARD_CONFIG_H_ */
