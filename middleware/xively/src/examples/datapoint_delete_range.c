// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.
/**
 * \brief   Example 008 that shows the basics of the xi synchronious interface
 * \file    main.c
 * \author  Olgierd Humenczuk
 *
 * The example that shows how to:
 *
 * a) initialize xi
 * b) delete range of datapoints
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
    static const char usage[] = "This is example_08 of xi library\n"
    "to delete range of datapoints: \n"
    "example_08 api_key feed_id datastream_id timestamp_start timestamp_end\n";

    printf( "%s", usage );
}

#define REQUIRED_ARGS 6

int main( int argc, const char* argv[] )
{
#ifdef XI_NOB_ENABLED
    XI_UNUSED( argc );
    XI_UNUSED( argv );
#else
    if( argc != REQUIRED_ARGS )
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
    xi_timestamp_t start;
    xi_timestamp_t end;

    memset( &start, 0, sizeof( xi_timestamp_t ) );
    memset( &end, 0, sizeof( xi_timestamp_t ) );

    start.timestamp = atoi( argv[ 4 ] );
    end.timestamp   = atoi( argv[ 5 ] );

    xi_datapoint_delete_range( xi_context, xi_context->feed_id
        , argv[ 3 ], &start, &end );

    // destroy the context cause we don't need it anymore
    xi_delete_context( xi_context );
#endif
    return 0;
}
