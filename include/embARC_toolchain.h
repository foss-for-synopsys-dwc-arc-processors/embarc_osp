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
 * \ingroup	TOOLCHAIN
 * \brief toolchain dependent definitions
 */

/**
 * \addtogroup TOOLCHAIN
 * @{
 */

#ifndef _EMBARC_TOOLCHAIN_H_
#define _EMBARC_TOOLCHAIN_H_

#ifndef __ASSEMBLY__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>     /* C99 standard lib */
#include <limits.h>     /* C99 standard lib */
#include <stddef.h>     /* C99 standard lib */
#include <stdbool.h>    /* C99 standard lib */

/*
 *  macro definitions of compiler extend function
 */
#ifndef __cplusplus                             /* C++ supports inline */
#if __STDC_VERSION__ < 199901L                  /* C99 supports inline */
#define inline  __inline__                      /* inline function */
#endif /* __STDC_VERSION__ < 199901L */
#endif /* __cplusplus */

#define Inline  static __inline__               /* inline function */

#define Asm     __asm__ volatile                /* inline asm (no optimization) */

/* compiler attributes */
#define EMBARC_FORCEINLINE      __attribute__((always_inline))
#define EMBARC_NOINLINE         __attribute__((noinline))
#define EMBARC_PACKED           __attribute__((packed))
#define EMBARC_WEAK             __attribute__((weak))
#define EMBARC_ALIAS(f)         __attribute__((weak, alias(#f)))
#define EMBARC_LINKTO(f)        __attribute__((alias(#f)))
#define EMBARC_NORETURN         __attribute__((noreturn))
#define EMBARC_NAKED            __attribute__((naked))  /* function without return */
#define EMBARC_ALIGNED(x)       __attribute__((aligned(x)))

/* array count macro */
#define EMBARC_ARRAY_COUNT(x) (sizeof(x) / sizeof(x[0]))

/* convert macro argument to string */
/* note: this needs one level of indirection, accomplished with the helper macro
 *       __EMBARC_TO_STRING */
#define __EMBARC_TO_STRING(x) #x
#define EMBARC_TO_STRING(x)   __EMBARC_TO_STRING(x)
#define EMBARC_CLZ(x)           (x == 0 ? 32 : __builtin_clz(x))
#define EMBARC_BITS(x)          (32 - EMBARC_CLZ(x))
/* x must be > 0 */
#define EMBARC_POW2_CEIL(x) ((1 << (31 - EMBARC_CLZ(x))) < x ? \
			     1 << (31 - EMBARC_CLZ(x) + 1) :   \
			     1 << (31 - EMBARC_CLZ(x)))

#define DIV_ROUND_UP(x, y)  (((x) + (y) - 1) / (y))

#if defined(__GNU__)
/* GNU tool specific definitions */

/**
 * @brief call c++ class constructors
 *
 */
extern void arc_gnu_do_global_ctors_aux(void);

/**
 * @brief call c++ initialization arrary
 *
 */
extern void arc_gnu_do_init_array_aux(void);

/**
 * @brief call c++ class deconstructors
 *
 */
extern void arc_gnu_do_global_dtors_aux(void);

#elif defined(__MW__)
/* Metaware tool specific definitions */
/* Metaware toolchain related definitions */
/**
 * @brief call c++ related initialization
 *
 */
extern void arc_mwdt_init(void);

/**
 * @brief call c++ related finish processing
 *
 */
extern void arc_mwdt_fini(void);

#else
#error "unsupported toolchain"
#endif

#ifdef __cplusplus
}
#endif
#endif  /* __ASSEMBLY__ */

#endif  /* _EMBARC_TOOLCHAIN_H_ */
/** @} */
