// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

#include "nob_runner.h"
#include "xi_layer_api.h"
#include "xi_coroutine.h"
#include "xi_connection_data.h"

layer_state_t process_xively_nob_step( xi_context_t* xi )
{
    // PRECONDITION
    assert( xi != 0 );

    static int16_t state                = 0;
    static layer_state_t layer_state    = LAYER_STATE_OK;
    xi_connection_data_t conn_data      = { XI_HOST, XI_PORT };

    BEGIN_CORO( state )

    // connect to the endpoint
    do
    {
        layer_state = CALL_ON_SELF_CONNECT( xi->layer_chain.bottom, ( void* ) &conn_data, LAYER_HINT_NONE );

        if( layer_state != LAYER_STATE_OK )
        {
            YIELD( state, layer_state );
        }

    } while( layer_state != LAYER_STATE_OK );

    // write data to the endpoint
    do
    {
        layer_state = CALL_ON_SELF_DATA_READY(
                      xi->layer_chain.top
                    , xi->input, LAYER_HINT_NONE );

        if( layer_state != LAYER_STATE_OK )
        {
            YIELD( state, layer_state );
        }

    } while( layer_state != LAYER_STATE_OK );

    if( layer_state == LAYER_STATE_ERROR )
    {
        EXIT( state, layer_state);
    }

    // now read the data from the endpoint
    do
    {
        layer_state = CALL_ON_SELF_ON_DATA_READY(
                      xi->layer_chain.bottom
                    , 0, LAYER_HINT_NONE );

        if( layer_state != LAYER_STATE_OK )
        {
            YIELD( state, layer_state );
        }

    } while( layer_state != LAYER_STATE_OK );

    // now close the connection
    do{
        layer_state = CALL_ON_SELF_CLOSE(xi->layer_chain.bottom);
        if( layer_state != LAYER_STATE_OK )
        {
            YIELD( state, layer_state );
        }

    } while( layer_state != LAYER_STATE_OK );

    EXIT( state, layer_state );

    END_CORO()

    state = 0;
    return 0;
}
