// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

#include "xi_err.h"
#include "xi_macros.h"
#include "xi_config.h"

#ifdef __cplusplus
extern "C" {
#endif

static xi_err_t xi_err = XI_NO_ERR;

#ifndef XI_OPT_NO_ERROR_STRINGS
const char* xi_err_string[ XI_ERR_COUNT ] =
{
          "XI_NO_ERR"
        , "XI_OUT_OF_MEMORY"                           // XI_OUT_OF_MEMORY
        , "XI_HTTP_STATUS_PARSE_ERROR"                 // XI_HTTP_STATUS_PARSE_ERROR
        , "XI_HTTP_HEADER_PARSE_ERROR"                 // XI_HTTP_HEADER_PARSE_ERROR
        , "XI_HTTP_PARSE_ERROR"                        // XI_HTTP_PARSE_ERROR
        , "XI_HTTP_ENCODE_CREATE_DATASTREAM"           // XI_HTTP_ENCODE_CREATE_DATASTREAM
        , "XI_HTTP_ENCODE_UPDATE_DATASTREAM"           // XI_HTTP_ENCODE_UPDATE_DATASTREAM
        , "XI_HTTP_ENCODE_GET_DATASTREAM"              // XI_HTTP_ENCODE_GET_DATASTREAM
        , "XI_HTTP_ENCODE_DELETE_DATASTREAM"           // XI_HTTP_ENCODE_DELETE_DATASTREAM
        , "XI_HTTP_ENCODE_DELETE_DATAPOINT"            // XI_HTTP_ENCODE_DELETE_DATAPOINT
        , "XI_HTTP_ENCODE_DELETE_RANGE_DATAPOINT"      // XI_HTTP_ENCODE_DELETE_RANGE_DATAPOINT
        , "XI_HTTP_ENCODE_UPDATE_FEED"                 // XI_HTTP_ENCODE_UPDATE_FEED
        , "XI_HTTP_CONSTRUCT_REQUEST_BUFFER_OVERRUN"   // XI_HTTP_CONSTRUCT_REQUEST_BUFFER_OVERRUN
        , "XI_HTTP_CONSTRUCT_CONTENT_BUFFER_OVERRUN"   // XI_HTTP_CONSTRUCT_CONTENT_BUFFER_OVERRUN
        , "XI_CSV_ENCODE_DATAPOINT_BUFFER_OVERRUN"     // XI_CSV_ENCODE_DATAPOINT_BUFFER_OVERRUN
        , "XI_CSV_ENCODE_DATASTREAM_BUFFER_OVERRUN"    // XI_CSV_ENCODE_DATASTREAM_BUFFER_OVERRUN
        , "XI_CSV_DECODE_FEED_PARSER_ERROR"            // XI_CSV_DECODE_FEED_PARSER_ERROR
        , "XI_CSV_DECODE_DATAPOINT_PARSER_ERROR"       // XI_CSV_DECODE_DATAPOINT_PARSER_ERROR
        , "XI_CSV_TIME_CONVERTION_ERROR"               // XI_CSV_TIME_CONVERTION_ERROR
        , "XI_SOCKET_INITIALIZATION_ERROR"             // XI_SOCKET_INITIALIZATION_ERROR
        , "XI_SOCKET_GETHOSTBYNAME_ERROR"              // XI_SOCKET_GETHOSTBYNAME_ERROR
        , "XI_SOCKET_CONNECTION_ERROR"                 // XI_SOCKET_CONNECTION_ERROR
        , "XI_SOCKET_SHUTDOWN_ERROR"                   // XI_SOCKET_SHUTDOWN_ERROR
        , "XI_SOCKET_WRITE_ERROR"                      // XI_SOCKET_WRITE_ERROR
        , "XI_SOCKET_READ_ERROR"                       // XI_SOCKET_READ_ERROR
        , "XI_SOCKET_CLOSE_ERROR"                      // XI_SOCKET_CLOSE_ERROR
        , "XI_DATAPOINT_VALUE_BUFFER_OVERFLOW"         // XI_DATAPOINT_VALUE_BUFFER_OVERFLOW
};
#endif /* XI_OPT_NO_ERROR_STRINGS */

const char* xi_get_error_string( xi_err_t e )
{
#ifdef XI_OPT_NO_ERROR_STRINGS
    return ( const char* ) &e;
#else
    return xi_err_string[ XI_MIN( ( short ) e, XI_ERR_COUNT - 1 ) ];
#endif
}

xi_err_t xi_get_last_error()
{
    xi_err_t ret = xi_err;
    xi_set_err( XI_NO_ERR );
    return ret;
}

void xi_set_err( xi_err_t e )
{
    xi_err = e;
}

#ifdef __cplusplus
}
#endif
