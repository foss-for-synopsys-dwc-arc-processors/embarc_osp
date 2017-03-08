// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.
#include <string.h>
#include "xi_layer_api.h"
#include "xi_common.h"
#include "xi_http_layer.h"
#include "xi_http_layer_constants.h"
#include "xi_http_layer_input.h"
#include "xi_http_layer_data.h"
#include "xi_macros.h"
#include "xi_debug.h"
#include "xi_stated_sscanf.h"
#include "xi_coroutine.h"
#include "xi_layer_helpers.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @note
 * in windows: compare ignore case funcs is stricmp & strnicmp
 * in linux: ignore case funcs is strcasecmp & strncasecmp
 */
#ifdef __MW__
static int strncasecmp(const char *s1, const char *s2, size_t n)
{
  return _strnicmp(s1, s2, n);
}
static int strcasecmp(const char *s1, const char *s2)
{
  return _stricmp(s1, s2);
}
#endif

// static array of recognizable http headers
static const char* XI_HTTP_TOKEN_NAMES[ XI_HTTP_HEADERS_COUNT ] =
    {
          "date"            // XI_HTTP_HEADER_DATE
        , "content-type"    // XI_HTTP_HEADER_CONTENT_TYPE
        , "content-length"  // XI_HTTP_HEADER_CONTENT_LENGTH
        , "connection"      // XI_HTTP_HEADER_CONNECTION
        , "x-request-id"    // XI_HTTP_HEADER_X_REQUEST_ID
        , "cache-control"   // XI_HTTP_HEADER_CACHE_CONTROL
        , "age"             // XI_HTTP_HEADER_AGE
        , "vary"            // XI_HTTP_HEADER_VARY
        , "unknown"         // XI_HTTP_HEADER_UNKNOWN, //!< !!!! this must be always on the last position
    };

static inline http_header_type_t classify_header( const char* header )
{
    for( unsigned short i = 0; i < XI_HTTP_HEADER_COUNT - 1; ++i )
    {
        if( strcasecmp( header, XI_HTTP_TOKEN_NAMES[ i ] ) == 0 )
            return ( http_header_type_t ) i;
    }

    return XI_HTTP_HEADER_UNKNOWN;
}

const void* http_layer_data_generator_query_body(
          const void* input
        , short* state )
{
    // unpack the data
    const http_layer_input_t* const http_layer_input
            = ( const http_layer_input_t* ) input;

    // required if sending the payload
    static unsigned short cnt_len   = 0;
    static const char* const p1     = XI_HOST;
    static const char* const p2     = XI_USER_AGENT;

    short ret_state                         = 0;
    const const_data_descriptor_t* data     = 0;

    ENABLE_GENERATOR();

    BEGIN_CORO( *state )

            // reset the content lenght
            cnt_len = 0;

            // SEND HOST
            gen_ptr_text( *state, XI_HTTP_TEMPLATE_HOST );
            gen_ptr_text( *state, p1 );
            gen_ptr_text( *state, XI_HTTP_CRLF );

            // SEND USER AGENT
            gen_ptr_text( *state, XI_HTTP_TEMPLATE_USER_AGENT );
            gen_ptr_text( *state, p2 );
            gen_ptr_text( *state, XI_HTTP_CRLF );

            // SEND ACCEPT
            gen_ptr_text( *state, XI_HTTP_TEMPLATE_ACCEPT );
            gen_ptr_text( *state, XI_HTTP_CRLF );

            // if there is a payload we have to calculate it's size and then send it
            if( http_layer_input->payload_generator )
            {
                while( ret_state != 1 )
                {
                    data        = (*http_layer_input->payload_generator)( &http_layer_input->http_union_data, &ret_state );
                    cnt_len    += data->real_size;
                }

                memset( buffer_32, 0, 32 );
                sprintf( buffer_32, "%d", ( int ) cnt_len );

                gen_ptr_text( *state, XI_HTTP_CONTENT_LENGTH );
                gen_ptr_text( *state, buffer_32 );
                gen_ptr_text( *state, XI_HTTP_CRLF );
            }

            // A API KEY
            gen_ptr_text( *state, XI_HTTP_TEMPLATE_X_API_KEY );
            gen_ptr_text( *state, http_layer_input->xi_context->api_key ); // api key
            gen_ptr_text( *state, XI_HTTP_CRLF );

            // the end, no more data double crlf
            if( http_layer_input->payload_generator )
            {
                gen_ptr_text( *state, XI_HTTP_CRLF );
            }
            else
            {
                gen_ptr_text_and_exit( *state, XI_HTTP_CRLF );
            }

            // if generator exists pass the execution
            if( http_layer_input->payload_generator )
            {
                call_sub_gen_and_exit( *state
                                       , ( const void* ) &http_layer_input->http_union_data
                                       , (*http_layer_input->payload_generator) );
            }

    END_CORO()

    return 0;
}

