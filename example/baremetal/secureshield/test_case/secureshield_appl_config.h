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

#ifndef _SECURESHIELD_APPL_CONFIG_H_
#define _SECURESHIELD_APPL_CONFIG_H_

/* tell linker script template the rom region information of containers */
// container1/2/3 will be allocated a (2048 bytes, only required in SecureShield V1) rom region.
// The .text and .rodata section in the object file (*container1/2/3.o) will be put into this ram region
// container4 has no ROM region. But the definition is required to generate the necessary symbols
#define SECURESHIELD_REGION_CONTAINERS_ROM \
 					GEN_CONTAINER_ROM_SECTION(container1, 2048, *container1.o) \
 					GEN_CONTAINER_ROM_SECTION(container2, 2048, *container2.o) \
 					GEN_CONTAINER_ROM_SECTION(container3, 2048, *container3.o) \
  					GEN_CONTAINER_ROM_SECTION(container4, 0)

// CONTAINER_ROM_OBJS_LIST is only for ARC_GNU
// This macro wii exclude the specified container objs from background container in linking,
// *(EXCLUDE_FILE(CONTAINER_ROM_OBJS_LIST) .text EXCLUDE_FILE(CONTAINER_ROM_OBJS_LIST)
// .text.* EXCLUDE_FILE(CONTAINER_ROM_OBJS_LIST) .gnu.linkonce.t.*)
#define CONTAINER_ROM_OBJS_LIST *container1.o *container2.o *container3.o

/* tell linker script template the ram region information of containers */
// container1/2/3 will be allocated a (2048 bytes, only required in SecureShield V1) ram region.
// The .data and .bss section in the object file (*container1/2/3.o) will be put into this ram region
// container4 will be allocated a (2048 bytes, only required in SecureShield V1) ram region. No obj files
// In SecureShield, the size(2048) will be ignored. The size of container ram region will be the actual used size.
#define SECURESHIELD_REGION_CONTAINERS_RAM \
 					GEN_CONTAINER_RAM_SECTION(container1, 2048, *container1.o) \
 					GEN_CONTAINER_RAM_SECTION(container2, 2048, *container2.o) \
 					GEN_CONTAINER_RAM_SECTION(container3, 2048, *container3.o) \
 					GEN_CONTAINER_RAM_SECTION(container4, 2048)


// CONTAINER_RAM_OBJS_LIST is only for ARC_GNU
// This macro wii exclude the specified container objs from background container in linking,
//*(EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .data EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST)
//.data.* EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .gnu.linkonce.d.*)
#define CONTAINER_RAM_OBJS_LIST *container1.o *container2.o *container3.o


#endif /* _SECURESHIELD_APPL_CONFIG_H_ */