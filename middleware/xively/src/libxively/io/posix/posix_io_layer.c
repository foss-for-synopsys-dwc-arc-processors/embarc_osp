// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

#include <stdio.h>
#if (!defined(XI_IO_LAYER_POSIX_COMPAT)) || (XI_IO_LAYER_POSIX_COMPAT == 0)
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#elif XI_IO_LAYER_POSIX_COMPAT == 1
#define LWIP_COMPAT_SOCKETS 1
#define LWIP_POSIX_SOCKETS_IO_NAMES 1
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#endif
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include "posix_io_layer.h"
#include "posix_data.h"
#include "xi_allocator.h"
#include "xi_err.h"
#include "xi_macros.h"
#include "xi_debug.h"
#include "xi_globals.h"

#include "xi_layer_api.h"
#include "xi_common.h"
#include "xi_connection_data.h"

#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_SO_SNDRCVTIMEO_NONSTANDARD
typedef int SOCKET_TIMEOUT;
#define SOCKET_TIMEOUT_SET(tmout, ms_val)	(*(int *)(tmout) = (ms_val))
#else
typedef struct timeval SOCKET_TIMEOUT;
#define SOCKET_TIMEOUT_SET(tmout, ms_val)	{							\
				((struct timeval *)(tmout))->tv_sec = (long)(ms_val) / 1000; 		\
				((struct timeval *)(tmout))->tv_usec = ((long)(ms_val) % 1000) * 1000;	\
			}
#endif

static SOCKET_TIMEOUT xively_socket_tmout;

/* Set timeout for xively network */
static void posix_io_set_network_timeout(int fd, uint32_t ms)
{
    SOCKET_TIMEOUT_SET(&xively_socket_tmout, ms);
#if LWIP_SO_SNDTIMEO
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)(&xively_socket_tmout), sizeof(SOCKET_TIMEOUT));
#endif
#if LWIP_SO_RCVTIMEO
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)(&xively_socket_tmout), sizeof(SOCKET_TIMEOUT));
#endif
}

layer_state_t posix_io_layer_data_ready(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    posix_data_t* posix_data                = ( posix_data_t* ) context->self->user_data;
    const const_data_descriptor_t* buffer   = ( const const_data_descriptor_t* ) data;

    //xi_debug_printf( "buffer->data_ptr:" );
    xi_debug_printf( "%s", buffer->data_ptr );

    XI_UNUSED( hint );

    if( buffer != 0 && buffer->data_size > 0 )
    {
        int len = write( posix_data->socket_fd, buffer->data_ptr, buffer->data_size );

        if( len == 0 )
        {
            // socket has been closed
            return LAYER_STATE_ERROR;
        }

        if( len < buffer->data_size )
        {
            return LAYER_STATE_ERROR;
        }
    }

    return LAYER_STATE_OK;
}

layer_state_t posix_io_layer_on_data_ready(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    xi_debug_logger( "[posix_io_layer_on_data_ready]" );

    posix_data_t* posix_data = ( posix_data_t* ) context->self->user_data;

    XI_UNUSED( hint );

    data_descriptor_t* buffer = 0;

    if( data )
    {
        buffer = ( data_descriptor_t* ) data;
    }
    else
    {
        static char data_buffer[ 32 ];
        memset( data_buffer, 0, sizeof( data_buffer ) );
        static data_descriptor_t buffer_descriptor = { data_buffer, sizeof( data_buffer ), 0, 0 };
        buffer = &buffer_descriptor;
    }

    layer_state_t state = LAYER_STATE_OK;

    do
    {
        memset( buffer->data_ptr, 0, buffer->data_size );

        int len = read( posix_data->socket_fd, buffer->data_ptr, buffer->data_size - 1 );

        if( len == 0 )
        {
            // socket has been closed
            return LAYER_STATE_ERROR;
        }

        if( len < 0 )
        {
            return LAYER_STATE_ERROR;
        }

        buffer->real_size = len;
        buffer->data_ptr[ buffer->real_size ] = '\0'; // put guard
        buffer->curr_pos = 0;
        state = CALL_ON_NEXT_ON_DATA_READY( context->self, ( void* ) buffer, LAYER_HINT_MORE_DATA );
    } while( state == LAYER_STATE_WANT_READ );
    return LAYER_STATE_OK;
}

layer_state_t posix_io_layer_close( layer_connectivity_t* context )
{
    posix_data_t* posix_data = ( posix_data_t* ) context->self->user_data;

    XI_UNUSED( posix_data );

    return CALL_ON_SELF_ON_CLOSE( context->self );
}