const void* http_layer_data_generator_datastream_body(
          const void* input
        , short* state )
{
    // unpack the data
    const http_layer_input_t* const http_layer_input
            = ( const http_layer_input_t* ) input;

    ENABLE_GENERATOR();

    BEGIN_CORO( *state )

        {
            memset( buffer_32, 0, 32 );
            sprintf( buffer_32, "%"PRIu32, ( uint32_t ) http_layer_input->xi_context->feed_id );
            gen_ptr_text( *state, buffer_32 ); // feed id
        }

        gen_ptr_text( *state, XI_CSV_SLASH );
        gen_ptr_text( *state, XI_CSV_DATASTREAMS );
        gen_ptr_text( *state, XI_CSV_SLASH );

        gen_ptr_text( *state, http_layer_input->http_union_data.xi_get_datastream.datastream ); // datastream id
        gen_ptr_text( *state, XI_CSV_DOT_CSV_SPACE );

        // SEND HTTP
        gen_ptr_text( *state, XI_HTTP_TEMPLATE_HTTP );
        gen_ptr_text( *state, XI_HTTP_CRLF );

        // SEND THE REST THROUGH SUB GENERATOR
        call_sub_gen_and_exit( *state, input, http_layer_data_generator_query_body );

    END_CORO()

    return 0;
}

const void* http_layer_data_generator_datastream_get(
          const void* input
        , short* state )
{
    ENABLE_GENERATOR();

    BEGIN_CORO( *state )

        gen_ptr_text( *state, XI_HTTP_GET );
        gen_ptr_text( *state, XI_HTTP_TEMPLATE_FEED );
        gen_ptr_text( *state, XI_CSV_SLASH );

        // SEND THE REST THROUGH SUB GENERATOR
        call_sub_gen_and_exit( *state, input, http_layer_data_generator_datastream_body );

    END_CORO()

    return 0;
}

const void* http_layer_data_generator_datastream_update(
          const void* input
        , short* state )
{
    ENABLE_GENERATOR();

    BEGIN_CORO( *state )

        gen_ptr_text( *state, XI_HTTP_PUT );
        gen_ptr_text( *state, XI_HTTP_TEMPLATE_FEED );
        gen_ptr_text( *state, XI_CSV_SLASH );

        // SEND THE REST THROUGH SUB GENERATOR
        call_sub_gen_and_exit( *state, input, http_layer_data_generator_datastream_body );

    END_CORO()

    return 0;
}

const void* http_layer_data_generator_datastream_create_body(
          const void* input
        , short* state )
{
    // unpack the data
    const http_layer_input_t* const http_layer_input
            = ( const http_layer_input_t* ) input;

    ENABLE_GENERATOR();

    BEGIN_CORO( *state )

        {
            memset( buffer_32, 0, 32 );
            sprintf( buffer_32, "%"PRIu32, ( uint32_t ) http_layer_input->xi_context->feed_id );
            gen_ptr_text( *state, buffer_32 ); // feed id
        }

        gen_ptr_text( *state, XI_CSV_SLASH );
        gen_ptr_text( *state, XI_CSV_DATASTREAMS );
        gen_static_text( *state, ".csv" );

        // SEND HTTP
        gen_ptr_text( *state, XI_HTTP_TEMPLATE_HTTP );
        gen_ptr_text( *state, XI_HTTP_CRLF );

        // SEND THE REST THROUGH SUB GENERATOR
        call_sub_gen_and_exit( *state, input, http_layer_data_generator_query_body );

    END_CORO()

    return 0;
}

