#pragma once

#include <stdint.h>
#include "manifest.h"

typedef struct _shared_data_t {
        manifest_t manifest;
        uint8_t m0_ready;
        uint8_t mc_ready;
        int counter;
} shared_data_t;

extern volatile shared_data_t* gp_shared_data;
