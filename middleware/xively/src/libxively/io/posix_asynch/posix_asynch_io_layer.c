// c
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>

// local
#include "posix_asynch_io_layer.h"
#include "posix_asynch_data.h"
#include "xi_allocator.h"
#include "xi_err.h"
#include "xi_macros.h"
#include "xi_debug.h"
#include "xi_layer_api.h"
#include "xi_common.h"
#include "xi_connection_data.h"
#include "xi_coroutine.h"

#ifdef __cplusplus
extern "C" {
#endif

layer_state_t posix_asynch_io_layer_data_ready(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    posix_asynch_data_t* posix_asynch_data  = ( posix_asynch_data_t* ) context->self->user_data;
    const const_data_descriptor_t* buffer   = ( const const_data_descriptor_t* ) data;

    XI_UNUSED( hint );

    if( buffer != 0 && buffer->data_size > 0 )
    {
        int len = write( posix_asynch_data->socket_fd, buffer->data_ptr, buffer->data_size );

        if( len < 0 )
        {
            int errval = errno;
            if( errval == EAGAIN ) // that can happen
            {
                return LAYER_STATE_WANT_WRITE;
            }

            xi_debug_printf( "error reading: errno = %d \n", errval );
            return LAYER_STATE_ERROR;
        }

        if( len < buffer->data_size )
        {
            return LAYER_STATE_ERROR;
        }
    }

    return hint == LAYER_HINT_MORE_DATA ? LAYER_STATE_WANT_WRITE : LAYER_STATE_OK;
}

layer_state_t posix_asynch_io_layer_on_data_ready(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    //xi_debug_logger( "[posix_asynch_io_layer_on_data_ready]" );

    posix_asynch_data_t* posix_asynch_data = ( posix_asynch_data_t* ) context->self->user_data;

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

    memset( buffer->data_ptr, 0, buffer->data_size );
    int len = read( posix_asynch_data->socket_fd, buffer->data_ptr, buffer->data_size - 1 );

    if( len < 0 )
    {
        int errval = errno;
        if( errval == EAGAIN ) // that can happen
        {
            return LAYER_STATE_WANT_READ;
        }

        xi_debug_printf( "error reading: errno = %d \n", errval );
        return LAYER_STATE_ERROR;
    }

    buffer->real_size = len;

    buffer->data_ptr[ buffer->real_size ] = '\0'; // put guard
    buffer->curr_pos = 0;
    state = CALL_ON_NEXT_ON_DATA_READY( context->self, ( void* ) buffer, LAYER_HINT_MORE_DATA );

    return state;
}

layer_state_t posix_asynch_io_layer_close( layer_connectivity_t* context )
{
    posix_asynch_data_t* posix_asynch_data = ( posix_asynch_data_t* ) context->self->user_data;

    XI_UNUSED( posix_asynch_data );

    return LAYER_STATE_OK;
}

layer_state_t posix_asynch_io_layer_on_close( layer_connectivity_t* context )
{
    // prepare return value
    layer_state_t ret = LAYER_STATE_OK;

    //
    posix_asynch_data_t* posix_asynch_data = ( posix_asynch_data_t* ) context->self->user_data;

    if( shutdown( posix_asynch_data->socket_fd, SHUT_RDWR ) == -1 )
    {
        xi_set_err( XI_SOCKET_SHUTDOWN_ERROR );
        close( posix_asynch_data->socket_fd ); // just in case
        ret = LAYER_STATE_ERROR;
        goto err_handling;
    }

    // close the connection & the socket
    if( close( posix_asynch_data->socket_fd ) == -1 )
    {
        xi_set_err( XI_SOCKET_CLOSE_ERROR );
        ret = LAYER_STATE_ERROR;
        goto err_handling;
    }

err_handling:
    // cleanup the memory
    if( posix_asynch_data ) { XI_SAFE_FREE( posix_asynch_data ); }

    return ret;
}

layer_state_t posix_asynch_io_layer_init(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    XI_UNUSED( hint );
    XI_UNUSED( data );

    // PRECONDITIONS
    assert( context != 0 );

    xi_debug_logger( "[posix_io_layer_init]" );

    layer_t* layer                              = ( layer_t* ) context->self;
    posix_asynch_data_t* posix_asynch_data      = xi_alloc( sizeof( posix_asynch_data_t ) );

    XI_CHECK_MEMORY( posix_asynch_data );

    layer->user_data                            = ( void* ) posix_asynch_data;

    xi_debug_logger( "Creating socket..." );

    posix_asynch_data->socket_fd                = socket( AF_INET, SOCK_STREAM, 0 );

    if( posix_asynch_data->socket_fd == -1 )
    {
        xi_debug_logger( "Socket creation [failed]" );
        xi_set_err( XI_SOCKET_INITIALIZATION_ERROR );
        return 0;
    }

    xi_debug_logger( "Setting socket non blocking behaviour..." );

    int flags = fcntl( posix_asynch_data->socket_fd, F_GETFL, 0 );

    if( flags == -1 )
    {
        xi_debug_logger( "Socket non blocking behaviour [failed]" );
        xi_set_err( XI_SOCKET_INITIALIZATION_ERROR );
        goto err_handling;
    }

    if( fcntl( posix_asynch_data->socket_fd, F_SETFL, flags | O_NONBLOCK ) == -1 )
    {
        xi_debug_logger( "Socket non blocking behaviour [failed]" );
        xi_set_err( XI_SOCKET_INITIALIZATION_ERROR );
        goto err_handling;
    }

    xi_debug_logger( "Socket creation [ok]" );

    // POSTCONDITIONS
    assert( layer->user_data != 0 );
    assert( posix_asynch_data->socket_fd != -1 );

    return LAYER_STATE_OK;

err_handling:
    // cleanup the memory
    if( posix_asynch_data )     { close( posix_asynch_data->socket_fd ); }
    if( layer->user_data )      { XI_SAFE_FREE( layer->user_data ); }

    return LAYER_STATE_ERROR;
}

layer_state_t posix_asynch_io_layer_connect(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    XI_UNUSED( hint );

    static uint16_t cs = 0; // local coroutine prereserved state

    xi_connection_data_t* connection_data   = ( xi_connection_data_t* ) data;
    layer_t* layer                          = ( layer_t* ) context->self;
    posix_asynch_data_t* posix_asynch_data  = ( posix_asynch_data_t* ) layer->user_data;

    BEGIN_CORO( cs )

    xi_debug_format( "Connecting layer [%d] to the endpoint", layer->layer_type_id );

    // socket specific data
    struct sockaddr_in name;
    struct hostent* hostinfo;

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

    if( connect( posix_asynch_data->socket_fd, ( struct sockaddr* ) &name, sizeof( struct sockaddr ) ) == -1 )
    {
        if( errno != EINPROGRESS )
        {
            xi_debug_printf( "errno: %d", errno );
            xi_debug_logger( "Connecting to the endpoint [failed]" );
            xi_set_err( XI_SOCKET_CONNECTION_ERROR );
            goto err_handling;
        }
        else
        {
            YIELD( cs, LAYER_STATE_WANT_WRITE ); // return here whenever we can write
        }
    }

    EXIT( cs, LAYER_STATE_OK );

err_handling:
    // cleanup the memory
    if( posix_asynch_data )     { close( posix_asynch_data->socket_fd ); }
    if( layer->user_data )      { XI_SAFE_FREE( layer->user_data ); }

    EXIT( cs, LAYER_STATE_ERROR );

    END_CORO()
}

#ifdef __cplusplus
}
#endif
