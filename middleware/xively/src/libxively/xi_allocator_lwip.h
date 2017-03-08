// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.
/*
 * Modified for port to ARC processor
 * by Huaqi Fang huaqi.fang@synopsys.com, Synopsys, Inc.
 */
#ifndef __XI_ALLOCATOR_LWIP_H__
#define __XI_ALLOCATOR_LWIP_H__

#ifndef MID_LWIP
#error "Middleware LWIP must include if using lwip memory management"
#else

#include "lwip/mem.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void* xi_alloc( size_t b )
{
    return ( void* ) mem_malloc( b );
}

static inline void xi_free( void* p )
{
    mem_free( p );
}

#ifdef __cplusplus
}
#endif

#endif /* MID_LWIP */

#endif // __XI_ALLOCATOR_LWIP_H__
