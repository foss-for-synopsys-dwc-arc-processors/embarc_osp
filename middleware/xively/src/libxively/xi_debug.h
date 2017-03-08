// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.
/*
 * Modified for port to ARC processor
 * by Huaqi Fang huaqi.fang@synopsys.com, Synopsys, Inc.
 */

/**
 * \file    xi_debug.h
 * \author  Olgierd Humenczuk
 * \brief   Macros to use for debugging
 */

#ifndef __XI_DEBUG_H__
#define __XI_DEBUG_H__

#include "xi_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef XI_DEBUG_PRINTF
    #include <stdio.h>
    #define __xi_printf(...) printf(__VA_ARGS__)
#else
    #define __xi_printf(...) XI_DEBUG_PRINTF(__VA_ARGS__)
#endif

#if XI_DEBUG_OUTPUT
    #define xi_debug_logger(msg) \
          __xi_printf( "[%s:%d (%s)] %s\r\n", __FILE__, __LINE__, __func__, msg )
    #define xi_debug_format(fmt, ...) \
          __xi_printf( "[%s:%d (%s)] "fmt"\r\n", __FILE__, __LINE__, __func__, __VA_ARGS__ )
    #define xi_debug_printf(...) \
          __xi_printf( __VA_ARGS__ )
    #define xi_debug_function_entered() \
          __xi_printf( "[%s:%d (%s)] -> entered\r\n", __FILE__, __LINE__, __func__ )
#else
    #define xi_debug_logger(...)
    #define xi_debug_format(...)
    #define xi_debug_printf(...)
    #define xi_debug_function_entered()
#endif

#if XI_DEBUG_ASSERT
    #ifdef NDEBUG
        #undef NDEBUG
    #endif
    #ifdef __GNU__
        #include <assert.h>
    #else
        #include "embARC_assert.h"
        #define assert(E) 	((E)? (void)0 : embARC_assert(#E, __FILE__, __LINE__))
    #endif
#else
    /* The actual header is missing in some toolchains, so we wrap it here. */
    #define assert(e) ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif // __XI_DEBUG_H__
