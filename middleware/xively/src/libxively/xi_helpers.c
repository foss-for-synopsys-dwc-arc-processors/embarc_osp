// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

// This file also containes code from MINIX C library, which is under MINIX license
// Copyright (c) 1987, 1997, 2006, Vrije Universiteit, Amsterdam, The Netherlands

#include <string.h>

#include "xi_debug.h"
#include "xi_helpers.h"
#include "xi_allocator.h"

char* xi_str_dup( const char* s )
{
    // PRECONDITIONS
    assert( s != 0 );

    size_t len = strlen( s );
    char * ret = xi_alloc( len + 1 );
    if( ret == 0 ) { return 0; }
    memcpy( ret, s, len + 1 );
    return ret;
}

int xi_str_copy_untiln( char* dst, size_t dst_size, const char* src, char delim )
{
    // PRECONDITIONS
    assert( dst != 0 );
    assert( dst_size > 1 );
    assert( src != 0 );

    size_t counter = 0;
    size_t real_size = dst_size - 1;

    while( *src != delim && counter < real_size && *src != '\0' )
    {
        *dst++ = *src++;
        counter++;
    }

    *dst = '\0';
    return counter;
}

char* xi_replace_with(
      char p, char r
    , char* buffer
    , size_t max_chars )
{
    char* c = buffer;

    while( *c != '\0' && max_chars-- )
    {
        if( *c == p ) { *c = r; }
        c++;
    }

    return c;
}
