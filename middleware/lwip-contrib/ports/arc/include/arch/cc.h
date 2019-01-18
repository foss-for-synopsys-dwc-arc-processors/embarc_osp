/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
/*
 * Modified for port to ARC processor
 * by Huaqi Fang huaqi.fang@synopsys.com, Synopsys, Inc.
 */

#ifndef _ARCH_CC_H_
#define _ARCH_CC_H_

#include <limits.h>
#include <string.h>
#include <stdlib.h> /* rand */
#include <stdint.h>
#include <sys/time.h>

// Common Defintions need by both toolchain
#define LWIP_TIMEVAL_PRIVATE 0
#ifndef LWIP_PROVIDE_ERRNO
#define LWIP_PROVIDE_ERRNO
#endif /* LWIP_PROVIDE_ERRNO */


// Extra definition needed by ARC GNU Toolchain
#ifdef __GNU__
/**
 * see sys/types.h
 * it defined FD_SET in gnu compiler
 * but here lwip source code sockets.h defined FD_SET
 */
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif /* _POSIX_SOURCE */

#endif /* __GNU__ */

// Extra defintion needed by Metaware Toolchain
#ifdef __MW__

#define SZT_F  "d"

#endif /* __MW__ */

/* Define platform endianness */
#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif /* BYTE_ORDER */

// No need to define this in LwIP 2.0
// Defined in arch.h, provided by <stdint.h>
// // basic types
// typedef unsigned   char    u8_t;
// typedef signed     char    s8_t;
// typedef unsigned   short   u16_t;
// typedef signed     short   s16_t;
// typedef unsigned   long    u32_t;
// typedef signed     long    s32_t;

// // addresses are 32-bits long
// typedef u32_t mem_ptr_t;

// No need to define this in LwIP 2.0
// Defined in arch.h, provided by <inttypes.h>
// printf format specifiers
// #define U16_F "u"
// #define S16_F "d"
// #define X16_F "x"
// #define U32_F "lu"
// #define S32_F "ld"
// #define X32_F "lx"

/* Compiler hints for packing structures */
#define PACK_STRUCT_FIELD(x) 	x
#define PACK_STRUCT_STRUCT	__attribute__((__packed__))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END


#ifdef LWIP_USE_BUILTIN_SWAPE
// byte-swapping
#include "arc_builtin.h"
#define LWIP_PLATFORM_BYTESWAP 1
#define LWIP_PLATFORM_HTONS(x) ((u16_t)_arc_swap16((s16_t)(x)))
#define LWIP_PLATFORM_HTONL(x) ((u32_t)_arc_swap32((s32_t)(x)))
#endif /* LWIP_USE_BUILTIN_SWAPE */

/**
 * \brief  	checksum algorithm
 * \details
 * 		LwIP gives you a choice of 3 algorithms:
 * 		- load byte by byte, construct 16 bits word and add: not efficient for most platforms
 *		- load first byte if odd address, loop processing 16 bits words, add last byte.
 *		- load first byte and word if not 4 byte aligned, loop processing 32 bits words, add last word/byte.
 */
#define LWIP_CHKSUM_ALGORITHM 2

#ifndef DEBUG_PRINT
	#if !defined(_HAVE_LIBGLOSS_) && defined(__GNU__) && defined(MID_COMMON)
		#include "xprintf.h"
		#define DEBUG_PRINT xprintf
 	#else
		#include <stdio.h>
		#define DEBUG_PRINT printf
	#endif
#endif /* DEBUG_PRINT */
#define LWIP_PLATFORM_DIAG(x)	do { DEBUG_PRINT x ; } while(0)


// assert output
#ifndef LWIP_NOASSERT
#include "embARC_assert.h"
#define LWIP_PLATFORM_ASSERT(x) do { DEBUG_PRINT("Assertion \"%s\" failed at line %d in %s\n", \
 										x, __LINE__, __FILE__); embARC_abort(); } while(0)
#else
#define LWIP_PLATFORM_ASSERT(x) do { DEBUG_PRINT("Assertion \"%s\" failed at line %d in %s\n", \
 										x, __LINE__, __FILE__); } while(0)
#endif /* LWIP_NOASSERT */

#define LWIP_RAND() ((u32_t)rand())

#endif // _ARCH_CC_H_
