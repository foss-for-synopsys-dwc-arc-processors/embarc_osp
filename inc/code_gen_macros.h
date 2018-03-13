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
#ifndef _CODE_GEN_MACROS_H_
#define _CODE_GEN_MACROS_H_

/**
 * Macros for doing code-generation with the preprocessor.
 *
 * http://stackoverflow.com/a/12540675
 */
#define FIRST(a, ...) a
#define SECOND(a, b, ...) b

#define EMPTY()

#define EVAL(...)	EVAL1024(__VA_ARGS__)
#define EVAL1024(...)	EVAL512(EVAL512(__VA_ARGS__))
#define EVAL512(...)	EVAL256(EVAL256(__VA_ARGS__))
#define EVAL256(...)	EVAL128(EVAL128(__VA_ARGS__))
#define EVAL128(...)	EVAL64(EVAL64(__VA_ARGS__))
#define EVAL64(...)	EVAL32(EVAL32(__VA_ARGS__))
#define EVAL32(...)	EVAL16(EVAL16(__VA_ARGS__))
#define EVAL16(...)	EVAL8(EVAL8(__VA_ARGS__))
#define EVAL8(...)	EVAL4(EVAL4(__VA_ARGS__))
#define EVAL4(...)	EVAL2(EVAL2(__VA_ARGS__))
#define EVAL2(...)	EVAL1(EVAL1(__VA_ARGS__))
#define EVAL1(...)	__VA_ARGS__

#define DEFER1(m)	m EMPTY()
#define DEFER2(m)	m EMPTY EMPTY()()
#define DEFER3(m)	m EMPTY EMPTY EMPTY()()()
#define DEFER4(m)	m EMPTY EMPTY EMPTY EMPTY()()()()

#define IS_PROBE(...)	SECOND(__VA_ARGS__, 0)
#define PROBE()		~, 1

#define CAT(a,b)	a ## b

#define NOT(x)		IS_PROBE(CAT(_NOT_, x))
#define _NOT_0		PROBE()

#define BOOL(x)		NOT(NOT(x))

#define IF_ELSE(condition)	_IF_ELSE(BOOL(condition))
#define _IF_ELSE(condition)	CAT(_IF_, condition)

#define _IF_1(...)	__VA_ARGS__ _IF_1_ELSE
#define _IF_0(...)	_IF_0_ELSE

#define _IF_1_ELSE(...)
#define _IF_0_ELSE(...)	__VA_ARGS__

#define HAS_ARGS(...)	BOOL(FIRST(_END_OF_ARGUMENTS_ __VA_ARGS__)())
#define _END_OF_ARGUMENTS_()	0

#define MAP(m, first, ...)		\
	m(first)			\
	IF_ELSE(HAS_ARGS(__VA_ARGS__))(	\
	DEFER2(_MAP)()(m, __VA_ARGS__)	\
	)(				\
	/* Do nothing, just terminate */\
	)
#define _MAP()	MAP

#endif /* _CODE_GNE_MACROS_H_ */
