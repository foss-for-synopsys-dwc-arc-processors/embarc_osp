/* ------------------------------------------
 * Copyright (c) 2019, Synopsys, Inc. All rights reserved.

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
#define __ASSEMBLY__
#include "arc.h"

	.file "arc_embarc_mli_startup.s"
	.section .init_bootstrap, "ax"
	.global _embarc_mli_startup
	.align 4
_embarc_mli_startup:
_mli_copy_rom:
	mov	r0, _f_mli_rom
	mov	r1, _load_addr_mli_rom
	breq	r0, r1, _mli_copy_data
	mov	r3, _e_mli_rom
_mli_copy_rom_loop:
	ld.ab	r2, [r1, 4]
	st.ab	r2, [r0, 4]
	brlt	r0, r3, _mli_copy_rom_loop
_mli_copy_data:
	mov	r0, _f_mli_data
	mov	r1, _load_addr_mli_data
	breq	r0, r1, _mli_copy_model
	mov	r3, _e_mli_data
_mli_copy_data_loop:
	ld.ab	r2, [r1, 4]
	st.ab	r2, [r0, 4]
	brlt	r0, r3, _mli_copy_data_loop
_mli_copy_model:
	mov	r0, _f_mli_model
	mov	r1, _load_addr_mli_model
	breq	r0, r1, _mli_copy_model_p2
	mov	r3, _e_mli_model
_mli_copy_model_loop:
	ld.ab	r2, [r1, 4]
	st.ab	r2, [r0, 4]
	brlt	r0, r3, _mli_copy_model_loop
_mli_copy_model_p2:
	mov	r0, _f_mli_model_p2
	mov	r1, _load_addr_mli_model_p2
	breq	r0, r1, _mli_copy_Zdata
	mov	r3, _e_mli_model_p2
_mli_copy_model_p2_loop:
	ld.ab	r2, [r1, 4]
	st.ab	r2, [r0, 4]
	brlt	r0, r3, _mli_copy_model_p2_loop
_mli_copy_Zdata:
	mov	r0, _f_Zdata
	mov	r1, _load_addr_Zdata
	breq	r0, r1, _mli_zero_ram
	mov	r3, _e_Zdata
_mli_copy_Zdata_loop:
	ld.ab	r2, [r1, 4]
	st.ab	r2, [r0, 4]
	brlt	r0, r3, _mli_copy_Zdata_loop
_mli_zero_ram:
	j	[blink]

/* required by libmil.a */
	.text
	.global _init_ad
	.align 4
_init_ad:
	lr	r0, [AUX_STATUS32]
	bset	r0, r0, 19
	flag	r0
	j [blink]
