#include "shared_data.h"


extern char _shared_data_addr[];
volatile shared_data_t* gp_shared_data = (shared_data_t*)_shared_data_addr;
