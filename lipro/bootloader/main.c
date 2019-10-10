#include "arc_builtin.h"
#include "arc.h"
#include "manifest.h"
#include "flash_drv.h"

#define CORE_NUM 4
#define IVT0_BASE 0x10000
#define IVT1_BASE 0x10400
#define IVT2_BASE 0x10800
#define IVT3_BASE 0x10C00

const unsigned long IVT[CORE_NUM] = { IVT0_BASE, IVT1_BASE, IVT2_BASE, IVT3_BASE };
int core_complete[CORE_NUM] = {0};

manifest_t manifest;
int main()
{
        flash_read(&manifest, 0, sizeof(manifest_t));
        int i;
        for (i = 0; i < MAX_MODULES; ++i) {
                if (manifest.modules[i].module_id != 0) {
                        flash_read((void*)manifest.modules[i].memory_address, manifest.modules[i].flash_address, manifest.modules[i].size_in_bytes);
                }
        }
}
