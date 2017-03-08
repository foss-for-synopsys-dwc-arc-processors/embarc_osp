// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.
/*
 * Modified for port to ARC processor
 * by Wayne Ren wei.ren@synopsys.com, Synopsys, Inc.
 */

/**
 * \file contiki_io_layer.c
 * \brief contiki io layer for xively library
 */

#include "contiki.h"
#include "contiki_io_layer.h"
#include "xively_contiki_data.h"
#include "net/ip/psock.h"

#include "xi_allocator.h"
#include "xi_err.h"
#include "xi_macros.h"
#include "xi_debug.h"

#include "xi_layer_api.h"
#include "xi_common.h"
#include "xi_connection_data.h"


#define DBG_LESS
#define DEBUG
#include "embARC_debug.h"

#define CONNECTING	(0)
#define CONNECTED	(1)
#define READING		(2)
#define READ_END	(3)
#define WRITTING	(4)
#define WRITE_END	(5)
#define CLOSING		(6)
#define CLOSED		(7)
#define ERROR		(-1)

PROCESS(xively_process, "xively process");


typedef struct
{
	struct psock p;
	unsigned int  out_len;
	uint8_t * out_buf;
	data_descriptor_t buffer_descriptor;
	int state;
	struct process *process;
} contiki_data_t;


process_event_t xively_event;

static contiki_data_t	contiki_state;

static PT_THREAD(handle_output(contiki_data_t *s))
{
	PSOCK_BEGIN(&s->p);
	if (s->state == WRITTING) {
		PSOCK_SEND(&s->p, s->out_buf, s->out_len);
		s->state = WRITE_END;
		process_post_synch(s->process, xively_event, s);
	}
	PSOCK_END(&s->p);
}

static void contiki_appcall(void *data)
{
	contiki_data_t *s = (contiki_data_t *)data;

	if (uip_closed() || uip_aborted() || uip_timedout()) {
		//xprintf("closed or aborted or timedout\n");
		if (s->state == READING || s->state == WRITTING) {
			s->state = ERROR;
		} else {
			s->state = CLOSED;
		}
		process_post_synch(s->process, xively_event, s);
	} else if (uip_connected()) {
		s->state = CONNECTED;
		PSOCK_INIT(&s->p, NULL, 0);
		process_post_synch(s->process, xively_event, s);
	} else if (uip_newdata()) {
		if (s->state == READING) {
			s->state = READ_END;
			process_post_synch(s->process, xively_event, s);
		}
	}
	if (s->state == CLOSING) {
		uip_close();
	}
	handle_output(s);
}

layer_state_t contiki_io_layer_data_ready(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint)
{
	contiki_data_t* s = (contiki_data_t*) context->self->user_data;
	const const_data_descriptor_t* buffer   = ( const const_data_descriptor_t* ) data;

	XI_UNUSED(hint);

	s->process = PROCESS_CURRENT();

	if (s->state == WRITTING) {
		return LAYER_STATE_WANT_WRITE;
	} else if (s->state == WRITE_END) {
		s->state = CONNECTED;
		return LAYER_STATE_OK;
	} else if (s->state == ERROR) {
		return LAYER_STATE_ERROR;
	} else if (s->state == CONNECTED) {
		if (buffer != NULL  && buffer->data_size > 0) {
			s->out_buf = (uint8_t *)buffer->data_ptr;
 			s->out_len = buffer->data_size;
 			//s->out_buf[s->out_len] = 0;
 			//xprintf("%s\n", s->out_buf);
 			s->state = WRITTING;
			handle_output(s);
			return LAYER_STATE_WANT_WRITE;
		}
		return LAYER_STATE_OK;
	}
	return LAYER_STATE_ERROR;
}


layer_state_t contiki_io_layer_on_data_ready(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint)
{
	contiki_data_t* s = (contiki_data_t*) context->self->user_data;
	data_descriptor_t* buffer  = 0;
	layer_state_t state;

	XI_UNUSED(hint);

	s->process = PROCESS_CURRENT();

	if (data) {
		buffer  = ( data_descriptor_t* ) data;
	} else {
        buffer = &s->buffer_descriptor;
	}

	if (s->state == READ_END) {
		buffer->data_ptr = (char *)uip_appdata;
		buffer->real_size = uip_datalen();
		buffer->data_size = uip_datalen() + 1;
    	buffer->data_ptr[buffer->real_size] = '\0'; // put guard
    	buffer->curr_pos = 0;
	 	state = CALL_ON_NEXT_ON_DATA_READY( context->self, ( void* ) buffer, LAYER_HINT_MORE_DATA );

	 	if (state == LAYER_STATE_WANT_READ) {
	 		s->state = READING;
	 		return LAYER_STATE_WANT_READ;
	 	}
	 	s->state = CONNECTED;
	 	return LAYER_STATE_OK;
	 } else if (s->state == READING) {
	 	return LAYER_STATE_WANT_READ;
	 } else if (s->state == CONNECTED) {
	 	s->state = READING;
		return LAYER_STATE_WANT_READ;
	 }

	 return LAYER_STATE_ERROR;	/* error, come here */
}


