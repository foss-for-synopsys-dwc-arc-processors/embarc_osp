// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "mbed_io_layer.h"
#include "mbed_data.h"

#include "xi_helpers.h"
#include "xi_allocator.h"
#include "xi_debug.h"
#include "xi_err.h"
#include "xi_macros.h"
#include "xi_globals.h"
#include "xi_layer_api.h"
#include "xi_common.h"

extern "C" {

layer_state_t mbed_io_layer_data_ready(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    // extract the layer specific data
    mbed_data_t* mbed_data                  = ( mbed_data_t* ) context->self->user_data;
    const const_data_descriptor_t* buffer   = ( const const_data_descriptor_t* ) data;

    if( buffer != 0 && buffer->data_size > 0 )
    {
        xi_debug_format( "sending: [%s]", buffer->data_ptr );
        int len = mbed_data->socket_ptr->send_all( ( char* ) buffer->data_ptr, buffer->data_size );
        xi_debug_format( "sent: %d bytes", len );

        if( len < buffer->data_size )
        {
            xi_set_err( XI_SOCKET_WRITE_ERROR );
            return LAYER_STATE_ERROR;
        }
    }

    return LAYER_STATE_OK;
}

layer_state_t mbed_io_layer_on_data_ready(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    // extract the layer specific data
    mbed_data_t* mbed_data  = ( mbed_data_t* ) context->self->user_data;

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
        buffer->real_size = mbed_data->socket_ptr->receive( buffer->data_ptr, buffer->data_size - 1 );

        xi_debug_format( "received: %d", buffer->real_size );

        buffer->data_ptr[ buffer->real_size ] = '\0'; // put guard
        buffer->curr_pos = 0;
        state = CALL_ON_NEXT_ON_DATA_READY( context->self, ( void* ) buffer, LAYER_HINT_MORE_DATA );
    } while( state == LAYER_STATE_WANT_READ );

    return LAYER_STATE_OK;
}

layer_state_t mbed_io_layer_close(
    layer_connectivity_t* context )
{
    // extract the layer specific data
    mbed_data_t* mbed_data
        = ( mbed_data_t* ) context->self->user_data;

    xi_debug_logger( "closing socket..." );

    // close the connection & the socket
    if( mbed_data->socket_ptr->close() == -1 )
    {
        xi_debug_logger( "error closing socket..." );
        xi_set_err( XI_SOCKET_CLOSE_ERROR );
        goto err_handling;
    }

    // safely destroy the object
    if ( mbed_data && mbed_data->socket_ptr )
    {
        xi_debug_logger( "deleting socket..." );

        delete mbed_data->socket_ptr;
        mbed_data->socket_ptr = 0;
    }

    if( mbed_data ) { XI_SAFE_FREE( mbed_data ); }

    return LAYER_STATE_OK;

    // cleanup the memory
err_handling:
    // safely destroy the object
    if ( mbed_data && mbed_data->socket_ptr )
    {
        delete mbed_data->socket_ptr;
        mbed_data->socket_ptr = 0;
    }

    if( mbed_data ) { XI_SAFE_FREE( mbed_data ); }

    return LAYER_STATE_ERROR;
}

layer_state_t mbed_io_layer_on_close( layer_connectivity_t* context )
{
    return CALL_ON_NEXT_CLOSE( context->self );
}

layer_state_t mbed_io_layer_init(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    layer_t* layer          = context->self;
    mbed_data_t* mbed_data  = 0;

    // allocation of the socket connector
    TCPSocketConnection* socket_ptr
        = new TCPSocketConnection();
    XI_CHECK_MEMORY( socket_ptr );

    // set the timeout for non-blocking operations
    socket_ptr->set_blocking( false, xi_globals.network_timeout );

    // allocate memory for the mbed data specific structure
    mbed_data = ( mbed_data_t* )
            xi_alloc( sizeof( mbed_data_t ) );
    XI_CHECK_MEMORY( mbed_data );

    mbed_data->socket_ptr = socket_ptr;

    // assign the layer specific data
    layer->user_data = ( void* ) mbed_data;

    return LAYER_STATE_OK;

err_handling:
    // safely destroy the object
    if ( mbed_data && mbed_data->socket_ptr )
    {
        delete mbed_data->socket_ptr;
        mbed_data->socket_ptr = 0;
    }

    if( mbed_data ) { XI_SAFE_FREE( mbed_data ); }

    return LAYER_STATE_ERROR;

}

layer_state_t mbed_io_layer_connect (
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint )
{
    layer_t* layer                          = context->self;
    mbed_data_t* mbed_data                  = ( mbed_data_t* ) layer->user_data;
    xi_connection_data_t* connection_data   = ( xi_connection_data_t* ) data;

    // try to connect
    int s = mbed_data->socket_ptr->connect(
          connection_data->address
        , connection_data->port );

    // check if not failed
    if( s == -1 )
    {
        xi_set_err( XI_SOCKET_CONNECTION_ERROR );
        goto err_handling;
    }

err_handling:
    // safely destroy the object
    if ( mbed_data && mbed_data->socket_ptr )
    {
        delete mbed_data->socket_ptr;
        mbed_data->socket_ptr = 0;
    }

    if( mbed_data ) { XI_SAFE_FREE( mbed_data ); }

    return LAYER_STATE_ERROR;
}