// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.
/*
 * Modified for port to ARC processor
 * by Wayne Ren wei.ren@synopsys.com, Synopsys, Inc.
 */

#ifndef _CONTIKI_IO_LAYER_H_
#define _CONTIKI_IO_LAYER_H_

// local
#include "xi_layer.h"

#ifdef __cplusplus
extern "C" {
#endif

layer_state_t contiki_io_layer_data_ready(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint );

layer_state_t contiki_io_layer_on_data_ready(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint );

layer_state_t contiki_io_layer_close(
    layer_connectivity_t* context );

layer_state_t contiki_io_layer_on_close(
    layer_connectivity_t* context );

layer_state_t contiki_io_layer_init(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint );

layer_state_t contiki_io_layer_connect(
      layer_connectivity_t* context
    , const void* data
    , const layer_hint_t hint );

#ifdef __cplusplus
}
#endif

#endif // _CONTIKI_IO_LAYER_H_
