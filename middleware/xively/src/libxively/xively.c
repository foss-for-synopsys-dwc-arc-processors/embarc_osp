// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.
/*
 * Modified for port to ARC processor
 * by Huaqi Fang huaqi.fang@synopsys.com, Synopsys, Inc.
 */

/**
 * \file    xively.c
 * \brief   Xively C library [see xively.h]
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "xi_allocator.h"
#include "xively.h"
#include "xi_macros.h"
#include "xi_debug.h"
#include "xi_helpers.h"
#include "xi_err.h"
#include "xi_globals.h"
#include "xi_common.h"
#include "xi_layer_api.h"
#include "xi_layer_interface.h"
#include "xi_layer_connection.h"
#include "xi_layer_types_conf.h"
#include "xi_layer_factory.h"
#include "xi_layer_factory_conf.h"
#include "xi_layer_default_allocators.h"
#include "xi_http_layer.h"
#include "xi_http_layer_data.h"
#include "xi_csv_layer.h"
#include "xi_connection_data.h"

#include "embARC_debug.h"

#if XI_DEBUG_OUTPUT
    #define XIVELY_DEBUG_PRINTF(...)
#else
    #define XIVELY_DEBUG_PRINTF(...)	EMBARC_PRINTF(__VA_ARGS__)
#endif

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------
// HELPER MACROS
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
// HELPER FUNCTIONS
//-----------------------------------------------------------------------

xi_value_type_t xi_get_value_type( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );

    return p->value_type;
}

xi_datapoint_t* xi_set_value_i32( xi_datapoint_t* p, int32_t value )
{
    // PRECONDITION
    assert( p != 0 );

    p->value.i32_value  = value;
    p->value_type       = XI_VALUE_TYPE_I32;

    return p;
}

int32_t xi_get_value_i32( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );
    assert( p->value_type == XI_VALUE_TYPE_I32 );

    return p->value.i32_value;
}

int32_t* xi_value_pointer_i32( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );

    if( p->value_type == XI_VALUE_TYPE_I32 )
    {
      return &p->value.i32_value;
    }
    else
    {
      return NULL;
    }
}

xi_datapoint_t* xi_set_value_f32( xi_datapoint_t* p, float value )
{
    // PRECONDITION
    assert( p != 0 );

    p->value.f32_value  = value;
    p->value_type       = XI_VALUE_TYPE_F32;

    return p;
}

float xi_get_value_f32( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );
    assert( p->value_type == XI_VALUE_TYPE_F32 );

    if( p->value_type == XI_VALUE_TYPE_F32 )
    {
      return p->value.f32_value;
    }
    else
    {
      return 0.;
    }
}

float* xi_value_pointer_f32( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );

    if( p->value_type == XI_VALUE_TYPE_F32 )
    {
      return &p->value.f32_value;
    }
    else
    {
      return NULL;
    }
}

xi_datapoint_t* xi_set_value_str( xi_datapoint_t* p, const char* value )
{
    // PRECONDITION
    assert( p != 0 );

    int s = xi_str_copy_untiln( p->value.str_value
        , XI_VALUE_STRING_MAX_SIZE, value, '\0' );

    XI_CHECK_SIZE( s, XI_VALUE_STRING_MAX_SIZE, XI_DATAPOINT_VALUE_BUFFER_OVERFLOW );

    p->value_type = XI_VALUE_TYPE_STR;

    return p;

err_handling:
    return 0;
}

char* xi_value_pointer_str( xi_datapoint_t* p )
{
    // PRECONDITION
    assert( p != 0 );

    if( p->value_type == XI_VALUE_TYPE_STR )
    {
      return p->value.str_value;
    }
    else
    {
      return NULL;
    }
}

void xi_set_network_timeout( uint32_t timeout )
{
    xi_globals.network_timeout = timeout;
}

uint32_t xi_get_network_timeout( void )
{
    return xi_globals.network_timeout;
}

//-----------------------------------------------------------------------
// LAYERS SETTINGS
//-----------------------------------------------------------------------

#define XI_IO_POSIX           0
#define XI_IO_DUMMY           1
#define XI_IO_MBED            2
#define XI_IO_POSIX_ASYNCH    3
#define XI_IO_CONTIKI         4

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief The LAYERS_ID enum
///
enum LAYERS_ID
{
      IO_LAYER = 0
    , HTTP_LAYER
    , CSV_LAYER
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CONNECTION_SCHEME_1_DATA IO_LAYER, HTTP_LAYER, CSV_LAYER
DEFINE_CONNECTION_SCHEME( CONNECTION_SCHEME_1, CONNECTION_SCHEME_1_DATA );

#if XI_IO_LAYER == XI_IO_POSIX

    // posix io layer
    #include "posix_io_layer.h"

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BEGIN_LAYER_TYPES_CONF()
          LAYER_TYPE( IO_LAYER, &posix_io_layer_data_ready, &posix_io_layer_on_data_ready
                                , &posix_io_layer_close, &posix_io_layer_on_close
                                , &posix_io_layer_init, &posix_io_layer_connect )
        , LAYER_TYPE( HTTP_LAYER, &http_layer_data_ready, &http_layer_on_data_ready
                                , &http_layer_close, &http_layer_on_close, 0, 0 )
        , LAYER_TYPE( CSV_LAYER, &csv_layer_data_ready, &csv_layer_on_data_ready
                            , &csv_layer_close, &csv_layer_on_close, 0, 0 )
    END_LAYER_TYPES_CONF()

#elif XI_IO_LAYER == XI_IO_DUMMY
    // dummy io layer
    #include "dummy_io_layer.h"

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BEGIN_LAYER_TYPES_CONF()
          LAYER_TYPE( IO_LAYER, &dummy_io_layer_data_ready, &dummy_io_layer_on_data_ready
                              , &dummy_io_layer_close, &dummy_io_layer_on_close
                              , &dummy_io_layer_init, &dummy_io_layer_connect )
        , LAYER_TYPE( HTTP_LAYER, &http_layer_data_ready, &http_layer_on_data_ready
                                , &http_layer_close, &http_layer_on_close, 0, 0 )
        , LAYER_TYPE( CSV_LAYER, &csv_layer_data_ready, &csv_layer_on_data_ready
                            , &csv_layer_close, &csv_layer_on_close, 0, 0 )
    END_LAYER_TYPES_CONF()

#elif XI_IO_LAYER == XI_IO_MBED
    // mbed io layer
    #include "mbed_io_layer.h"

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BEGIN_LAYER_TYPES_CONF()
          LAYER_TYPE( IO_LAYER, &mbed_io_layer_data_ready, &mbed_io_layer_on_data_ready
                              , &mbed_io_layer_close, &mbed_io_layer_on_close )
        , LAYER_TYPE( HTTP_LAYER, &http_layer_data_ready, &http_layer_on_data_ready
                                , &http_layer_close, &http_layer_on_close )
        , LAYER_TYPE( CSV_LAYER, &csv_layer_data_ready, &csv_layer_on_data_ready
                            , &csv_layer_close, &csv_layer_on_close )
    END_LAYER_TYPES_CONF()

#elif XI_IO_LAYER == XI_IO_POSIX_ASYNCH
    // mbed io layer
    #include "posix_asynch_io_layer.h"

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BEGIN_LAYER_TYPES_CONF()
          LAYER_TYPE( IO_LAYER, &posix_asynch_io_layer_data_ready, &posix_asynch_io_layer_on_data_ready
                              , &posix_asynch_io_layer_close, &posix_asynch_io_layer_on_close
                              , &posix_asynch_io_layer_init, &posix_asynch_io_layer_connect )
        , LAYER_TYPE( HTTP_LAYER, &http_layer_data_ready, &http_layer_on_data_ready
                                , &http_layer_close, &http_layer_on_close, 0, 0 )
        , LAYER_TYPE( CSV_LAYER, &csv_layer_data_ready, &csv_layer_on_data_ready
                            , &csv_layer_close, &csv_layer_on_close, 0, 0 )
    END_LAYER_TYPES_CONF()
#elif XI_IO_LAYER == XI_IO_CONTIKI
    // contiki io layer
    #include "contiki_io_layer.h"

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BEGIN_LAYER_TYPES_CONF()
          LAYER_TYPE( IO_LAYER, &contiki_io_layer_data_ready, &contiki_io_layer_on_data_ready
                              , &contiki_io_layer_close, &contiki_io_layer_on_close
                              , &contiki_io_layer_init, &contiki_io_layer_connect)
        , LAYER_TYPE( HTTP_LAYER, &http_layer_data_ready, &http_layer_on_data_ready
                                , &http_layer_close, &http_layer_on_close, 0, 0 )
        , LAYER_TYPE( CSV_LAYER, &csv_layer_data_ready, &csv_layer_on_data_ready
                            , &csv_layer_close, &csv_layer_on_close, 0, 0 )
    END_LAYER_TYPES_CONF()

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_FACTORY_CONF()
      FACTORY_ENTRY( IO_LAYER, &placement_layer_pass_create, &placement_layer_pass_delete
                             , &default_layer_heap_alloc, &default_layer_heap_free )
    , FACTORY_ENTRY( HTTP_LAYER, &placement_layer_pass_create, &placement_layer_pass_delete
                               , &default_layer_heap_alloc, &default_layer_heap_free )
    , FACTORY_ENTRY( CSV_LAYER, &placement_layer_pass_create, &placement_layer_pass_delete
                           , &default_layer_heap_alloc, &default_layer_heap_free )
END_FACTORY_CONF()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------
// MAIN LIBRARY FUNCTIONS
//-----------------------------------------------------------------------

xi_context_t* xi_create_context(
      xi_protocol_t protocol, const char* api_key
    , xi_feed_id_t feed_id )
{
    // allocate the structure to store new context
    xi_context_t* ret = ( xi_context_t* ) xi_alloc( sizeof( xi_context_t ) );

    XI_CHECK_MEMORY( ret );

    // copy given numeric parameters as is
    ret->protocol       = protocol;
    ret->feed_id        = feed_id;

    // copy string parameters carefully
    if( api_key )
    {
        // duplicate the string
        ret->api_key  = xi_str_dup( api_key );

        XI_CHECK_MEMORY( ret->api_key );
    }
    else
    {
        ret->api_key  = 0;
    }

    switch( protocol )
    {
        case XI_HTTP:
            {
                // @TODO make a configurable pool of these
                // static structures allocated statically
                static http_layer_data_t    http_layer_data;
                static csv_layer_data_t     csv_layer_data;
                static xi_response_t        xi_response;


                // clean the structures
                memset( &http_layer_data, 0, sizeof( http_layer_data_t ) );
                memset( &csv_layer_data, 0, sizeof( csv_layer_data_t ) );
                memset( &xi_response, 0, sizeof( xi_response_t ) );

                // the response pointer
                http_layer_data.response    = &xi_response;
                csv_layer_data.response     = &xi_response;

                // prepare user data description
                void* user_datas[] = { 0, ( void* ) &http_layer_data, ( void* ) &csv_layer_data };

                // create and connect layers store the information in layer_chain member
                ret->layer_chain = create_and_connect_layers( CONNECTION_SCHEME_1, user_datas, CONNECTION_SCHEME_LENGTH( CONNECTION_SCHEME_1 ) );
            }
            break;
        default:
            goto err_handling;
    }


    return ret;

err_handling:
    if( ret )
    {
        if( ret->api_key )
        {
            XI_SAFE_FREE( ret->api_key );
        }

        XI_SAFE_FREE( ret );
    }

    return 0;
}

void xi_delete_context( xi_context_t* context )
{
    assert( context != 0 && "context must not be null!" );

    switch( context->protocol )
    {
        case XI_HTTP:
            destroy_and_disconnect_layers( &( context->layer_chain ), CONNECTION_SCHEME_LENGTH( CONNECTION_SCHEME_1 ) );
            break;
        default:
            assert( 0 && "not yet implemented!" );
            break;
    }

    XI_SAFE_FREE( context->api_key );
    XI_SAFE_FREE( context );
}

#ifndef XI_NOB_ENABLED
const xi_response_t* xi_feed_get(
          xi_context_t* xi
        , xi_feed_t* feed )
{
    // we shall need it later
    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    { // init & connect
        state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }

        xi_connection_data_t conn_data = { XI_HOST, XI_PORT };

        state = CALL_ON_SELF_CONNECT( io_layer, ( void *) &conn_data, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }
    }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_FEED_GET
        , xi
        , 0
        , { .xi_get_feed = { .feed = feed } }
    };

    state = CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );

    if( state == LAYER_STATE_OK )
    {
        CALL_ON_SELF_ON_DATA_READY( io_layer, ( void *) 0, LAYER_HINT_NONE );
    }

    CALL_ON_SELF_CLOSE( input_layer );

    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}

const xi_response_t* xi_feed_get_all(
          xi_context_t* xi
        , xi_feed_t* feed )
{
    // we shall need it later
    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    { // init & connect
        state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }

        xi_connection_data_t conn_data = { XI_HOST, XI_PORT };

        state = CALL_ON_SELF_CONNECT( io_layer, ( void *) &conn_data, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }
    }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_FEED_GET_ALL
        , xi
        , 0
        , { .xi_get_feed = { .feed = feed } }
    };

    state = CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );

    if( state == LAYER_STATE_OK )
    {
        CALL_ON_SELF_ON_DATA_READY( io_layer, ( void *) 0, LAYER_HINT_NONE );
    }

    CALL_ON_SELF_CLOSE( input_layer );

    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}


const xi_response_t* xi_feed_update(
          xi_context_t* xi
        , const xi_feed_t* feed )
{
    // we shall need it later
    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

	XIVELY_DEBUG_PRINTF("XIVELY FEED UPDATE:");
    { // init & connect
		XIVELY_DEBUG_PRINTF("INIT-> ");
        state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }
        xi_connection_data_t conn_data = { XI_HOST, XI_PORT };

		XIVELY_DEBUG_PRINTF("CONNECT-> ");
        state = CALL_ON_SELF_CONNECT( io_layer, ( void *) &conn_data, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }
    }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_FEED_UPDATE
        , xi
        , 0
        , { .xi_update_feed = { ( xi_feed_t * ) feed } }
    };

	XIVELY_DEBUG_PRINTF("SEND-> ");
    state = CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );

    if( state == LAYER_STATE_OK )
    {
		XIVELY_DEBUG_PRINTF("RESPONSE-> ");
        CALL_ON_SELF_ON_DATA_READY( io_layer, ( void *) 0, LAYER_HINT_NONE );
    }

	XIVELY_DEBUG_PRINTF("CLOSE-> ");
    CALL_ON_SELF_CLOSE( input_layer );

	XIVELY_DEBUG_PRINTF("DONE\r\n");
    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}

const xi_response_t* xi_datastream_get(
            xi_context_t* xi, xi_feed_id_t feed_id
          , const char * datastream_id, xi_datapoint_t* o )
{
    XI_UNUSED( feed_id );

    // we shall need it later
    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    { // init & connect
        state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }

        xi_connection_data_t conn_data = { XI_HOST, XI_PORT };

        state = CALL_ON_SELF_CONNECT( io_layer, ( void *) &conn_data, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }
    }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_DATASTREAM_GET
        , xi
        , 0
        , { ( struct xi_get_datastream_t ) { datastream_id, o } }
    };

    state = CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );

    if( state == LAYER_STATE_OK )
    {
        CALL_ON_SELF_ON_DATA_READY( io_layer, ( void *) 0, LAYER_HINT_NONE );
    }

    CALL_ON_SELF_CLOSE( input_layer );

    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}


const xi_response_t* xi_datastream_create(
            xi_context_t* xi, xi_feed_id_t feed_id
          , const char * datastream_id
          , const xi_datapoint_t* datapoint )
{
    XI_UNUSED( feed_id );

    // we shall need it later
    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    { // init & connect
        state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }

        xi_connection_data_t conn_data = { XI_HOST, XI_PORT };

        state = CALL_ON_SELF_CONNECT( io_layer, ( void *) &conn_data, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }
    }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_DATASTREAM_CREATE
        , xi
        , 0
        , { .xi_create_datastream = { ( char* ) datastream_id, ( xi_datapoint_t* ) datapoint } }
    };

    state = CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );

    if( state == LAYER_STATE_OK )
    {
        CALL_ON_SELF_ON_DATA_READY( io_layer, ( void *) 0, LAYER_HINT_NONE );
    }

    CALL_ON_SELF_CLOSE( input_layer );

    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}

const xi_response_t* xi_datastream_update(
          xi_context_t* xi, xi_feed_id_t feed_id
        , const char * datastream_id
        , const xi_datapoint_t* datapoint )
{
    XI_UNUSED( feed_id );

    // we shall need it later
    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    { // init & connect
        state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }

        xi_connection_data_t conn_data = { XI_HOST, XI_PORT };

        state = CALL_ON_SELF_CONNECT( io_layer, ( void *) &conn_data, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }
    }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_DATASTREAM_UPDATE
        , xi
        , 0
        , { .xi_update_datastream = { ( char* ) datastream_id, ( xi_datapoint_t* ) datapoint } }
    };

    state = CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );

    if( state == LAYER_STATE_OK )
    {
        CALL_ON_SELF_ON_DATA_READY( io_layer, ( void *) 0, LAYER_HINT_NONE );
    }

    CALL_ON_SELF_CLOSE( input_layer );

    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}

const xi_response_t* xi_datastream_delete(
            xi_context_t* xi, xi_feed_id_t feed_id
          , const char * datastream_id )
{
    XI_UNUSED( feed_id );

    // we shall need it later
    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    { // init & connect
        state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }

        xi_connection_data_t conn_data = { XI_HOST, XI_PORT };

        state = CALL_ON_SELF_CONNECT( io_layer, ( void *) &conn_data, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }
    }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_DATASTREAM_DELETE
        , xi
        , 0
        , { .xi_delete_datastream = { datastream_id } }
    };

    state = CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );

    if( state == LAYER_STATE_OK )
    {
        CALL_ON_SELF_ON_DATA_READY( io_layer, ( void *) 0, LAYER_HINT_NONE );
    }

    CALL_ON_SELF_CLOSE( input_layer );

    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}

const xi_response_t* xi_datapoint_delete(
          const xi_context_t* xi, xi_feed_id_t feed_id
        , const char * datastream_id
        , const xi_datapoint_t* o )
{
    XI_UNUSED( feed_id );

    // we shall need it later
    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    { // init & connect
        state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }

        xi_connection_data_t conn_data = { XI_HOST, XI_PORT };

        state = CALL_ON_SELF_CONNECT( io_layer, ( void *) &conn_data, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }
    }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_DATAPOINT_DELETE
        , ( xi_context_t* ) xi
        , 0
        , { .xi_delete_datapoint = { ( char* ) datastream_id, ( xi_datapoint_t* ) o } }
    };

    state = CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );

    if( state == LAYER_STATE_OK )
    {
        CALL_ON_SELF_ON_DATA_READY( io_layer, ( void *) 0, LAYER_HINT_NONE );
    }

    CALL_ON_SELF_CLOSE( input_layer );

    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}

extern const xi_response_t* xi_datapoint_delete_range(
            const xi_context_t* xi, xi_feed_id_t feed_id
          , const char * datastream_id
          , const xi_timestamp_t* start
          , const xi_timestamp_t* end )
{
    XI_UNUSED( feed_id );

    // we shall need it later
    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    { // init & connect
        state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }

        xi_connection_data_t conn_data = { XI_HOST, XI_PORT };

        state = CALL_ON_SELF_CONNECT( io_layer, ( void *) &conn_data, LAYER_HINT_NONE );
        if( state != LAYER_STATE_OK ) { return 0; }
    }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    http_layer_input_t http_layer_input =
    {
          HTTP_LAYER_INPUT_DATAPOINT_DELETE_RANGE
        , ( xi_context_t* ) xi
        , 0
        , { .xi_delete_datapoint_range = { ( char* ) datastream_id, ( xi_timestamp_t* ) start, ( xi_timestamp_t* ) end } }
    };

    state = CALL_ON_SELF_DATA_READY( input_layer, ( void *) &http_layer_input, LAYER_HINT_NONE );

    if( state == LAYER_STATE_OK )
    {
        CALL_ON_SELF_ON_DATA_READY( io_layer, ( void *) 0, LAYER_HINT_NONE );
    }

    CALL_ON_SELF_CLOSE( input_layer );

    return ( ( csv_layer_data_t* ) input_layer->user_data )->response;
}
#else
extern const xi_context_t* xi_nob_feed_update(
         xi_context_t* xi
       , const xi_feed_t* value )
{
    // we shall need it later
    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
    if( state != LAYER_STATE_OK ) { return 0; }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    static http_layer_input_t http_layer_input;
    memset( &http_layer_input, 0, sizeof( http_layer_input_t ) );

    // set the layer input
    http_layer_input.query_type = HTTP_LAYER_INPUT_FEED_UPDATE;
    http_layer_input.xi_context = xi;
    http_layer_input.http_union_data.xi_update_feed.feed = value;

    // assign the input parameter so that can be used via the runner
    xi->input = &http_layer_input;

    return xi;
}

extern const xi_context_t* xi_nob_feed_get(
         xi_context_t* xi
       , xi_feed_t* value )
{
    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
    if( state != LAYER_STATE_OK ) { return 0; }
    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    static http_layer_input_t http_layer_input;
    memset( &http_layer_input, 0, sizeof( http_layer_input_t ) );

    // set the layer input
    http_layer_input.query_type = HTTP_LAYER_INPUT_FEED_GET;
    http_layer_input.xi_context = xi;
    http_layer_input.http_union_data.xi_get_feed.feed = value;

    // assign the input parameter so that can be used via the runner
    xi->input = &http_layer_input;

    return xi;
}

extern const xi_context_t* xi_nob_feed_get_all(
          xi_context_t* xi
        , xi_feed_t* value )
{
    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
    if( state != LAYER_STATE_OK ) { return 0; }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    static http_layer_input_t http_layer_input;
    memset( &http_layer_input, 0, sizeof( http_layer_input_t ) );

    // set the layer input
    http_layer_input.query_type = HTTP_LAYER_INPUT_FEED_GET_ALL;
    http_layer_input.xi_context = xi;
    http_layer_input.http_union_data.xi_get_feed.feed = value;

    // assign the input parameter so that can be used via the runner
    xi->input = &http_layer_input;

    return xi;
}


extern const xi_context_t* xi_nob_datastream_create(
         xi_context_t* xi, xi_feed_id_t feed_id
       , const char * datastream_id
       , const xi_datapoint_t* value )
{
    XI_UNUSED( feed_id );

    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
    if( state != LAYER_STATE_OK ) { return 0; }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    static http_layer_input_t http_layer_input;
    memset( &http_layer_input, 0, sizeof( http_layer_input_t ) );

    // set the layer input
    http_layer_input.query_type                                         = HTTP_LAYER_INPUT_DATASTREAM_CREATE;
    http_layer_input.xi_context                                         = xi;
    http_layer_input.http_union_data.xi_create_datastream.datastream    = datastream_id;
    http_layer_input.http_union_data.xi_create_datastream.value         = value;

    // assign the input parameter so that can be used via the runner
    xi->input = &http_layer_input;

    return xi;
}

extern const xi_context_t* xi_nob_datastream_update(
         xi_context_t* xi, xi_feed_id_t feed_id
       , const char * datastream_id
       , const xi_datapoint_t* value )
{
    XI_UNUSED( feed_id );

    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
    if( state != LAYER_STATE_OK ) { return 0; }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    static http_layer_input_t http_layer_input;
    memset( &http_layer_input, 0, sizeof( http_layer_input_t ) );

    // set the layer input
    http_layer_input.query_type                                         = HTTP_LAYER_INPUT_DATASTREAM_UPDATE;
    http_layer_input.xi_context                                         = xi;
    http_layer_input.http_union_data.xi_update_datastream.datastream    = datastream_id;
    http_layer_input.http_union_data.xi_update_datastream.value         = value;

    // assign the input parameter so that can be used via the runner
    xi->input = &http_layer_input;

    return xi;
}

const xi_context_t* xi_nob_datastream_get(
         xi_context_t* xi, xi_feed_id_t feed_id
       , const char * datastream_id, xi_datapoint_t* dp )
{
    XI_UNUSED( feed_id );

    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
    if( state != LAYER_STATE_OK ) { return 0; }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    static http_layer_input_t http_layer_input;
    memset( &http_layer_input, 0, sizeof( http_layer_input_t ) );

    // set the layer input
    http_layer_input.query_type = HTTP_LAYER_INPUT_DATASTREAM_GET;
    http_layer_input.xi_context = xi;
    http_layer_input.http_union_data.xi_get_datastream.datastream = datastream_id;
    http_layer_input.http_union_data.xi_get_datastream.value = dp;

    // assign the input parameter so that can be used via the runner
    xi->input = &http_layer_input;

    return xi;
}

const xi_context_t* xi_nob_datastream_delete(
         xi_context_t* xi, xi_feed_id_t feed_id
       , const char* datastream_id )
{
    XI_UNUSED( feed_id );

    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
    if( state != LAYER_STATE_OK ) { return 0; }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    static http_layer_input_t http_layer_input;
    memset( &http_layer_input, 0, sizeof( http_layer_input_t ) );

    // set the layer input
    http_layer_input.query_type                                         = HTTP_LAYER_INPUT_DATASTREAM_DELETE;
    http_layer_input.xi_context                                         = xi;
    http_layer_input.http_union_data.xi_delete_datastream.datastream    = datastream_id;

    // assign the input parameter so that can be used via the runner
    xi->input = &http_layer_input;

    return xi;
}

const xi_context_t* xi_nob_datapoint_delete(
         xi_context_t* xi, xi_feed_id_t feed_id
       , const char * datastream_id
       , const xi_datapoint_t* dp )
{
    XI_UNUSED( feed_id );

    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
    if( state != LAYER_STATE_OK ) { return 0; }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    static http_layer_input_t http_layer_input;
    memset( &http_layer_input, 0, sizeof( http_layer_input_t ) );

    // set the layer input
    http_layer_input.query_type                                         = HTTP_LAYER_INPUT_DATAPOINT_DELETE;
    http_layer_input.xi_context                                         = xi;
    http_layer_input.http_union_data.xi_delete_datapoint.datastream     = datastream_id;
    http_layer_input.http_union_data.xi_delete_datapoint.value          = dp;

    // assign the input parameter so that can be used via the runner
    xi->input = &http_layer_input;

    return xi;
}

const xi_context_t* xi_nob_datapoint_delete_range(
        xi_context_t* xi, xi_feed_id_t feed_id, const char * datastream_id
      , const xi_timestamp_t* start, const xi_timestamp_t* end )
{
    XI_UNUSED( feed_id );

    layer_state_t state = LAYER_STATE_OK;

    // extract the input layer
    layer_t* input_layer    = xi->layer_chain.top;
    layer_t* io_layer       = xi->layer_chain.bottom;

    state = CALL_ON_SELF_INIT( io_layer, 0, LAYER_HINT_NONE );
    if( state != LAYER_STATE_OK ) { return 0; }

    // clean the response before writing to it
    memset( ( ( csv_layer_data_t* ) input_layer->user_data )->response, 0, sizeof( xi_response_t ) );

    // create the input parameter
    static http_layer_input_t http_layer_input;
    memset( &http_layer_input, 0, sizeof( http_layer_input_t ) );

    // set the layer input
    http_layer_input.query_type                                             = HTTP_LAYER_INPUT_DATAPOINT_DELETE_RANGE;
    http_layer_input.xi_context                                             = xi;
    http_layer_input.http_union_data.xi_delete_datapoint_range.datastream   = datastream_id;
    http_layer_input.http_union_data.xi_delete_datapoint_range.value_start  = start;
    http_layer_input.http_union_data.xi_delete_datapoint_range.value_end    = end;

    // assign the input parameter so that can be used via the runner
    xi->input = &http_layer_input;

    return xi;
}
#endif // XI_NOB_ENABLED

#ifdef __cplusplus
}
#endif
