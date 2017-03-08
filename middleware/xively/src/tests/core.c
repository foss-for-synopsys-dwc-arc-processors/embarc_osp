// Copyright (c) 2003-2013, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.
#include "tinytest.h"
#include "tinytest_macros.h"

#include "xively.h"
#include "xi_err.h"
#include "xi_helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////////////
// HTTP PARSER TESTS
///////////////////////////////////////////////////////////////////////////////

void test_parse_http_status(void* data)
{
    (void)(data);

    tt_assert( 0 == 0 );


    /* Every test-case function needs to finish with an "end:"
       label and (optionally) code to clean up local variables. */
 end:
    xi_set_err( XI_NO_ERR );
    ;
}

void test_parse_http_header(void *data)
{
    (void)(data);

    tt_assert( 0 == 0 );

    /* Every test-case function needs to finish with an "end:"
       label and (optionally) code to clean up local variables. */
 end:
    xi_set_err( XI_NO_ERR );
    ;
}

void test_parse_http(void* data)
{
    (void)(data);

    tt_assert( 0 == 0 );

    /* Every test-case function needs to finish with an "end:"
       label and (optionally) code to clean up local variables. */
 end:
    xi_set_err( XI_NO_ERR );
    ;
}

///////////////////////////////////////////////////////////////////////////////
// HTTP CONSTRUCT TEST
///////////////////////////////////////////////////////////////////////////////

void test_http_construct_request(void *data)
{
    (void)(data);

    tt_assert( 0 == 0 );

 end:
    xi_set_err( XI_NO_ERR );
    ;
}

void test_http_construct_content(void *data)
{
    (void)(data);

    tt_assert( 0 == 0 );

 end:
    xi_set_err( XI_NO_ERR );
    ;
}

///////////////////////////////////////////////////////////////////////////////
// CSV TESTS
///////////////////////////////////////////////////////////////////////////////

void test_csv_decode_datapoint(void *data)
{
    (void)(data);

    tt_assert( 0 == 0 );

    /* Every test-case function needs to finish with an "end:"
       label and (optionally) code to clean up local variables. */
 end:
    xi_set_err( XI_NO_ERR );
    ;
}

void test_csv_decode_datapoint_error( void * data )
{
    (void)(data);

    tt_assert( 0 == 0 );

    /* Every test-case function needs to finish with an "end:"
       label and (optionally) code to clean up local variables. */
 end:
    xi_set_err( XI_NO_ERR );
    ;
}

void test_csv_encode_create_datastream( void* data )
{
    (void)(data);

    tt_assert( 0 == 0 );

    /* Every test-case function needs to finish with an "end:"
       label and (optionally) code to clean up local variables. */
 end:
    xi_set_err( XI_NO_ERR );
    ;
}

void test_csv_encode_create_datastream_error( void* data )
{
    (void)(data);

    tt_assert( 0 == 0 );

    /* Every test-case function needs to finish with an "end:"
       label and (optionally) code to clean up local variables. */
 end:
    xi_set_err( XI_NO_ERR );
    ;
}

void test_csv_encode_datapoint( void* data )
{
    (void)(data);

    tt_assert( 0 == 0 );

    /* Every test-case function needs to finish with an "end:"
       label and (optionally) code to clean up local variables. */
 end:
    xi_set_err( XI_NO_ERR );
    ;
}

void test_helpers_copy_until( void* data )
{
    (void)(data);

    tt_assert( 0 == 0 );

 end:
    ;
}

//decl

void test_helpers_decode_value( void* data )
{
    (void)(data);

    tt_assert( 0 == 0 );

 end:
    ;
}

void test_create_and_delete_context(void* data)
{
  (void)(data);

#define TEST_API_KEY_STRING "abcXYZ2398138172jckajf39djas_dasd-TyX"
#define TEST_FEED_ID_NUMBER 123456

  xi_context_t* xi_context
      = xi_create_context( XI_HTTP
              , TEST_API_KEY_STRING
              , TEST_FEED_ID_NUMBER );

  tt_assert( xi_context != 0 );
  tt_assert( strcmp( xi_context->api_key, TEST_API_KEY_STRING ) == 0 );
  tt_assert( xi_context->feed_id == TEST_FEED_ID_NUMBER );

  xi_delete_context( xi_context );

end:
   xi_set_err( XI_NO_ERR );
   ;
}

