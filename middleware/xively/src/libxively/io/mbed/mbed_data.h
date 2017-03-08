// Copyright (c) 2003-2014, LogMeIn, Inc. All rights reserved.
// This is part of Xively C library, it is under the BSD 3-Clause license.

#ifndef __MBED_DATA_H__
#define __MBED_DATA_H__

#include "TCPSocketConnection.h"

typedef struct {
    TCPSocketConnection* socket_ptr;
} mbed_data_t;

#endif // __MBED_DATA_H__