const void* http_layer_data_generator_datastream_create(
          const void* input
        , short* state )
{
    ENABLE_GENERATOR();

    BEGIN_CORO( *state )

        gen_ptr_text( *state, XI_HTTP_POST );
        gen_ptr_text( *state, XI_HTTP_TEMPLATE_FEED );
        gen_ptr_text( *state, XI_CSV_SLASH );

        // SEND THE REST THROUGH SUB GENERATOR
        call_sub_gen_and_exit( *state, input, http_layer_data_generator_datastream_create_body );

    END_CORO()

    return 0;
}

const void* http_layer_data_generator_feed_get(
          const void* input
        , short* state )
{
    // unpack the data
    const http_layer_input_t* const http_layer_input
            = ( const http_layer_input_t* ) input;

    // local patterns
    static const char* const p1 = ".csv?datastreams=";

    // PRECONDITIONS
    assert( http_layer_input->http_union_data.xi_get_feed.feed->datastream_count > 0 );

    static unsigned char i = 0;

    ENABLE_GENERATOR();

    BEGIN_CORO( *state )

        // after starting coro reset static counter
        i = 1;

        gen_ptr_text( *state, XI_HTTP_GET );
        gen_ptr_text( *state, XI_HTTP_TEMPLATE_FEED );
        gen_ptr_text( *state, XI_CSV_SLASH );

        {
            memset( buffer_32, 0, 32 );
            sprintf( buffer_32, "%"PRIu32, ( uint32_t ) http_layer_input->xi_context->feed_id );
            gen_ptr_text( *state, buffer_32 ); // feed id
        }

        gen_ptr_text( *state, p1 );

        // 0 case separated
        gen_ptr_text( *state, http_layer_input->http_union_data.xi_get_feed.feed->datastreams[ 0 ].datastream_id );

        // iterate over all datastreams
        {
            for( ; i < http_layer_input->http_union_data.xi_get_feed.feed->datastream_count; ++i )
            {
                gen_ptr_text( *state, XI_CSV_COMMA );
                gen_ptr_text( *state, http_layer_input->http_union_data.xi_get_feed.feed->datastreams[ i ].datastream_id );
            }
        }

        // space
        gen_ptr_text( *state, XI_HTTP_SPACE );

        // SEND HTTP
        gen_ptr_text( *state, XI_HTTP_TEMPLATE_HTTP );
        gen_ptr_text( *state, XI_HTTP_CRLF );

        // SEND THE REST THROUGH SUB GENERATOR
        call_sub_gen_and_exit( *state, input, http_layer_data_generator_query_body );

    END_CORO()

    return 0;
}

const void* http_layer_data_generator_feed_get_all(
          const void* input
        , short* state )
{
    // unpack the data
    const http_layer_input_t* const http_layer_input
            = ( const http_layer_input_t* ) input;

    // PRECONDITIONS
    assert( http_layer_input->http_union_data.xi_get_feed.feed != 0 );

    ENABLE_GENERATOR();

    BEGIN_CORO( *state )

        gen_ptr_text( *state, XI_HTTP_GET );
        gen_ptr_text( *state, XI_HTTP_TEMPLATE_FEED );
        gen_ptr_text( *state, XI_CSV_SLASH );

        {
            memset( buffer_32, 0, 32 );
            sprintf( buffer_32, "%"PRIu32, ( uint32_t ) http_layer_input->xi_context->feed_id );
            gen_ptr_text( *state, buffer_32 ); // feed id
        }

        gen_ptr_text( *state, XI_CSV_DOT_CSV_SPACE );

        // SEND HTTP
        gen_ptr_text( *state, XI_HTTP_TEMPLATE_HTTP );
        gen_ptr_text( *state, XI_HTTP_CRLF );

        // SEND THE REST THROUGH SUB GENERATOR
        call_sub_gen_and_exit( *state, input, http_layer_data_generator_query_body );

    END_CORO()

    return 0;
}

