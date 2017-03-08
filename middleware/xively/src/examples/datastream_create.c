// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.
/**
 * \brief   Example 002 that shows the basics of the xi synchronious interface
 * \file    main.c
 * \author  Olgierd Humenczuk
 *
 * The example that shows how to:
 *
 * a) initialize xi
 * b) create the datapoint
 * c) gracefully close xi library
 *
 */

#include <xively.h>
#include <xi_helpers.h>

#include <time.h>
#include <stdio.h>

void print_usage()
{
    static const char usage[] = "This is example_02 of xi library\n"
    "to create your datastream write: \n"
    "example_02 api_key feed_id datastream_id value\n";

    printf( "%s", usage );
}

int main( int argc, const char* argv[] )
{
#ifdef XI_NOB_ENABLED
    XI_UNUSED( argc );
    XI_UNUSED( argv );
#else
    if( argc < 5 )
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

    xi_datapoint_t datapoint;
    xi_set_value_i32( &datapoint, atoi( argv[ 4 ] ) );

    { // get actual timestamp
        time_t timer = 0;
        time( &timer );
        datapoint.timestamp.timestamp = timer;
    }

    xi_datastream_create( xi_context
            , atoi( argv[ 2 ] ), argv[ 3 ]
            , &datapoint );

    // destroy the context cause we don't need it anymore
    xi_delete_context( xi_context );
#endif

    return 0;
}
