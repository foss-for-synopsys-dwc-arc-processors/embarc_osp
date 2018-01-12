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

/* tell linker script template, there is a region named container1 in normal rom, its size is 2048 bytes 
 * container2 related definiion is a placeholder to generate necessary symbols */
#define SECURESHIELD_REGION_CONTAINERS_ROM \
 					GEN_CONTAINER_ROM_SECTION(container1, 2048)	\
 					GEN_CONTAINER_ROM_SECTION(container2, 0)

/* tell linker script template, there are regions named container1, container2 and container12 in normal 
 * ram who sizes are 2048 bytes */
#define SECURESHIELD_REGION_CONTAINERS_RAM \
 					GEN_CONTAINER_RAM_SECTION(container1, 2048) \
 					GEN_CONTAINER_RAM_SECTION(container2, 2048) \
 					GEN_CONTAINER_RAM_SECTION(container12_shared, 2048)

#endif /* _SECURESHIELD_APPL_CONFIG_H_ */