const void* http_layer_data_generator_feed_update(
          const void* input
        , short* state )
{
    // unpack the data
    const http_layer_input_t* const http_layer_input
            = ( const http_layer_input_t* ) input;

    ENABLE_GENERATOR();

    BEGIN_CORO( *state )

        gen_ptr_text( *state, XI_HTTP_PUT );
        gen_ptr_text( *state, XI_HTTP_TEMPLATE_FEED );
        gen_ptr_text( *state, XI_CSV_SLASH );

        {
            memset( buffer_32, 0, 32 );
            sprintf( buffer_32, "%"PRIu32, ( uint32_t ) http_layer_input->xi_context->feed_id );
            gen_ptr_text( *state, buffer_32 ); // feed id
        }

        gen_ptr_text( *state, XI_CSV_DOT_CSV_SPACE );

        // SEND HTTP
        gen_ptr_text( *state, XI_HTTP_TEMPLATE_HTTP );
        gen_ptr_text( *state, XI_HTTP_CRLF );

        // SEND THE REST THROUGH SUB GENERATOR
        call_sub_gen_and_exit( *state, input, http_layer_data_generator_query_body );

    END_CORO()

    return 0;
}

const void* http_layer_data_generator_datastream_delete(
          const void* input
        , short* state )
{
    // unpack the data
    const http_layer_input_t* const http_layer_input
            = ( const http_layer_input_t* ) input;

    ENABLE_GENERATOR();

    BEGIN_CORO( *state )

        memset( buffer_32, 0, 32 );
        sprintf( buffer_32, "%"PRIu32, ( uint32_t ) http_layer_input->xi_context->feed_id );
        gen_ptr_text( *state, buffer_32 ); // feed id

        gen_ptr_text( *state, XI_CSV_SLASH );
        gen_ptr_text( *state, XI_CSV_DATASTREAMS );
        gen_ptr_text( *state, XI_CSV_SLASH );

        gen_ptr_text( *state, http_layer_input->http_union_data.xi_delete_datastream.datastream );
        gen_ptr_text( *state, XI_CSV_DOT_CSV_SPACE );

        // SEND HTTP
        gen_ptr_text( *state, XI_HTTP_TEMPLATE_HTTP );
        gen_ptr_text( *state, XI_HTTP_CRLF );

        // SEND THE REST THROUGH SUB GENERATOR
        call_sub_gen_and_exit( *state, input, http_layer_data_generator_datastream_body );

    END_CORO()

    return 0;
}

const void* http_layer_data_generator_datapoint_delete(
          const void* input
        , short* state )
{
    // unpack the data
    const http_layer_input_t* const http_layer_input
            = ( const http_layer_input_t* ) input;

    xi_time_t stamp         = 0;
    struct xi_tm* gmtinfo   = 0;

    ENABLE_GENERATOR();

    BEGIN_CORO( *state )

        memset( buffer_32, 0, 32 );
        sprintf( buffer_32, "%"PRIu32, ( uint32_t ) http_layer_input->xi_context->feed_id );
        gen_ptr_text( *state, buffer_32 ); // feed id

        gen_ptr_text( *state, XI_CSV_SLASH );
        gen_ptr_text( *state, XI_CSV_DATASTREAMS );
        gen_ptr_text( *state, XI_CSV_SLASH );

        gen_ptr_text( *state, http_layer_input->http_union_data.xi_delete_datapoint.datastream );

        gen_ptr_text( *state, XI_CSV_SLASH );
        gen_ptr_text( *state, XI_CSV_DATAPOINTS );
        gen_ptr_text( *state, XI_CSV_SLASH );

        {
            stamp   = http_layer_input->http_union_data.xi_delete_datapoint.value->timestamp.timestamp;
            gmtinfo = xi_gmtime( &stamp );

            memset( buffer_32, 0, 32 );
            snprintf( buffer_32, 32
                , XI_CSV_TIMESTAMP_PATTERN
                , gmtinfo->tm_year + 1900
                , gmtinfo->tm_mon + 1
                , gmtinfo->tm_mday
                , gmtinfo->tm_hour
                , gmtinfo->tm_min
                , gmtinfo->tm_sec
                , ( int ) http_layer_input->http_union_data.xi_delete_datapoint.value->timestamp.micro );

            gen_ptr_text( *state, buffer_32 );
            gen_ptr_text( *state, XI_CSV_DOT_CSV_SPACE );
        }

        // SEND HTTP
        gen_ptr_text( *state, XI_HTTP_TEMPLATE_HTTP );
        gen_ptr_text( *state, XI_HTTP_CRLF );

        // SEND THE REST THROUGH SUB GENERATOR
        call_sub_gen_and_exit( *state, input, http_layer_data_generator_datastream_body );

    END_CORO()

    return 0;
}

