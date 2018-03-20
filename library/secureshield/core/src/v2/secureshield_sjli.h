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
#ifndef _SECURESHIELD_SJLI_H_
#define _SECURESHIELD_SJLI_H_


typedef void (*SJLI_ENTRY) (void);

#ifndef NUM_SJLI_ENTRY
#define NUM_SJLI_ENTRY	10
#endif

#define SJLI_INSTRUCTION_LENGTH	4
#define SJLI_ENTRY_RESERVE_NO 3


/* .long is used here,  so old toolchain can also compile */
#define SJLI(u12) \
		Asm(".long %[val]\n" ::[val] "i" SJLI_OPCODE(u12))

#define SFLAG(u6) \
		Asm(".long %[val]\n" ::[val] "i" SFLAG_OPCODE(u6))



extern const SJLI_ENTRY secureshield_sjli_entry_table[NUM_SJLI_ENTRY];

extern uint32_t container_call_in(INT_EXC_FRAME *src_frame);
extern uint32_t container_call_out(PROCESSOR_FRAME *dst);
extern void sjli_init(void);
extern int32_t sjli_entry_install(uint32_t no, SJLI_ENTRY entry);
extern SJLI_ENTRY sjli_entry_get(uint32_t no);

#endif /* _SECURESHIELD_SJLI_H_ */