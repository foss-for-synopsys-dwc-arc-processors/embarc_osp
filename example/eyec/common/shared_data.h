#pragma once

#include <stdint.h>

typedef struct _shared_data_t {
    uint8_t m0_ready;
    uint8_t mc_ready;
} shared_data_t;

extern volatile shared_data_t g_shared_data;