const void* http_layer_data_generator_datapoint_delete_range(
          const void* input
        , short* state )
{
    // unpack the data
    const http_layer_input_t* const http_layer_input
            = ( const http_layer_input_t* ) input;

    xi_time_t stamp         = 0;
    struct xi_tm* gmtinfo   = 0;

    static const char* const p1 = "?start=";

    ENABLE_GENERATOR();

    BEGIN_CORO( *state )

        memset( buffer_32, 0, 32 );
        sprintf( buffer_32, "%"PRIu32, ( uint32_t ) http_layer_input->xi_context->feed_id );
        gen_ptr_text( *state, buffer_32 ); // feed id

        gen_ptr_text( *state, XI_CSV_SLASH );
        gen_ptr_text( *state, XI_CSV_DATASTREAMS );
        gen_ptr_text( *state, XI_CSV_SLASH );

        gen_ptr_text( *state, http_layer_input->http_union_data.xi_delete_datapoint_range.datastream );

        gen_ptr_text( *state, XI_CSV_SLASH );
        gen_ptr_text( *state, XI_CSV_DATAPOINTS );
        gen_ptr_text( *state, p1 );

        {
            stamp   = http_layer_input->http_union_data.xi_delete_datapoint_range.value_start->timestamp;
            gmtinfo = xi_gmtime( &stamp );

            memset( buffer_32, 0, 32 );
            snprintf( buffer_32, 32
                , XI_CSV_TIMESTAMP_PATTERN
                , gmtinfo->tm_year + 1900
                , gmtinfo->tm_mon + 1
                , gmtinfo->tm_mday
                , gmtinfo->tm_hour
                , gmtinfo->tm_min
                , gmtinfo->tm_sec
                , ( int ) http_layer_input->http_union_data.xi_delete_datapoint_range.value_start->micro );

            gen_ptr_text( *state, buffer_32 );
            gen_ptr_text( *state, XI_HTTP_AMPERSAND );

            stamp   = http_layer_input->http_union_data.xi_delete_datapoint_range.value_end->timestamp;
            gmtinfo = xi_gmtime( &stamp );

            memset( buffer_32, 0, 32 );
            snprintf( buffer_32, 32
                , XI_CSV_TIMESTAMP_PATTERN
                , gmtinfo->tm_year + 1900
                , gmtinfo->tm_mon + 1
                , gmtinfo->tm_mday
                , gmtinfo->tm_hour
                , gmtinfo->tm_min
                , gmtinfo->tm_sec
                , ( int ) http_layer_input->http_union_data.xi_delete_datapoint_range.value_end->micro );

            gen_ptr_text( *state, buffer_32 );
            gen_ptr_text( *state, XI_CSV_DOT_CSV_SPACE );
        }


        //

        // SEND HTTP
        gen_ptr_text( *state, XI_HTTP_TEMPLATE_HTTP );
        gen_ptr_text( *state, XI_HTTP_CRLF );

        // SEND THE REST THROUGH SUB GENERATOR
        call_sub_gen_and_exit( *state, input, http_layer_data_generator_datastream_body );

    END_CORO()

    return 0;
}

static inline layer_state_t http_layer_data_ready_gen(
      layer_connectivity_t* context
    , const http_layer_input_t* input
    , xi_generator_t* gen )
{
    // generator state
    static short gstate         = 0;
    static short local_state    = 0;
    static layer_state_t state  = LAYER_STATE_OK;

    BEGIN_CORO( local_state )

        state = LAYER_STATE_OK;
        gstate = 0;

        // send the data through the next layer
        while( gstate != 1 )
        {
            while( state == LAYER_STATE_OK && gstate != 1 )
            {
                const const_data_descriptor_t* ret
                        = ( const const_data_descriptor_t* ) ( *gen )( input, &gstate );

                state = CALL_ON_PREV_DATA_READY(
                              context->self
                            , ( const void* ) ret
                            , LAYER_HINT_NONE );
            }

            if( state != LAYER_STATE_OK )
            {
                if (state == LAYER_STATE_ERROR) {
                    EXIT( local_state, LAYER_STATE_ERROR );
                }
                YIELD( local_state, state );
                state = CALL_ON_PREV_DATA_READY(context->self, NULL, LAYER_HINT_NONE);
            }
        }

        EXIT( local_state, LAYER_STATE_OK );

    END_CORO()
}

