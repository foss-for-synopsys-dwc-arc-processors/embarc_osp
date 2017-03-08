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
 * \version 2017.03
 * \date 2016-02-03
 * \author Wayne Ren(Wei.Ren@synopsys.com)
--------------------------------------------- */
#define __ASSEMBLY__
#include "secureshield_config.h"
#include "secureshield_secure_call_exports.h"

.globl secureshield_start
.globl secureshield_config
.weak  __secureshield_mode

	.file 	"wrapper.s"

	.section .rom.secureshield.secure, "ax"
secureshield_start:
	.incbin SECURESHIELD_SECURE_BIN
secureshield_config:
	/* secureshield expects its configuration section right
	   after itself in secure rom. If the configuration magic is not found,
	   secureshield will intentionally freeze to avoid accidentally
	   unprotected systems */
	.long  SECURESHIELD_MAGIC

	/* secureshield mode
	   0: disabled
	   1: enabled
	 */
	.long __secureshield_mode

	/* start and end address of protected bss */
	.long _f_rom_secureshieldruntime
	.long _e_rom_secureshieldruntime

	.long _f_ram_secureshieldruntime
	.long _e_ram_secureshieldruntime


	.long _f_text_background_container
	.long _e_rodata_background_container

	.long _f_data_background_container
	.long _e_bss_background_container

	 /* start and end address of containers configuration tables */
	.long __secureshield_cfgtbl_start
	.long __secureshield_cfgtbl_end

	/* start and end address of list of pointers to containers configuration tables */
	.long __secureshield_cfgtbl_ptr_start
	.long __secureshield_cfgtbl_ptr_end

__secureshield_mode:
	/* secureshield default mode - user can override weak reference */
	.long 0

.section .ram.secureshield.secure, "aw", @nobits
	.space SECURESHIELD_SECURE_RAM_USAGE