#include "arc_builtin.h"
#include "arc.h"
#include "manifest.h"
#include "flash_drv.h"
#include "lipro_common_defs.h"

extern char _second_stage_addr[];

static void unhalt_core(uint32_t core_id)
{
        volatile uint32_t* pmu_base = (volatile uint32_t*)0x40000000;
        pmu_base[2 * core_id] = *(uint32_t *)_second_stage_addr;
        pmu_base[2 * core_id + 1] = 1;
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
        
        unhalt_core(1);
        uint32_t reset_vector = *(uint32_t *)_second_stage_addr;
        ((void(*)())reset_vector)();
        return -1;
}
