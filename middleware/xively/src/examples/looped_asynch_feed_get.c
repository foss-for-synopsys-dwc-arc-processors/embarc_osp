// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.
/**
 * \brief   Example 004 that shows the basics of the xi synchronious interface
 * \file    main.c
 * \author  Olgierd Humenczuk
 *
 * The example that shows how to:
 *
 * a) initialize xi
 * b) get the datastream
 * c) gracefully close xi library
 *
 */

#include <xively.h>
#include <xi_helpers.h>

#include <time.h>
#include <stdio.h>
#include <sys/select.h>
#include <stdbool.h>

#include "io/posix_asynch/posix_asynch_data.h"
#include "nob/nob_runner.h"

void print_usage()
{

#ifdef XI_NOB_ENABLED
    static const char usage[] = "This is looped asynch get of xi library\n"
    "to constantly view your datastream write: \n"
    "looped_asynch_feed_get api_key feed_id\n";
#else
    static const char usage[] = "Please recompile libxiveley with XI_NOB_ENABLED=true and one of the asynch io layers in order to use that example";
#endif

    printf( "%s", usage );
}

int main( int argc, const char* argv[] )
{

    XI_UNUSED( argc );
    XI_UNUSED( argv );

#ifdef XI_NOB_ENABLED
    if( argc < 4 )
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

    xi_feed_t ret;

restart:
    memset( &ret, 0, sizeof( xi_feed_t ) );
    xi_nob_feed_get_all( xi_context, &ret );

    posix_asynch_data_t* posix_data
            = ( posix_asynch_data_t* ) xi_context->layer_chain.bottom->user_data;

    fd_set rfds;
    struct timeval tv;
    int retval = 0, i;
    bool sent = false;

    /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO( &rfds );
    FD_SET( posix_data->socket_fd, &rfds );


    while( 1 )
    {
        /* Wait up to five seconds. */
        tv.tv_sec = 15;
        tv.tv_usec = 0;

        if ( !sent )
        {
          retval = select( posix_data->socket_fd + 1, NULL, &rfds, NULL, &tv );
        }

        if ( sent )
        {
          retval = select( posix_data->socket_fd + 1, &rfds, NULL, NULL, &tv );
        }
        if ( retval == -1 )
        {
            perror("error in select()");
        }
        else if ( retval )
        {

            i = process_xively_nob_step( xi_context );
            switch( i ) {
              case LAYER_STATE_OK:
                if( sent == true )
                {
                    CALL_ON_SELF_ON_CLOSE( xi_context->layer_chain.bottom );
                    goto print_data;
                }
                sent = true;
                break;
              case LAYER_STATE_ERROR:
                printf("error in send\r\n");
                while(1){};
            }
        }
        else
        {
            printf( "No data within five seconds.\n" );
            exit( 0 );
        }
    }

print_data:

    for( size_t i = 0; i < ret.datastream_count; ++i )
    {
        printf( "timestamp = %ld.%ld, value = %s\n"
            , ret.datastreams[ i ].datapoints[ 0 ].timestamp.timestamp, ret.datastreams[ i ].datapoints[ 0 ].timestamp.micro
            , ret.datastreams[ i ].datapoints[ 0 ].value.str_value );
    }

    goto restart;


    // destroy the context cause we don't need it anymore
    xi_delete_context( xi_context );
#else
    print_usage();
#endif

    return 0;
}
