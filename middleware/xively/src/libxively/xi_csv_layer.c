// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

#include "xi_csv_layer.h"
#include "xively.h"

#include "xi_macros.h"
#include "xi_debug.h"
#include "xi_coroutine.h"
#include "xi_generator.h"
#include "xi_stated_csv_decode_value_state.h"
#include "xi_stated_sscanf.h"
#include "xi_http_layer_constants.h"
#include "xi_csv_layer_data.h"
#include "xi_layer_api.h"
#include "xi_http_layer_input.h"
#include "xi_layer_helpers.h"

#ifdef __cplusplus
extern "C" {
#endif

inline static int csv_encode_value(
      char* buffer
    , size_t buffer_size
    , const xi_datapoint_t* p )
{
    // PRECONDITION
    assert( buffer != 0 );
    assert( buffer_size != 0 );
    assert( p != 0 );

    switch( p->value_type )
    {
        case XI_VALUE_TYPE_I32:
            return snprintf( buffer, buffer_size, "%"PRIi32, p->value.i32_value );
        case XI_VALUE_TYPE_F32:
            return snprintf( buffer, buffer_size, "%f", p->value.f32_value );
        case XI_VALUE_TYPE_STR:
            return snprintf( buffer, buffer_size, "%s", p->value.str_value );
        default:
            return -1;
    }
}

typedef enum
{
    XI_STATE_INITIAL = 0,
    XI_STATE_MINUS,
    XI_STATE_NUMBER,
    XI_STATE_FLOAT,
    XI_STATE_DOT,
    XI_STATE_STRING,
    XI_STATES_NO
} xi_dfa_state_t;

typedef enum
{
    XI_CHAR_UNKNOWN = 0,
    XI_CHAR_NUMBER,
    XI_CHAR_LETTER,
    XI_CHAR_DOT,
    XI_CHAR_SPACE,
    XI_CHAR_NEWLINE,
    XI_CHAR_TAB,
    XI_CHAR_MINUS,
    XI_CHARS_NO
} xi_char_type_t;

inline static xi_char_type_t csv_classify_char( char c )
{
    switch( c )
    {
        case 13:
        case 11:
            return XI_CHAR_NEWLINE;
        case 9:
            return XI_CHAR_TAB;
        case 32:
            return XI_CHAR_SPACE;
        case 33: case 34: case 35: case 36: case 37: case 38: case 39:
        case 40: case 41: case 42: case 43: case 44:
            return XI_CHAR_UNKNOWN;
        case 45:
            return XI_CHAR_MINUS;
        case 46:
            return XI_CHAR_DOT;
        case 47:
            return XI_CHAR_UNKNOWN;
        case 48: case 49: case 50: case 51: case 52: case 53: case 54:
        case 55: case 56:
        case 57:
            return XI_CHAR_NUMBER;
        case 58: case 59: case 60: case 61: case 62: case 63:
        case 64:
            return XI_CHAR_UNKNOWN;
        case 65: case 66: case 67: case 68: case 69: case 70: case 71:
        case 72: case 73: case 74: case 75: case 76: case 77: case 78:
        case 79: case 80: case 81: case 82: case 83: case 84: case 85:
        case 86: case 87: case 88: case 89:
        case 90:
            return XI_CHAR_LETTER;
        case 91: case 92: case 93: case 94: case 95:
        case 96:
            return XI_CHAR_UNKNOWN;
        case 97: case 98: case 99: case 100: case 101: case 102: case 103:
        case 104: case 105: case 106: case 107: case 108: case 109: case 110:
        case 111: case 112: case 113: case 114: case 115: case 116: case 117:
        case 118: case 119: case 120: case 121:
        case 122:
            return XI_CHAR_LETTER;
        case 123:
        case 124:
        case 125:
            return XI_CHAR_UNKNOWN;
        default:
            return XI_CHAR_UNKNOWN;
    }
}

// the transition function
static const short states[][6][2] =
{
      // state initial                          // state minus                            // state number                           // state float                            // state dot                              // string
    { { XI_CHAR_UNKNOWN   , XI_STATE_STRING  }, { XI_CHAR_UNKNOWN   , XI_STATE_STRING  }, { XI_CHAR_UNKNOWN   , XI_STATE_STRING  }, { XI_CHAR_UNKNOWN   , XI_STATE_STRING  }, { XI_CHAR_UNKNOWN   , XI_STATE_STRING  }, { XI_CHAR_UNKNOWN   , XI_STATE_STRING  } },
    { { XI_CHAR_NUMBER    , XI_STATE_NUMBER  }, { XI_CHAR_NUMBER    , XI_STATE_NUMBER  }, { XI_CHAR_NUMBER    , XI_STATE_NUMBER  }, { XI_CHAR_NUMBER    , XI_STATE_FLOAT   }, { XI_CHAR_NUMBER    , XI_STATE_FLOAT   }, { XI_CHAR_NUMBER    , XI_STATE_STRING  } },
    { { XI_CHAR_LETTER    , XI_STATE_STRING  }, { XI_CHAR_LETTER    , XI_STATE_STRING  }, { XI_CHAR_LETTER    , XI_STATE_STRING  }, { XI_CHAR_LETTER    , XI_STATE_STRING  }, { XI_CHAR_LETTER    , XI_STATE_STRING  }, { XI_CHAR_LETTER    , XI_STATE_STRING  } },
    { { XI_CHAR_DOT       , XI_STATE_DOT     }, { XI_CHAR_DOT       , XI_STATE_DOT     }, { XI_CHAR_DOT       , XI_STATE_DOT     }, { XI_CHAR_DOT       , XI_STATE_STRING  }, { XI_CHAR_DOT       , XI_STATE_STRING  }, { XI_CHAR_DOT       , XI_STATE_STRING  } },
    { { XI_CHAR_SPACE     , XI_STATE_STRING  }, { XI_CHAR_SPACE     , XI_STATE_STRING  }, { XI_CHAR_SPACE     , XI_STATE_STRING  }, { XI_CHAR_SPACE     , XI_STATE_STRING  }, { XI_CHAR_SPACE     , XI_STATE_STRING  }, { XI_CHAR_SPACE     , XI_STATE_STRING  } },
    { { XI_CHAR_NEWLINE   , XI_STATE_INITIAL }, { XI_CHAR_NEWLINE   , XI_STATE_INITIAL }, { XI_CHAR_NEWLINE   , XI_STATE_INITIAL }, { XI_CHAR_NEWLINE   , XI_STATE_INITIAL }, { XI_CHAR_NEWLINE   , XI_STATE_INITIAL }, { XI_CHAR_NEWLINE   , XI_STATE_INITIAL } },
    { { XI_CHAR_TAB       , XI_STATE_STRING  }, { XI_CHAR_TAB       , XI_STATE_STRING  }, { XI_CHAR_TAB       , XI_STATE_STRING  }, { XI_CHAR_TAB       , XI_STATE_STRING  }, { XI_CHAR_TAB       , XI_STATE_STRING  }, { XI_CHAR_TAB       , XI_STATE_STRING  } },
    { { XI_CHAR_MINUS     , XI_STATE_MINUS   }, { XI_CHAR_MINUS     , XI_STATE_STRING  }, { XI_CHAR_MINUS     , XI_STATE_STRING  }, { XI_CHAR_MINUS     , XI_STATE_STRING  }, { XI_CHAR_MINUS     , XI_STATE_STRING  }, { XI_CHAR_MINUS     , XI_STATE_STRING  } }
};

signed char xi_stated_csv_decode_value(
          xi_stated_csv_decode_value_state_t* st
        , const_data_descriptor_t* source
        , xi_datapoint_t* p
        , layer_hint_t hint )
{
    // unused
    XI_UNUSED( hint );

    // PRECONDITION
    assert( st != 0 );
    assert( source != 0 );
    assert( p != 0 );

    // tmp
    char c = 0;
    xi_char_type_t ct = XI_CHAR_UNKNOWN;

    // if not the first run jump into the proper label
    if( st->state != XI_STATE_INITIAL )
    {
        goto data_ready;
    }

    // secure the output buffer
    XI_GUARD_EOS( p->value.str_value, XI_VALUE_STRING_MAX_SIZE );

    // clean the counter
    st->counter = 0;

    // check if the buffer needs more data
    if( source->curr_pos == source->real_size )
    {
        return 0;
    }

    c           = source->data_ptr[ source->curr_pos++ ];
    st->state   = XI_STATE_INITIAL;

    // main processing loop
    while( c != '\n' && c !='\0' && c!='\r' )
    {
        if( st->counter >= XI_VALUE_STRING_MAX_SIZE - 1 )
        {
            xi_set_err( XI_DATAPOINT_VALUE_BUFFER_OVERFLOW );
            return 0;
        }

        ct          = csv_classify_char( c );
        st->state   = states[ ct ][ st->state ][ 1 ];

        switch( st->state )
        {
            case XI_STATE_MINUS:
            case XI_STATE_NUMBER:
            case XI_STATE_FLOAT:
            case XI_STATE_DOT:
            case XI_STATE_STRING:
                p->value.str_value[ st->counter ] = c;
                break;
        }

        // this is where we shall need to jump for more data
        if( source->curr_pos == source->real_size )
        {
            if( hint == LAYER_HINT_MORE_DATA )
            {
                // need more data
                return 0;
            }

            if( hint == LAYER_HINT_NONE )
            {
                ++( st->counter );
                break;
            }
data_ready:
            source->curr_pos = 0; // reset the counter
        }

        //
        ++( st->counter );
        c = source->data_ptr[ ( source->curr_pos )++ ];
    }

    // set the guard
    p->value.str_value[ st->counter ] = '\0';

    // update of the state for loose states...
    switch( st->state )
    {
        case XI_STATE_MINUS:
        case XI_STATE_DOT:
        case XI_STATE_INITIAL:
            st->state = XI_STATE_STRING;
            break;
    }

    switch( st->state )
    {
        case XI_STATE_NUMBER:
            p->value.i32_value  = atoi( p->value.str_value );
            p->value_type       = XI_VALUE_TYPE_I32;
            break;
        case XI_STATE_FLOAT:
            p->value.f32_value  = ( float ) atof( p->value.str_value );
            p->value_type       = XI_VALUE_TYPE_F32;
            break;
        case XI_STATE_STRING:
        default:
            p->value_type       = XI_VALUE_TYPE_STR;
    }

    st->state = XI_STATE_INITIAL;

    return 1;
}

const void* csv_layer_data_generator_datapoint(
          const void* input
        , short* state )
{
    // we expect input to be datapoint
    const union http_union_data_t* ld   = ( const union http_union_data_t* ) input;
    const xi_datapoint_t* dp            = ( const xi_datapoint_t* ) ld->xi_get_datastream.value;

    // tmp
    struct xi_tm* gmtinfo               = 0;

    ENABLE_GENERATOR();
    BEGIN_CORO( *state )

        // if there is a timestamp encode it
        if( dp->timestamp.timestamp != 0 )
        {
            gmtinfo = xi_gmtime( &dp->timestamp.timestamp );

            memset( buffer_32, 0, 32 );

            snprintf( buffer_32, 32
                , XI_CSV_TIMESTAMP_PATTERN
                , gmtinfo->tm_year + 1900
                , gmtinfo->tm_mon + 1
                , gmtinfo->tm_mday
                , gmtinfo->tm_hour
                , gmtinfo->tm_min
                , gmtinfo->tm_sec
                , ( int ) dp->timestamp.micro );

            gen_ptr_text( *state, buffer_32 );
            gen_ptr_text( *state, XI_CSV_COMMA );
        }

        // value
        {
            memset( buffer_32, 0, 32 );
            csv_encode_value( buffer_32, 32, dp );
            gen_ptr_text( *state, buffer_32 );
        }

        // end of line
        gen_ptr_text_and_exit( *state, XI_HTTP_NEWLINE );

    END_CORO()
}

const void* csv_layer_data_generator_datastream(
          const void* input
        , short* state )
{
    // we expect input to be datapoint
    const union http_union_data_t* ld   = ( const union http_union_data_t* ) input;

    ENABLE_GENERATOR();
    BEGIN_CORO( *state )

        // SEND DATAPOINT ID
        gen_ptr_text( *state, ld->xi_create_datastream.datastream );
        gen_ptr_text( *state, XI_CSV_COMMA );

        call_sub_gen_and_exit( *state
                               , ld
                               , csv_layer_data_generator_datapoint );

    END_CORO()

    return 0;

}

const void* csv_layer_data_generator_feed(
          const void* input
        , short* state )
{
    // we expect input to be datapoint
    const union http_union_data_t* ld   = ( const union http_union_data_t* ) input;
    const xi_feed_t* feed               = ( const xi_feed_t* ) ld->xi_get_feed.feed;
    static unsigned char i              = 0;                                            // local global index required to be static cause used via the persistent for
                                                                                        //
    static union http_union_data_t tmp_http_data;

    ENABLE_GENERATOR();
    BEGIN_CORO( *state )

        memset( &tmp_http_data, 0, sizeof( union http_union_data_t ) );

        i = 0;

        // SEND DATAPOINTS
        {
            for( ; i < feed->datastream_count; ++i )
            {
                tmp_http_data.xi_get_datastream.datastream  = feed->datastreams[ i ].datastream_id;
                tmp_http_data.xi_get_datastream.value       = ( xi_datapoint_t* ) &feed->datastreams[ i ].datapoints[ 0 ];

                // SEND THE REST THROUGH SUB GENERATOR
                call_sub_gen( *state, &tmp_http_data, csv_layer_data_generator_datastream );
            }

            gen_ptr_text_and_exit( *state, XI_HTTP_EMPTY );
        }

    END_CORO()

    return 0;
}


// parse the timestamp and the value and save it within the proper datastream field
layer_state_t csv_layer_parse_datastream(
        csv_layer_data_t* csv_layer_data
      , const_data_descriptor_t* data
      , const layer_hint_t hint
      , xi_datapoint_t* dp )
{
    XI_UNUSED( hint );
    XI_UNUSED( dp );

    // some tmp variables
    signed char ret_state = 0;
    static struct xi_tm gmtinfo;

    // patterns
    const const_data_descriptor_t pattern1   = { XI_CSV_TIMESTAMP_PATTERN, strlen( XI_CSV_TIMESTAMP_PATTERN ), strlen( XI_CSV_TIMESTAMP_PATTERN ), 0 };
    void* pv1[]                              = {
        ( void* ) &( gmtinfo.tm_year )
        , ( void* ) &( gmtinfo.tm_mon )
        , ( void* ) &( gmtinfo.tm_mday )
        , ( void* ) &( gmtinfo.tm_hour )
        , ( void* ) &( gmtinfo.tm_min )
        , ( void* ) &( gmtinfo.tm_sec )
        , ( void* ) &( dp->timestamp.micro ) };

    const const_data_descriptor_t pattern2   = { ",", 1, 1, 0 };


    BEGIN_CORO( csv_layer_data->datapoint_decode_state )

    memset( &gmtinfo, 0, sizeof( struct xi_tm ) );

    // parse the timestamp
    {

        // read timestamp
        do
        {

            ret_state = xi_stated_sscanf( &( csv_layer_data->stated_sscanf_state ), &pattern1, data, pv1 );

            if( ret_state == 0 )
            {
                // need more data
                YIELD( csv_layer_data->datapoint_decode_state, LAYER_STATE_WANT_READ );
                ret_state = 0;
                continue;
            }

        } while( ret_state == 0 );

        // read comma
        do
        {
            ret_state = xi_stated_sscanf( &( csv_layer_data->stated_sscanf_state ), &pattern2, data, 0 );

            if( ret_state == 0 )
            {
                // need more data
                YIELD( csv_layer_data->datapoint_decode_state, LAYER_STATE_WANT_READ );
                ret_state = 0;
                continue;
            }

        } while( ret_state == 0 );

        // test the result
        if( ret_state == -1 )
        {
            EXIT( csv_layer_data->datapoint_decode_state, LAYER_STATE_ERROR );
        }

        // here it's safe to convert the gmtinfo to timestamp
        gmtinfo.tm_year            -= 1900;
        gmtinfo.tm_mon             -= 1;
        dp->timestamp.timestamp     = xi_mktime( &gmtinfo );
    }


    // parse the value
    {
        do
        {
            ret_state = xi_stated_csv_decode_value( &( csv_layer_data->csv_decode_value_state ), data, dp, hint );

            if( ret_state == 0 )
            {
                YIELD( csv_layer_data->datapoint_decode_state, LAYER_STATE_WANT_READ )
                continue;
            }

        } while( ret_state == 0 );
    }

    // exit the function
    EXIT( csv_layer_data->datapoint_decode_state, ( ret_state == -1 ? LAYER_STATE_ERROR : LAYER_STATE_OK ) );

    END_CORO()

    //return LAYER_STATE_OK;
}

layer_state_t csv_layer_parse_feed(
        csv_layer_data_t* csv_layer_data
      , const_data_descriptor_t* data
      , const layer_hint_t hint
      , xi_feed_t* dp )
{
    XI_UNUSED( hint );
    XI_UNUSED( dp );

    // some tmp variables
    signed char sscanf_state               = 0;
    layer_state_t state                    = LAYER_STATE_OK;

    // patterns
    const char status_pattern1[]        = "%" XI_STR( XI_MAX_DATASTREAM_NAME ) "C,";
    const const_data_descriptor_t v1    = { status_pattern1, sizeof( status_pattern1 ) - 1, sizeof( status_pattern1 ) - 1, 0 };
    void* pv1[1];

    BEGIN_CORO( csv_layer_data->feed_decode_state )

    // clear the count of datastreams that has been read so far
    dp->datastream_count = 0;

    // loop over datastreams
    do
    {
        // we are expecting the patttern with datapoint id
        {
            do
            {
                pv1[ 0 ] = ( void* ) dp->datastreams[ dp->datastream_count ].datastream_id;

                sscanf_state = xi_stated_sscanf(
                              &( csv_layer_data->stated_sscanf_state )
                            , ( const_data_descriptor_t* ) &v1
                            , ( const_data_descriptor_t* ) data
                            , pv1 );

                if( sscanf_state == 0 )
                {
                    YIELD( csv_layer_data->feed_decode_state, LAYER_STATE_WANT_READ );
                    sscanf_state = 0;
                    continue;
                }
                else if( sscanf_state == -1 )
                {
                    EXIT( csv_layer_data->feed_decode_state, LAYER_STATE_ERROR );
                }

            } while( sscanf_state == 0 );
        }

        // after parsing the datastream name it's time for the datastream itself
        {
            //
            do
            {
                state = csv_layer_parse_datastream( csv_layer_data, data, hint, &( dp->datastreams[ dp->datastream_count ].datapoints[ 0 ] ) );

                if( state == LAYER_STATE_WANT_READ )
                {
                    YIELD( csv_layer_data->feed_decode_state, LAYER_STATE_WANT_READ );
                    state = LAYER_STATE_WANT_READ;
                    continue;
                }
                else if( state == LAYER_STATE_ERROR )
                {
                    EXIT( csv_layer_data->feed_decode_state, LAYER_STATE_ERROR );
                }

            } while( state == LAYER_STATE_WANT_READ );

            // update the counter values
            dp->datastreams[ dp->datastream_count ].datapoint_count     = 1;
            dp->datastream_count                                       += 1;
        }
    } while( hint == LAYER_HINT_MORE_DATA ); // continuation condition

    EXIT( csv_layer_data->feed_decode_state, LAYER_STATE_OK );

    END_CORO()

    //return LAYER_STATE_OK;
}

layer_state_t csv_layer_data_ready(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    XI_UNUSED( context );
    XI_UNUSED( data );
    XI_UNUSED( hint );
    //xi_debug_function_entered();

    // unpack the data, changing the constiness to avoid copying cause
    // these layers shares the same data and the generator suppose to be the only
    // field that set is required
    http_layer_input_t* http_layer_input = ( http_layer_input_t* ) ( data );

    // store the layer input in custom data will need that later
    // during the response parsing
    ( ( csv_layer_data_t* ) context->self->user_data )->http_layer_input = ( void* ) http_layer_input;

    switch( http_layer_input->query_type )
    {
        case HTTP_LAYER_INPUT_DATASTREAM_DELETE:
        case HTTP_LAYER_INPUT_DATAPOINT_DELETE:
        case HTTP_LAYER_INPUT_DATAPOINT_DELETE_RANGE:
        case HTTP_LAYER_INPUT_FEED_GET:
        case HTTP_LAYER_INPUT_FEED_GET_ALL:
        case HTTP_LAYER_INPUT_DATASTREAM_GET:
            http_layer_input->payload_generator = 0;
            break;
        case HTTP_LAYER_INPUT_DATASTREAM_UPDATE:
            http_layer_input->payload_generator = &csv_layer_data_generator_datapoint;
            break;
        case HTTP_LAYER_INPUT_DATASTREAM_CREATE:
            http_layer_input->payload_generator = &csv_layer_data_generator_datastream;
            break;
        case HTTP_LAYER_INPUT_FEED_UPDATE:
            http_layer_input->payload_generator = &csv_layer_data_generator_feed;
            break;
        default:
            return LAYER_STATE_ERROR;
    };

    return CALL_ON_PREV_DATA_READY( context->self, ( void* ) http_layer_input, hint );
}

layer_state_t csv_layer_on_data_ready(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    XI_UNUSED( context );
    XI_UNUSED( data );
    XI_UNUSED( hint );

    // unpack the layer data
    csv_layer_data_t* csv_layer_data = ( csv_layer_data_t* ) context->self->user_data;

    //
    switch( csv_layer_data->http_layer_input->query_type )
    {
        case HTTP_LAYER_INPUT_DATASTREAM_GET:
                return csv_layer_parse_datastream(
                              csv_layer_data
                            , ( void* ) data
                            , hint
                            , ( xi_datapoint_t* ) csv_layer_data->http_layer_input->http_union_data.xi_get_datastream.value );
        case HTTP_LAYER_INPUT_FEED_GET_ALL:
        case HTTP_LAYER_INPUT_FEED_GET:
            return csv_layer_parse_feed(
                              csv_layer_data
                            , ( void* ) data, hint
                            , ( xi_feed_t* ) csv_layer_data->http_layer_input->http_union_data.xi_get_feed.feed );
        default:
            break;
    }

    return LAYER_STATE_OK;

    // END_CORO()

    // return LAYER_STATE_OK;
}

layer_state_t csv_layer_close(
    layer_connectivity_t* context )
{
    return CALL_ON_PREV_CLOSE( context->self );
}

layer_state_t csv_layer_on_close(
    layer_connectivity_t* context )
{
    XI_UNUSED( context );

    return LAYER_STATE_OK;
}

#ifdef __cplusplus
}
#endif
