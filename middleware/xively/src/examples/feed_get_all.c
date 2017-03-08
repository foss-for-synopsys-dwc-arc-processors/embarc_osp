// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.
/**
 * \brief   Example 007 that shows the basics of the xi synchronious interface
 * \file    main.c
 * \author  Olgierd Humenczuk
 *
 * The example that shows how to:
 *
 * a) initialize xi
 * b) get multiple datapoints ( feed )
 * c) gracefully close xi library
 *
 */

#include <xively.h>
#include <xi_helpers.h>

#include <time.h>
#include <stdio.h>
#include <string.h>

void print_usage()
{
    static const char usage[] = "This is feed_get_all of xi library\n"
    "to get all feeds values: \n"
    "example_07 api_key feed_id\n";

    printf( "%s", usage );
}

#define REQUIRED_ARGS 3

static inline void print_datapoint( const xi_datapoint_t* p )
{
    switch( p->value_type )
    {
        case XI_VALUE_TYPE_I32:
            printf( " value: %d@%ld\n", p->value.i32_value, p->timestamp.timestamp );
            break;
        case XI_VALUE_TYPE_F32:
            printf( " value: %f@%ld\n", p->value.f32_value, p->timestamp.timestamp );
            break;
        case XI_VALUE_TYPE_STR:
            printf( " value: %s@%ld\n", p->value.str_value, p->timestamp.timestamp );
            break;
        default:
            printf( "unknown value type\n" );
    }
}

int main( int argc, const char* argv[] )
{
#ifdef XI_NOB_ENABLED
    XI_UNUSED( argc );
    XI_UNUSED( argv );
#else
    if( argc < REQUIRED_ARGS )
    {
        print_usage();
        exit( 0 );
    }

    // create the xi library context
    xi_context_t* xi_context
        = xi_create_context(
                  XI_HTTP, argv[ 1 ]
                , atoi( argv[ 2 ] ) );

    // check if everything works
    if( xi_context == 0 )
    {
        return -1;
    }

    // create feed
    xi_feed_t f;
    memset( &f, 0, sizeof( xi_feed_t ) );

    xi_feed_get_all( xi_context, &f );

    printf( "\n" );

    for( size_t i = 0; i < f.datastream_count; ++i )
    {
        xi_datastream_t* d = &f.datastreams[ i ];
        printf( "datasream_id: %s ", d->datastream_id );

        for( size_t j = 0; j < d->datapoint_count; ++j )
        {
            xi_datapoint_t* p = &d->datapoints[ j ];
            print_datapoint( p );
        }
    }

    // destroy the context cause we don't need it anymore
    xi_delete_context( xi_context );
#endif
    return 0;
}
