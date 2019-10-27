#include "arc_builtin.h"
#include "arc.h"
#include "manifest.h"
#include "flash_drv.h"
#include "shared_data.h"

#define CORE_NUM 4
volatile unsigned long core_ready[CORE_NUM];
extern char _second_stage_addr[];

int main()
{
        flash_read((void *)&gp_shared_data->manifest, 0, sizeof(manifest_t));
        int i;
        for (i = 0; i < MAX_MODULES; ++i) {
                if (gp_shared_data->manifest.modules[i].module_id != 0) {
                        flash_read((void*)gp_shared_data->manifest.modules[i].memory_address, gp_shared_data->manifest.modules[i].flash_address, gp_shared_data->manifest.modules[i].size_in_bytes);
                }
        }
        for (i = 0; i < CORE_NUM; ++i) {
                core_ready[i] = 1;
        }
        ((void(*)())_second_stage_addr)();
}
