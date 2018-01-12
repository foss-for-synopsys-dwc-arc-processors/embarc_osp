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

#ifndef _SECURESHIELD_EXPORTS_H_
#define _SECURESHIELD_EXPORTS_H_

/* maximum number of containers: 1 is the minimum (unprivileged container) */
#define SECURESHIELD_MAX_CONTAINERS 5U

/* select an overloaded macro, so that 0 to 7 arguments can be used */
#define __SECURESHIELD_MACRO_SELECT(_0, _1, _2, _3, _4, _5, _6, _7, NAME, ...) NAME

/* count macro arguments */
#define SECURESHIELD_MACRO_NARGS(...) \
 	__SECURESHIELD_MACRO_SELECT(_0, ##__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1, 0)

/* declare explicit callee-saved registers to hold input arguments (0 to 7) */
/* note: sizeof(type) must be less than or equal to 4 */
#define SECURESHIELD_MACRO_REGS_ARGS(type, ...) \
	__SECURESHIELD_MACRO_SELECT(_0, ##__VA_ARGS__, \
				__SECURESHIELD_MACRO_REGS_ARGS7, \
				__SECURESHIELD_MACRO_REGS_ARGS6, \
				__SECURESHIELD_MACRO_REGS_ARGS5, \
				__SECURESHIELD_MACRO_REGS_ARGS4, \
				__SECURESHIELD_MACRO_REGS_ARGS3, \
				__SECURESHIELD_MACRO_REGS_ARGS2, \
				__SECURESHIELD_MACRO_REGS_ARGS1, \
				__SECURESHIELD_MACRO_REGS_ARGS0)(type, ##__VA_ARGS__)

#define __SECURESHIELD_MACRO_REGS_ARGS0(type)
#define __SECURESHIELD_MACRO_REGS_ARGS1(type, a0) \
		register type r0 asm("r0") = (type) a0;
#define __SECURESHIELD_MACRO_REGS_ARGS2(type, a0, a1) \
		register type r0 asm("r0") = (type) a0; \
		register type r1 asm("r1") = (type) a1;
#define __SECURESHIELD_MACRO_REGS_ARGS3(type, a0, a1, a2) \
		register type r0 asm("r0") = (type) a0; \
		register type r1 asm("r1") = (type) a1; \
		register type r2 asm("r2") = (type) a2;
#define __SECURESHIELD_MACRO_REGS_ARGS4(type, a0, a1, a2, a3) \
		register type r0 asm("r0") = (type) a0; \
		register type r1 asm("r1") = (type) a1; \
		register type r2 asm("r2") = (type) a2; \
		register type r3 asm("r3") = (type) a3;
#define __SECURESHIELD_MACRO_REGS_ARGS5(type, a0, a1, a2, a3, a4) \
		register type r0 asm("r0") = (type) a0; \
		register type r1 asm("r1") = (type) a1; \
		register type r2 asm("r2") = (type) a2; \
		register type r3 asm("r3") = (type) a3;	\
		register type r4 asm("r4") = (type) a4;
#define __SECURESHIELD_MACRO_REGS_ARGS6(type, a0, a1, a2, a3, a4, a5) \
		register type r0 asm("r0") = (type) a0; \
		register type r1 asm("r1") = (type) a1; \
		register type r2 asm("r2") = (type) a2; \
		register type r3 asm("r3") = (type) a3;	\
		register type r4 asm("r4") = (type) a4; \
		register type r5 asm("r5") = (type) a5;
#define __SECURESHIELD_MACRO_REGS_ARGS7(type, a0, a1, a2, a3, a4, a5, a6) \
		register type r0 asm("r0") = (type) a0; \
		register type r1 asm("r1") = (type) a1; \
		register type r2 asm("r2") = (type) a2; \
		register type r3 asm("r3") = (type) a3;	\
		register type r4 asm("r4") = (type) a4; \
		register type r5 asm("r5") = (type) a5;	\
		register type r6 asm("r6") = (type) a6;

/* declare explicit callee-saved registers to hold output values */
/* note: currently only one output value is allowed, up to 32 bits */
#define SECURESHIELD_MACRO_REGS_RETVAL(type, name) \
	register type name asm("r0");

/* declare callee-saved input/output operands for gcc-style inline asm
 * note: this macro requires that a C variable having the same name of the
 *       corresponding callee-saved register is declared;
 * note: for 0 inputs a dummy immediate is passed to avoid errors on a misplaced
 *       comma in the inline assembly
 */

#define SECURESHIELD_MACRO_ASM_INPUT(...) \
	 __SECURESHIELD_MACRO_SELECT(_0, ##__VA_ARGS__, \
	 			__SECURESHIELD_MACRO_ASM_INPUT7, \
				__SECURESHIELD_MACRO_ASM_INPUT6, \
				__SECURESHIELD_MACRO_ASM_INPUT5, \
				__SECURESHIELD_MACRO_ASM_INPUT4, \
				__SECURESHIELD_MACRO_ASM_INPUT3, \
				__SECURESHIELD_MACRO_ASM_INPUT2, \
				__SECURESHIELD_MACRO_ASM_INPUT1, \
				__SECURESHIELD_MACRO_ASM_INPUT0)(__VA_ARGS__)
#define __SECURESHIELD_MACRO_ASM_INPUT0()			[__dummy] "i" (0)
#define __SECURESHIELD_MACRO_ASM_INPUT1(a0)			[r0] "r" (r0)
#define __SECURESHIELD_MACRO_ASM_INPUT2(a0, a1)		[r0] "r" (r0), [r1] "r" (r1)
#define __SECURESHIELD_MACRO_ASM_INPUT3(a0, a1, a2)		\
				[r0] "r" (r0), [r1] "r" (r1), [r2] "r" (r2)
#define __SECURESHIELD_MACRO_ASM_INPUT4(a0, a1, a2, a3) \
				[r0] "r" (r0), [r1] "r" (r1), [r2] "r" (r2), [r3] "r" (r3)
#define __SECURESHIELD_MACRO_ASM_INPUT5(a0, a1, a2, a3, a4) \
				[r0] "r" (r0), [r1] "r" (r1), [r2] "r" (r2), [r3] "r" (r3), \
				[r4] "r" (r4)
#define __SECURESHIELD_MACRO_ASM_INPUT6(a0, a1, a2, a3, a4, a5) \
				[r0] "r" (r0), [r1] "r" (r1), [r2] "r" (r2), [r3] "r" (r3), \
				[r4] "r" (r4), [r5] "r" (r5)
#define __SECURESHIELD_MACRO_ASM_INPUT7(a0, a1, a2, a3, a4, a5, a6) \
				[r0] "r" (r0), [r1] "r" (r1), [r2] "r" (r2), [r3] "r" (r3), \
				[r4] "r" (r4), [r5] "r" (r5), [r6] "r" (r6)

#define SECURESHIELD_MACRO_ASM_OUTPUT(name) [res] "=r" (name)




#define SJLI_OPCODE(u12) 	(0x800028a0 | (((((u12 << 6) & 0xfc0) | ((u12 >> 6) & 0x3f)))  << 16))
#define SFLAG_OPCODE(u6)	(0x003f306f | (u6 << 22))


#endif/*__SECURESHIELD_EXPORTS_H__*/