layer_state_t http_layer_data_ready(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    XI_UNUSED( context );
    XI_UNUSED( data );
    XI_UNUSED( hint );
    //xi_debug_function_entered();

    // unpack the data
    const http_layer_input_t* http_layer_input = ( const http_layer_input_t* ) data;

    switch( http_layer_input->query_type )
    {
        case HTTP_LAYER_INPUT_DATASTREAM_UPDATE:
            return http_layer_data_ready_gen(
                          context
                        , http_layer_input
                        , &http_layer_data_generator_datastream_update );
        case HTTP_LAYER_INPUT_DATASTREAM_GET:
            return http_layer_data_ready_gen(
                          context
                        , http_layer_input
                        , &http_layer_data_generator_datastream_get );
        case HTTP_LAYER_INPUT_DATASTREAM_CREATE:
            return http_layer_data_ready_gen(
                          context
                        , http_layer_input
                        , &http_layer_data_generator_datastream_create );
        case HTTP_LAYER_INPUT_FEED_GET:
            return http_layer_data_ready_gen(
                          context
                        , http_layer_input
                        , &http_layer_data_generator_feed_get );
        case HTTP_LAYER_INPUT_FEED_GET_ALL:
            return http_layer_data_ready_gen(
                          context
                        , http_layer_input
                        , &http_layer_data_generator_feed_get_all );
        case HTTP_LAYER_INPUT_FEED_UPDATE:
            return http_layer_data_ready_gen(
                          context
                        , http_layer_input
                        , &http_layer_data_generator_feed_update );
        case HTTP_LAYER_INPUT_DATASTREAM_DELETE:
            return http_layer_data_ready_gen(
                          context
                        , http_layer_input
                        , &http_layer_data_generator_datastream_delete );
        case HTTP_LAYER_INPUT_DATAPOINT_DELETE:
            return http_layer_data_ready_gen(
                          context
                        , http_layer_input
                        , &http_layer_data_generator_datapoint_delete );
        case HTTP_LAYER_INPUT_DATAPOINT_DELETE_RANGE:
            return http_layer_data_ready_gen(
                          context
                        , http_layer_input
                        , &http_layer_data_generator_datapoint_delete_range );
        default:
            return LAYER_STATE_ERROR;
    };
}

