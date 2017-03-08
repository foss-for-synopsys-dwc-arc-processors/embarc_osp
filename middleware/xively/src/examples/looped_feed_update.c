// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.
/**
 * \brief   Example 006 that shows the basics of the xi synchronious interface
 * \file    main.c
 * \author  Olgierd Humenczuk
 *
 * The example that shows how to:
 *
 * a) initialize xi
 * b) update multiple datapoints ( feed )
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
    static const char usage[] = "This is looped_feed_update of xi library\n"
    "to update multiple datastreams write: \n"
    "example_06 api_key feed_id datastream_id0 value0 datastream_id1 value1 ... datastream_idn valuen\n";

    printf( "%s", usage );
}

#define REQUIRED_ARGS 3

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

    // remember the count for pairs
    size_t pairs_count = ( argc - REQUIRED_ARGS ) / 2;

    // create feed
    xi_feed_t f;
    memset( &f, 0, sizeof( xi_feed_t ) );

    // set datastream count
    f.feed_id           = atoi( argv[ 2 ] );
    f.datastream_count  = pairs_count;

    // for each pair
    for( size_t i = 0; i < pairs_count; i++ )
    {
        // get the datastream pointer
        xi_datastream_t* d = &f.datastreams[ i ];

        // set the datapoint count
        d->datapoint_count   = 1;

        int size = sizeof( d->datastream_id );
        int s = xi_str_copy_untiln( d->datastream_id, size
            , argv[ REQUIRED_ARGS + ( i * 2 ) ], '\0' );

        if( s >= size )
        {
            printf( "datastream id too long: %s\n", argv[ REQUIRED_ARGS + ( i * 2 ) ] );
        }

        // get the datpoint counter
        xi_datapoint_t* p = &d->datapoints[ 0 ];

        // set the datapoint
        xi_set_value_i32( p, atoi( argv[ REQUIRED_ARGS + ( i  * 2 ) + 1 ] ) );
    }

    for( int i = 0; i < 20; ++i )
    {
        xi_feed_update( xi_context, &f );
    }

    // destroy the context cause we don't need it anymore
    xi_delete_context( xi_context );
#endif
    return 0;
}
