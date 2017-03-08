// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

#include "xi_http_layer_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

const char* const XI_HTTP_POST                    = "POST ";
const char* const XI_HTTP_GET                     = "GET ";
const char* const XI_HTTP_PUT                     = "PUT ";
const char* const XI_HTTP_DELETE                  = "DELETE ";
const char* const XI_HTTP_CRLF                    = "\r\n";
const char* const XI_HTTP_TEMPLATE_FEED           = "/v2/feeds";
const char* const XI_HTTP_TEMPLATE_CSV            = ".csv";
const char* const XI_HTTP_TEMPLATE_HTTP           = "HTTP/1.1";
const char* const XI_HTTP_TEMPLATE_HOST           = "Host: ";
const char* const XI_HTTP_TEMPLATE_USER_AGENT     = "User-Agent: ";
const char* const XI_HTTP_TEMPLATE_X_API_KEY      = "X-ApiKey: ";
const char* const XI_HTTP_TEMPLATE_ACCEPT         = "Accept: */*";
const char* const XI_HTTP_CONTENT_LENGTH          = "Content-Length: ";
const char* const XI_CSV_TIMESTAMP_PATTERN        = "%04d-%02d-%02dT%02d:%02d:%02d.%06dZ";
const char* const XI_CSV_SLASH                    = "/";
const char* const XI_CSV_COMMA                    = ",";
const char* const XI_CSV_DATASTREAMS              = "datastreams";
const char* const XI_CSV_DATAPOINTS               = "datapoints";
const char* const XI_CSV_DOT_CSV_SPACE            = ".csv "; 
const char* const XI_HTTP_AMPERSAND               = "&";
const char* const XI_HTTP_SPACE                   = " ";
const char* const XI_HTTP_NEWLINE                 = "\n";
const char* const XI_HTTP_EMPTY                   = "";

char buffer_32[ 32 ];

#ifdef __cplusplus
}
#endif
