#pragma once

#include <stdint.h>
#include "manifest.h"

#define NUM_CORES 4

typedef struct _shared_data_t {
        manifest_t manifest;
        uint8_t core_init_complete[NUM_CORES];
        int counter;
} shared_data_t;

extern volatile shared_data_t* gp_shared_data;
