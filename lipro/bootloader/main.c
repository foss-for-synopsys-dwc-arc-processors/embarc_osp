#include "arc_builtin.h"
#include "arc.h"
#include "manifest.h"
#include "flash_drv.h"
#include "lipro_common_defs.h"

volatile unsigned long core_ready[NUM_CORES];
extern char _second_stage_addr[];

void release_core(uint32_t core_id)
{
        core_ready[core_id] = 1;
}


int bl_main()
{
        uint32_t i;

        for (i = 0; i < NUM_CORES; ++i) {
                gp_shared_data->core_init_complete[i] = 0;
        }
        
        flash_read((void *)&(gp_shared_data->manifest), 0, sizeof(manifest_t));

        for (i = 0; i < MAX_MODULES; ++i) {
                if (gp_shared_data->manifest.modules[i].module_id != 0) {
                        flash_read((void*)gp_shared_data->manifest.modules[i].load_address, gp_shared_data->manifest.modules[i].flash_address, gp_shared_data->manifest.modules[i].size_in_bytes);
                }
        }
        for (i = 0; i < NUM_CORES; ++i) {
                release_core(i);
        }
        uint32_t reset_vector = *(uint32_t *)_second_stage_addr;
        ((void(*)())reset_vector)();
        return -1;
}