layer_state_t contiki_io_layer_close( layer_connectivity_t* context )
{
    contiki_data_t* s = ( contiki_data_t* ) context->self->user_data;

    XI_UNUSED(s);

    return CALL_ON_SELF_ON_CLOSE(context->self);
}

layer_state_t contiki_io_layer_on_close( layer_connectivity_t* context )
{
	contiki_data_t* s = ( contiki_data_t* ) context->self->user_data;

	s->process = PROCESS_CURRENT();
	if (s->state == CLOSED) {
		return LAYER_STATE_OK;
	}
	if (s->state == CLOSING) {
		return LAYER_STATE_WANT_WRITE;
	} else if (s->state == CONNECTED) {
		s->state = CLOSING;
		return LAYER_STATE_WANT_WRITE;
	}
	return LAYER_STATE_ERROR;
}


layer_state_t contiki_io_layer_init(
	layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint)
{
	XI_UNUSED( hint );
    XI_UNUSED( data );

    // PRECONDITIONS
    assert( context != 0 );

    xi_debug_logger( "[contiki_io_layer_init]" );

    layer_t* layer              = ( layer_t* ) context->self;
    contiki_data_t* s    = &contiki_state;

    s->state = CLOSED;
    s->process = NULL;

    layer->user_data            = ( void* ) s;

    xi_debug_logger( "Creating psocket..." );

    xi_debug_logger( "Psocket creation [ok]" );

    // POSTCONDITIONS
    assert( layer->user_data != 0 );

    return LAYER_STATE_OK;
}

layer_state_t contiki_io_layer_connect(
	layer_connectivity_t* context
	, const void* data
	, const layer_hint_t hint )
{

 	XI_UNUSED( hint );

	uip_ipaddr_t *ip;
	resolv_status_t dns_status;
    xi_connection_data_t* connection_data;
    layer_t* layer = ( layer_t* ) context->self;
    contiki_data_t* s = (contiki_data_t* ) layer->user_data;
    struct uip_conn *c;

    s->process = PROCESS_CURRENT();
    if (s->state == CONNECTED) {
    	xi_debug_logger( "Connecting to the endpoint [ok]" );
    	return LAYER_STATE_OK;
    } else if (s->state == CONNECTING) {
    	return LAYER_STATE_WANT_WRITE;
    } else if (s->state == CLOSED) {
	    connection_data   = ( xi_connection_data_t* ) data;
	    dns_status = resolv_lookup(connection_data->address, &ip);

	    if (dns_status != RESOLV_STATUS_CACHED) {
	    	if (dns_status ==  RESOLV_STATUS_NOT_FOUND || dns_status == RESOLV_STATUS_ERROR) {
	    		xi_debug_logger( "Getting Host by name [failed]" );
	        	xi_set_err( XI_SOCKET_GETHOSTBYNAME_ERROR );
	        	return LAYER_STATE_ERROR;
	    	}
	    	if (dns_status != RESOLV_STATUS_RESOLVING) {
	    		resolv_query(connection_data->address);
	    	}
	    	return LAYER_STATE_WANT_WRITE;	/* no IP, cannot go further */
	   	}
	   	xi_debug_logger( "Getting Host by name [ok]" );
	    xi_debug_logger( "Connecting to the endpoint..." );

	    c = uip_connect(ip, uip_htons(connection_data->port));
	 	if(c == NULL) {
	        xi_debug_logger( "Connecting to the endpoint [failed]" );
	        xi_set_err( XI_SOCKET_CONNECTION_ERROR );
	        return LAYER_STATE_ERROR;
	    }
	    s->state = CONNECTING;
	    c->appstate.p = &xively_process;
	    c->appstate.state = s;
	    tcpip_poll_tcp(c);
		return LAYER_STATE_WANT_WRITE;
	}

	return LAYER_STATE_ERROR;
}

PROCESS_THREAD(xively_process, ev, data)
{
	PROCESS_BEGIN();
	xively_event = process_alloc_event();
	xi_debug_logger("Xively process start");
	while(1) {
		PROCESS_WAIT_EVENT();
		if (ev == tcpip_event) {
			contiki_appcall(data);
		}
	}
	PROCESS_END();
}