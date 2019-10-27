#include <arc.h>
#include <stdio.h>
#include "shared_data.h"

int main(void)
{
        int core_id = (_lr(AUX_IDENTITY) >> 8) & 0xff;
        int i;
        for (i = 0; i < MAX_MODULES; ++i) {
                if (gp_shared_data->manifest.modules[i].module_id == (core_id+1)) {
                        // TODO copy if needed, namely for ESI...
                        
                        ((void(*)())gp_shared_data->manifest.modules[i].memory_address)();
                }
        }
        return 1;
}
