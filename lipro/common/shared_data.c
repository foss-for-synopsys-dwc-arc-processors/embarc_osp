#include "shared_data.h"


//#pragma bss(".shared_data")
volatile shared_data_t* gp_shared_data = (shared_data_t*)_shared_data_addr;
//#pragma bss()