layer_state_t http_layer_on_data_ready(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    XI_UNUSED( hint );
    static uint16_t cs = 0; // coroutine state

    // unpack http_layer_data so unpack it
    http_layer_data_t* http_layer_data = ( http_layer_data_t* ) context->self->user_data;

    // some tmp variables
    short sscanf_state      = 0;
    unsigned short prev_pos = 0;
    layer_state_t state     = LAYER_STATE_OK;
    unsigned short before   = 0;
    unsigned short after    = 0;

    xi_stated_sscanf_state_t tmp_state;
    memset( &tmp_state, 0, sizeof( xi_stated_sscanf_state_t ) );

    xi_stated_sscanf_state_t* xi_stated_state = &http_layer_data->xi_stated_sscanf_state;

    // patterns
    const char status_pattern1[]       = "HTTP/1.1 %d %" XI_STR( XI_HTTP_STATUS_STRING_SIZE ) ".\r\n";
    const const_data_descriptor_t v1   = { status_pattern1, sizeof( status_pattern1 ) - 1, sizeof( status_pattern1 ) - 1, 0 };
    void* pv1[]                        = { ( void* ) &( http_layer_data->response->http.http_status ), ( void* ) http_layer_data->response->http.http_status_string };

    const char status_pattern2[]       = "%" XI_STR( XI_HTTP_HEADER_NAME_MAX_SIZE ) "s: %" XI_STR( XI_HTTP_HEADER_VALUE_MAX_SIZE ) ".\r\n";
    const const_data_descriptor_t v2   = { status_pattern2, sizeof( status_pattern2 ) - 1, sizeof( status_pattern2 ) - 1, 0 };
    void*                  pv2[]       =
    {
          ( void* ) http_layer_data->response->http.http_headers[ XI_HTTP_HEADER_UNKNOWN ].name
        , ( void* ) http_layer_data->response->http.http_headers[ XI_HTTP_HEADER_UNKNOWN ].value
    };

    const char status_pattern3[]         = "%d";
    const const_data_descriptor_t v3     = { status_pattern3, sizeof( status_pattern3 ) - 1, sizeof( status_pattern3 ) - 1, 0 };
    const_data_descriptor_t data3        =
    {
          http_layer_data->response->http.http_headers[ XI_HTTP_HEADER_UNKNOWN ].value
        , XI_HTTP_HEADER_VALUE_MAX_SIZE
        , XI_HTTP_HEADER_VALUE_MAX_SIZE
        , 0
    };
    void*                   pv3[]        = { ( void* ) &http_layer_data->content_length };

    const char status_pattern4[]       = "\r\n";
    const const_data_descriptor_t v4   = { status_pattern4, sizeof( status_pattern4 ) - 1, sizeof( status_pattern4 ) - 1, 0 };

    const char status_pattern5[]       = "%" XI_STR( XI_HTTP_HEADER_VALUE_MAX_SIZE ) "B";
    const const_data_descriptor_t v5   = { status_pattern5, sizeof( status_pattern5 ) - 1, sizeof( status_pattern5 ) - 1, 0 };
    void*                  pv5[]       =
    {
        ( void* ) http_layer_data->response->http.http_headers[ XI_HTTP_HEADER_UNKNOWN ].value
    };


    BEGIN_CORO( cs )

    memset( xi_stated_state, 0, sizeof( xi_stated_sscanf_state_t ) );

    // STAGE 01 find the http status
    {
        //
        {
            sscanf_state                          = 0;

            while( sscanf_state == 0 )
            {
                sscanf_state = xi_stated_sscanf(
                              xi_stated_state
                            , ( const_data_descriptor_t* ) &v1
                            , ( const_data_descriptor_t* ) data
                            , pv1 );

                if( sscanf_state == 0 )
                {
                    YIELD( cs, LAYER_STATE_WANT_READ )
                    continue;
                }
            }

            if( sscanf_state == -1 )
            {
                EXIT( cs, LAYER_STATE_ERROR )
            }
        }
    }

    //
    xi_debug_format( "HTTP STATUS: %d", http_layer_data->response->http.http_status );

    // STAGE 02 reading headers
    {
        do
        {
            sscanf_state = 0;

            while( sscanf_state == 0 )
            {
                // save the curr pos to be able to get back
                prev_pos = ( ( const_data_descriptor_t* ) data )->curr_pos;

                sscanf_state = xi_stated_sscanf(
                              xi_stated_state
                            , ( const_data_descriptor_t* ) &v2
                            , ( const_data_descriptor_t* ) data
                            , pv2 );

                if( sscanf_state == 0 )
                {
                    YIELD( cs, LAYER_STATE_WANT_READ )
                    continue;
                }
            }

            if( sscanf_state == 1 )
            {
                xi_debug_format( "%s: %s"
                                 , http_layer_data->response->http.http_headers[ XI_HTTP_HEADER_UNKNOWN ].name
                                 , http_layer_data->response->http.http_headers[ XI_HTTP_HEADER_UNKNOWN ].value );

                http_header_type_t header_type = classify_header( http_layer_data->response->http.http_headers[ XI_HTTP_HEADER_UNKNOWN ].name );
                //__xi_printf("%s:%d\n", http_layer_data->response->http.http_headers[ XI_HTTP_HEADER_UNKNOWN ].name, header_type);

                if( header_type == XI_HTTP_HEADER_CONTENT_LENGTH )
                {
                    sscanf_state = xi_stated_sscanf( &tmp_state, &v3, &data3, pv3 );

                    if( sscanf_state == -1 )
                    {
                        EXIT( cs, LAYER_STATE_ERROR );
                    }
                }

                if( header_type != XI_HTTP_HEADER_UNKNOWN )
                {
                    memcpy( http_layer_data->response->http.http_headers[ header_type ].name
                          , http_layer_data->response->http.http_headers[ XI_HTTP_HEADER_UNKNOWN ].name
                          , XI_HTTP_HEADER_NAME_MAX_SIZE );

                    memcpy( http_layer_data->response->http.http_headers[ header_type ].value
                          , http_layer_data->response->http.http_headers[ XI_HTTP_HEADER_UNKNOWN ].value
                          , XI_HTTP_HEADER_VALUE_MAX_SIZE );
                }

                http_layer_data->response->http.http_headers_checklist[ header_type ]
                        = &http_layer_data->response->http.http_headers[ header_type ];
            }

        } while( sscanf_state == 1 );
    }

    if( sscanf_state == -1 ) // may not be failure yet
    {
        // restore the read position
        ( ( const_data_descriptor_t* ) data )->curr_pos = prev_pos;
    }

    // STAGE 03 reading second \r\n that means that the payload should begin just right after
    {
        sscanf_state                = 0;

        while( sscanf_state == 0 )
        {

            sscanf_state = xi_stated_sscanf(
                          xi_stated_state
                        , ( const_data_descriptor_t* ) &v4
                        , ( const_data_descriptor_t* ) data
                        , 0 );

            if( sscanf_state == 0 )
            {
                YIELD( cs, LAYER_STATE_WANT_READ )
                continue;
            }
        }
    }

    if( sscanf_state == -1 )
    {
        xi_debug_logger( "No double \\r\\n" );

        EXIT( cs, LAYER_STATE_ERROR )
    }

    // STAGE 04 reading payload
    {
        // clear the buffer
        xi_debug_logger( "" );

        http_layer_data->counter    = 0;
        sscanf_state                = 0;
        //__xi_printf("content_length:%d\n", http_layer_data->content_length);

        while( http_layer_data->counter < http_layer_data->content_length )
        {
            before = ( ( const_data_descriptor_t* ) data )->curr_pos;
            after  = ( ( const_data_descriptor_t* ) data )->real_size;
            http_layer_data->counter += after - before;

            xi_debug_printf( "%s", ( ( const_data_descriptor_t* ) data )->data_ptr + ( ( const_data_descriptor_t* ) data )->curr_pos );

            if( http_layer_data->response->http.http_status == 200 )
            {
                state = CALL_ON_NEXT_ON_DATA_READY( context->self
                                            , ( const void* ) data
                                            , ( http_layer_data->counter < http_layer_data->content_length ) ? LAYER_HINT_MORE_DATA : LAYER_HINT_NONE );

                if( state == LAYER_STATE_WANT_READ && http_layer_data->counter < http_layer_data->content_length )
                {
                    YIELD( cs, LAYER_STATE_WANT_READ );
                }
            }
            else
            {
                sscanf_state = 0;

                sscanf_state = xi_stated_sscanf(
                              xi_stated_state
                            , ( const_data_descriptor_t* ) &v5
                            , ( const_data_descriptor_t* ) data
                            , pv5 );

                if( sscanf_state == 0 && http_layer_data->counter < http_layer_data->content_length )
                {
                    YIELD( cs, LAYER_STATE_WANT_READ );
                }
            }
        }
    }

    if( sscanf_state == -1 )
    {
        EXIT( cs, LAYER_STATE_ERROR )
    }

    EXIT( cs, LAYER_STATE_OK )

    END_CORO()
}


layer_state_t http_layer_close(
    layer_connectivity_t* context )
{
    return CALL_ON_PREV_CLOSE( context->self );
}

layer_state_t http_layer_on_close(
    layer_connectivity_t* context )
{
    return  CALL_ON_NEXT_ON_CLOSE( context->self );
}

layer_state_t init_http_layer(
      layer_t* layer )
{
    XI_UNUSED( layer );

    return LAYER_STATE_OK;
}

#ifdef __cplusplus
}
#endif