layer_state_t posix_io_layer_on_close( layer_connectivity_t* context )
{
    posix_data_t* posix_data = ( posix_data_t* ) context->self->user_data;

    // shutdown the communication
    if( shutdown( posix_data->socket_fd, SHUT_RDWR ) == -1 )
    {
        xi_set_err( XI_SOCKET_SHUTDOWN_ERROR );
        goto err_handling;
    }

    // close the connection & the socket
    if( close( posix_data->socket_fd ) == -1 )
    {
        xi_set_err( XI_SOCKET_CLOSE_ERROR );
        goto err_handling;
    }

    // cleanup the memory
    if( context->self->user_data )
    {
        xi_debug_logger( "Freeing posix_data memory... \n" );
        XI_SAFE_FREE( context->self->user_data );
    }

    return CALL_ON_NEXT_ON_CLOSE( context->self );

err_handling:
    close( posix_data->socket_fd );

    if( context->self->user_data )
    {
        xi_debug_logger( "Freeing posix_data memory... \n" );
        XI_SAFE_FREE( context->self->user_data );
    }

    CALL_ON_NEXT_ON_CLOSE( context->self );
    return LAYER_STATE_ERROR;
}

// here we are going to allocate the space for the posix data, and we are going to create the socket
// and store it in the data module so it's we can use it later on
layer_state_t posix_io_layer_init(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    XI_UNUSED( hint );
    XI_UNUSED( data );

    // PRECONDITIONS
    assert( context != 0 );

    xi_debug_logger( "[posix_io_layer_init]" );

    layer_t* layer              = ( layer_t* ) context->self;
    posix_data_t* posix_data    = xi_alloc( sizeof( posix_data_t ) );

    XI_CHECK_MEMORY( posix_data );

    layer->user_data            = ( void* ) posix_data;

    xi_debug_logger( "Creating socket..." );

    posix_data->socket_fd       = socket( AF_INET, SOCK_STREAM, 0 );

    if( posix_data->socket_fd == -1 )
    {
        xi_debug_logger( "Socket creation [failed]" );
        xi_set_err( XI_SOCKET_INITIALIZATION_ERROR );
        return 0;
    }

    posix_io_set_network_timeout(posix_data->socket_fd, xi_globals.network_timeout);

    xi_debug_logger( "Socket creation [ok]" );

    // POSTCONDITIONS
    assert( layer->user_data != 0 );
    assert( posix_data->socket_fd != -1 );

    return LAYER_STATE_OK;

err_handling:
    // cleanup the memory
    if( posix_data )        { close( posix_data->socket_fd ); }
    if( layer->user_data )  { XI_SAFE_FREE( layer->user_data ); }

    return LAYER_STATE_ERROR;
}

layer_state_t posix_io_layer_connect( layer_connectivity_t* context, const void* data, const layer_hint_t hint )
{
    XI_UNUSED( hint );

    // PRECONDITIONS
    assert( context != 0 );

    xi_connection_data_t* connection_data   = ( xi_connection_data_t* ) data;
    layer_t* layer                          = ( layer_t* ) context->self;
    posix_data_t* posix_data                = ( posix_data_t* ) layer->user_data;

    xi_debug_format( "Connecting layer [%d] to the endpoint", layer->layer_type_id );

     // socket specific data
    struct sockaddr_in name;
    struct hostent* hostinfo;

    xi_debug_logger( "Getting host by name..." );

    // get the hostaddress
    hostinfo = gethostbyname( connection_data->address );

    // if null it means that the address has not been founded
    if( hostinfo == NULL )
    {
        xi_debug_logger( "Getting Host by name [failed]" );
        xi_set_err( XI_SOCKET_GETHOSTBYNAME_ERROR );
        goto err_handling;
    }

    xi_debug_logger( "Getting Host by name [ok]" );

    // set the address and the port for further connection attempt
    memset( &name, 0, sizeof( struct sockaddr_in ) );
    name.sin_family     = AF_INET;
    name.sin_addr       = *( ( struct in_addr* ) hostinfo->h_addr );
    name.sin_port       = htons( connection_data->port );

    xi_debug_logger( "Connecting to the endpoint..." );

    if( connect( posix_data->socket_fd, ( struct sockaddr* ) &name, sizeof( struct sockaddr ) ) != 0 )
    {
        xi_debug_format( "errno: %d", errno );
        xi_debug_logger( "Connecting to the endpoint [failed]" );
        xi_set_err( XI_SOCKET_CONNECTION_ERROR );
        goto err_handling;
    }

    xi_debug_logger( "Connecting to the endpoint [ok]" );

    return LAYER_STATE_OK;

err_handling:
    // cleanup the memory
    if( posix_data )        { close( posix_data->socket_fd ); }
    if( layer->user_data )  { XI_SAFE_FREE( layer->user_data ); }

    return LAYER_STATE_ERROR;
}

#ifdef __cplusplus
}
#endif
