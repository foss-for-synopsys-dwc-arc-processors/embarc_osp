#include "shared_data.h"


#pragma bss(".shared_data")
volatile shared_data_t g_shared_data;
#pragma bss()
