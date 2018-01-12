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
 * \ingroup	EMBARC_APP_BAREMETAL_SECURESHIELD_SECRET_SECURE_SID
 * \brief	secureshield secret secure sid example container memory map information file
 */
#ifndef _SECURESHIELD_APPL_CONFIG_H_
#define _SECURESHIELD_APPL_CONFIG_H_

/*
 * This file is used to tell secureshield how to generate the linker file.
 * If a memory region is required to be created for a specific container. The macro
 * 	GEN_CONTAINER_RAM_SECTION(container_name/region_name, region_size, object files if required)
 * should be called. THe macro
 * 	SECURESHIELD_REGION_CONTAINERS_ROM is the collect of definitions to generate the normal container's
 * 	sections in normal ROM. For example
 *  #define SECURESHIELD_REGION_CONTAINERS_ROM  GEN_CONTAINER_ROM_SECTION(container1, 2048)\
 *  						GEN_CONTAINER_ROM_SECTION(container2, 2048) \
 *  						GEN_CONTAINER_ROM_SECTION(container3, 2048, test1.o, test2.o)
 *  means to create sections for container 1, container 2 and container 3, and their size is 2048 bytes.
 *  For container 3, the .text and .rodata of test1.o and test2.o will also be put into container 3's rom section.
 *  Because the limitation of MPU in Secureshield V1, the possible value of region_size must be 2048, 4096, 8192,
 *  16K, 32K, 64K ... etc..
 *  The macro SECURESHIELD_REGION_CONTAINERS_RAM works the same like SECURESHIELD_REGION_CONTAINERS_ROM, it has
 *  the definitions in normal RAM
 *
 *  THe macro SECURESHIELD_REGION_SECURE_CONTAINER_ROM and SECURESHIELD_REGION_SECURE_CONTAINER_RAM are used for
 *  secure ROM and secure RAM.
 */

/* tell linker script template, there is a region named container1 in secure rom, its size is 0 */
#define SECURESHIELD_REGION_SECURE_CONTAINERS_ROM GEN_SECURE_CONTAINER_ROM_SECTION(container1, 0)

// CONTAINER_ROM_OBJS_LIST is only for ARC_GNU
// This macro wii exclude the specified container objs from background container in linking,
// (EXCLUDE_FILE(CONTAINER_ROM_OBJS_LIST) .text EXCLUDE_FILE(CONTAINER_ROM_OBJS_LIST)
// .text. EXCLUDE_FILE(CONTAINER_ROM_OBJS_LIST) .gnu.linkonce.t.*)
#define CONTAINER_ROM_OBJS_LIST *container2.o

/* tell linker script template, there is a region named container1 in secure ram, its size is 2048 bytes */
#define SECURESHIELD_REGION_SECURE_CONTAINERS_RAM GEN_SECURE_CONTAINER_RAM_SECTION(container1, 2048)

// CONTAINER_RAM_OBJS_LIST is only for ARC_GNU
// This macro wii exclude the specified container objs from background container in linking,
//(EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .data EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST)
//.data. EXCLUDE_FILE(CONTAINER_RAM_OBJS_LIST) .gnu.linkonce.d.*)
#define CONTAINER_RAM_OBJS_LIST *container2.o

/* tell linker script template, there is a region named container2 in normal rom, its size is 0 bytes */
#define SECURESHIELD_REGION_CONTAINERS_ROM GEN_CONTAINER_ROM_SECTION(container2, 2048, *container2.o*)

/* tell linker script template, there is a region named container1 in normal ram, its size is 2048 bytes */
#define SECURESHIELD_REGION_CONTAINERS_RAM GEN_CONTAINER_RAM_SECTION(container2, 2048, *container2.o*)

#endif /* _SECURESHIELD_APPL_CONFIG_H_ */