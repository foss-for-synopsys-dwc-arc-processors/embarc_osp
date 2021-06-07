/*
* Copyright 2019-2021, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

/**
 * @file MLI Library Configuration header
 *
 * @brief This header defines MLI Library configuration 
 */

#ifndef _MLI_CONFIG_H_
#define _MLI_CONFIG_H_
/**
* Define Library build configuration options
*/

/**
* Concatenate primitive: Maximum number of tensors that might be concatenated.
*/
#define MLI_CONCAT_MAX_TENSORS (8)

/**
* RNN cell primitive: Maximum number of input tensors.
*/
#define MLI_RNN_MAX_INPUT (4)

/**
* Library Debug mode
*/
#define     DBG_MODE_RELEASE   (0) /*< No debug. Messages:OFF; Assertions:OFF; ReturnCodes: Always OK */
#define     DBG_MODE_RET_CODES (1) /*< Return codes mode. Messages:OFF; Assertions:OFF; ReturnCodes: Valid Return*/
#define     DBG_MODE_ASSERT    (2) /*< Assert. Messages:OFF; Assertions:ON; Extra Assertions:OFF; ReturnCodes: Valid Return */
#define     DBG_MODE_DEBUG     (3) /*< Debug. Messages:ON; Assertions:ON; Extra Assertions:OFF; ReturnCodes: Valid Return */
#define     DBG_MODE_FULL      (4) /*< Full Debug. Messages:ON; Assertions:ON; Extra Assertions:ON; ReturnCodes: Valid Return */

#ifndef MLI_DEBUG_MODE
#define MLI_DEBUG_MODE (DBG_MODE_RELEASE)
#endif

/**
* Allow functions to pretty print their function name
*/
#ifndef MLI_DEBUG_ENABLE_STACK_TRACE_MESSAGES
#define MLI_DEBUG_ENABLE_STACK_TRACE_MESSAGES (0)
#endif

/**
* Define platform specific data
*/
#include <stdint.h>

#if defined (_ARC)

#include <arc/xy.h>

#ifdef __FXAPI__
#include <stdfix.h>
#endif

#define DO_PRAGMA(x) _Pragma(#x)
#define PRAGMA_CLANG(x) DO_PRAGMA(clang x)
#else
// Only support restrict on ARC platforms, but not on X86
#define __restrict
#define PRAGMA_CLANG(x)
#endif // if defined (_ARC)

#if defined(__clang__)
#define MLI_FORCE_INLINE inline __attribute__((always_inline))
#define MLI_NO_INLINE __attribute__((used)) __attribute__((noinline))
#define MLI_CODE_SECTION_START(x) code(x) 
#define MLI_CODE_SECTION_END()    code()

#elif defined(__GNUC__)
#define MLI_FORCE_INLINE inline __attribute__((always_inline))
#define MLI_NO_INLINE __attribute__((noinline))
#define __builtin_assume(x)
#define MLI_CODE_SECTION_START(x) section(x) // TODO: Need to verify
#define MLI_CODE_SECTION_END()    section()

#elif defined(_MSC_VER)
#define MLI_FORCE_INLINE inline __forceinline
#define MLI_NO_INLINE __declspec(noinline)
#define __builtin_assume(x) __assume(x)
#define __attribute__(x)
#define MLI_CODE_SECTION_START(x) code_seg(x)
#define MLI_CODE_SECTION_END()    code_seg()

#else
#error "Current compiler isn't supported"

#endif

/*
* Define the platform (according to pre-defined macro or according to HW config)
* 1 - ARCV2DSP ISA
* 2 - ARCV2DSP ISA with XY Memory
* 3 - ARCV2DSP ISA with 64bit operands (HS Only)
* 4 - ARCV2DSP ISA with vector DSP
*/

#if defined(V2DSP_VECTOR) || ((defined(__Xvdsp)))
#undef V2DSP_VECTOR
#define PLATFORM (4)
#define PLATFORM_STR  "ARCv2DSP VDSP"
#include "arc_vector.h"
#elif defined(V2DSP_XY) || ((defined __Xxy) && !(defined(V2DSP) || defined(V2DSP_WIDE)))
/* Platform with XY memory (EM9D or EM11D) */
#undef V2DSP_XY
#define PLATFORM (2)
#define PLATFORM_STR  "ARCv2DSP XY"

#elif defined(V2DSP_WIDE) || ((defined __Xdsp_wide) && !(defined(V2DSP) || defined(V2DSP_XY)))
/* Platform with wide DSP ISA (HS45D or HS47D) */
#undef V2DSP_WIDE
#define PLATFORM (3)
#define PLATFORM_STR  "ARCv2DSP Wide"

#elif defined(V2DSP) || ((defined(__Xdsp2) || defined(__Xdsp_complex)) && !(defined(V2DSP_XY) || defined(V2DSP_WIDE)))
/* Platform with DSP ISA (EM5D or EM7D) */
#undef V2DSP
#define PLATFORM (1)
#define PLATFORM_STR  "ARCv2DSP"

#elif (defined(__GNUC__) && !defined(__clang__)) || defined(_MSC_VER)
#undef X86_PLATFORM
#define PLATFORM (5)
#define PLATFORM_STR  "X86"

#else
#error "Target platform is undefined or defined incorrectly"

#endif

#define     V2DSP        (1)
#define     V2DSP_XY     (2)
#define     V2DSP_WIDE   (3)
#define     V2DSP_VECTOR (4)
#define     X86_PLATFORM (5)

/*
* Re-define ML pointers for XY specific platform
*
* MLI_PTR is used for all the read pointers
* MLI_CONV_OUT_PTR is used for the output buffers of all weigths based kernels.
* this means all the kernels that perform a convolution like operation between inputs and weights.
* MLI_OUT_PTR is used for the output of all other kernels.
*/
#if (PLATFORM == V2DSP_XY)
#define MLI_PTR(p) __xy p *
#define MLI_PTR_IS_XY true
#define MLI_OUT_PTR(p) __xy p *
#define MLI_OUT_PTR_IS_XY true
#define MLI_CONV_OUT_PTR(p) p *
#define MLI_CONV_OUT_PTR_IS_XY false
#define MLI_CCM_ATT 
#elif (PLATFORM == V2DSP_VECTOR) && !defined(MLI_BUILD_REFERENCE)
#define MLI_PTR(p) __vccm p *
#define MLI_PTR_IS_VCCM true
#define MLI_PTR_IS_XY false
#define MLI_OUT_PTR(p) __vccm p *
#define MLI_OUT_PTR_IS_XY false
#define MLI_CONV_OUT_PTR(p) __vccm p *
#define MLI_CONV_OUT_PTR_IS_XY false
#define MLI_CCM_ATT __vccm
#else
#define MLI_PTR(p) p *
#define MLI_PTR_IS_XY false
#define MLI_OUT_PTR(p) p *
#define MLI_OUT_PTR_IS_XY false
#define MLI_CONV_OUT_PTR(p) p *
#define MLI_CONV_OUT_PTR_IS_XY false
#define MLI_CCM_ATT
#endif

#endif // _MLI_CONFIG_H_