void test_datapoint_value_setters_and_getters(void* data)
{
  (void)(data);

  void * ret;

  xi_datapoint_t dp;

  ret = xi_set_value_i32( &dp, 320 );

  tt_assert( ret != 0 );

  tt_assert( xi_get_value_i32( &dp ) == 320 );

  tt_assert( xi_get_value_type( &dp ) == XI_VALUE_TYPE_I32 );

  tt_assert( xi_value_pointer_i32( &dp ) != 0 &&
             xi_value_pointer_f32( &dp ) == 0 &&
             xi_value_pointer_str( &dp ) == 0 );

  ret = xi_set_value_f32( &dp, (320.0/10) );

  tt_assert( ret != 0 );

  tt_assert( xi_get_value_f32( &dp ) == (320.0/10) );

  tt_assert( xi_get_value_type( &dp ) == XI_VALUE_TYPE_F32 );

  tt_assert( xi_value_pointer_i32( &dp ) == 0 &&
             xi_value_pointer_f32( &dp ) != 0 &&
             xi_value_pointer_str( &dp ) == 0 );

  ret = xi_set_value_str( &dp, "Let's test everything!" );

  tt_assert( ret != 0 );

  char s[XI_VALUE_STRING_MAX_SIZE+100];
  for (size_t i = 0; i < sizeof(s); i++) {
    s[i] = 'a';
  }

  ret = xi_set_value_str( &dp, s );

  tt_assert( ret != 0 )

  tt_assert( xi_get_value_type( &dp ) == XI_VALUE_TYPE_STR );

  tt_assert( xi_value_pointer_i32( &dp ) == 0 &&
             xi_value_pointer_f32( &dp ) == 0 &&
             xi_value_pointer_str( &dp ) != 0 );

  char * x = xi_value_pointer_str( &dp );

  tt_assert( strlen(s) > strlen(x) );
  tt_assert( strlen(x) == XI_VALUE_STRING_MAX_SIZE-1 );

  s[XI_VALUE_STRING_MAX_SIZE-5] = '\0';

  ret = xi_set_value_str( &dp, s );

  tt_assert( ret != 0 );

  tt_assert( strlen(s) == strlen(x) );
  tt_assert( strlen(x) == XI_VALUE_STRING_MAX_SIZE-5 );

end:
   ;
}


struct testcase_t core_tests[] = {
    /* Here's a really simple test: it has a name you can refer to it
       with, and a function to invoke it. */
    { "test_parse_http_status", test_parse_http_status, TT_ENABLED_, 0, 0 },
    { "test_parse_http_header", test_parse_http_header, TT_ENABLED_, 0, 0 },
    { "test_parse_http", test_parse_http, TT_ENABLED_, 0, 0 },

    { "test_http_construct_request", test_http_construct_request, TT_ENABLED_, 0, 0 },
    { "test_http_construct_content", test_http_construct_content, TT_ENABLED_, 0, 0 },

    { "test_csv_decode_datapoint", test_csv_decode_datapoint, TT_ENABLED_, 0, 0 },
    { "test_csv_decode_datapoint_error", test_csv_decode_datapoint_error, TT_ENABLED_, 0, 0 },
    { "test_csv_encode_create_datastream", test_csv_encode_create_datastream, TT_ENABLED_, 0, 0 },
    { "test_csv_encode_create_datastream_error", test_csv_encode_create_datastream_error, TT_ENABLED_, 0, 0 },
    { "test_csv_encode_datapoint", test_csv_encode_datapoint, TT_ENABLED_, 0, 0 },

    { "test_helpers_copy_until", test_helpers_copy_until, TT_ENABLED_, 0, 0 },
    { "test_helpers_decode_value", test_helpers_decode_value, TT_ENABLED_, 0, 0 },

    { "test_create_and_delete_context", test_create_and_delete_context, TT_ENABLED_, 0, 0 },
    { "test_datapoint_value_setters_and_getters", test_datapoint_value_setters_and_getters, TT_ENABLED_, 0, 0 },
    /* The array has to end with END_OF_TESTCASES. */
    END_OF_TESTCASES
};

/* Next, we make an array of testgroups.  This is mandatory.  Unlike more
   heavy-duty testing frameworks, groups can't nest. */
struct testgroup_t groups[] = {

    /* Every group has a 'prefix', and an array of tests.  That's it. */
    { "csv/", core_tests },

    END_OF_GROUPS
};

#if XI_UNIT_TEST_NATIVE
int main( int argc, char const *argv[] )
{
  int r = tinytest_main( argc, argv, groups );
  printf( "status: %i\n", r );
  return r;
}
#else
int main() 
{
  const char a[] = {"sim"};
  int r = tinytest_main( 1, (const char **) a, groups );
  printf( "status: %i\n", r);
  return r;
}
#endif
