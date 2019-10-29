#include <arc.h>
#include <stdio.h>
#include <string.h>
#include "lipro_common_defs.h"
#include "arc_mpu.h"

#define MPU_REGIONS 16
typedef struct mpu_desc_t {
        uint32_t base;
        uint32_t size;
        uint32_t attr;
} mpu_desc_t;

typedef struct core_mpu_config_t {
        uint32_t default_attr;
        uint32_t mpu_regions;
        mpu_desc_t descriptors[MPU_REGIONS];
} core_mpu_config_t;

const core_mpu_config_t mpu_cfg[NUM_CORES] = {
        // core 0
        {
                0x0, // default - no access
                5,
                {
                        { MP_CODE_START, MP_CODE_SIZE, AUX_MPU_ATTR_KE | AUX_MPU_ATTR_KR | AUX_MPU_ATTR_KW }, // code - execute
                        { MP_DATA_START, MP_DATA_SIZE, AUX_MPU_ATTR_KR | AUX_MPU_ATTR_KW }, // data - read/write
                        { SSTG_CODE_START, SSTG_CODE_SIZE, AUX_MPU_ATTR_KE | AUX_MPU_ATTR_KR },
                        { SSTG_DATA_START, SSTG_DATA_SIZE, AUX_MPU_ATTR_KR | AUX_MPU_ATTR_KW },                        
                        { SHARED_DATA_START, SHARED_DATA_SIZE, AUX_MPU_ATTR_KR | AUX_MPU_ATTR_KW },                        
                }
        },
        // core 1
        {
                0x0, // default - no access
                5,
                {
                        { MC_CODE_START, MC_CODE_SIZE, AUX_MPU_ATTR_KE | AUX_MPU_ATTR_KR | AUX_MPU_ATTR_KW }, // code - execute
                        { MC_DATA_START, MC_DATA_SIZE, AUX_MPU_ATTR_KR | AUX_MPU_ATTR_KW }, // data - read/write
                        { SSTG_CODE_START, SSTG_CODE_SIZE, AUX_MPU_ATTR_KE | AUX_MPU_ATTR_KR },
                        { SSTG_DATA_START, SSTG_DATA_SIZE, AUX_MPU_ATTR_KR | AUX_MPU_ATTR_KW },                        
                        { SHARED_DATA_START, SHARED_DATA_SIZE, AUX_MPU_ATTR_KR | AUX_MPU_ATTR_KW },                        
                }
        },
        // core 2
        {
                0x0, // default - no access
                0,
                {
                }
        },
        // core 3
        {
                0x0, // default - no access
                0,
                {
                }
        }
};
        
void mpu_config(const core_mpu_config_t* pmpu_cfg)
{
        uint32_t i;
        arc_mpu_default(pmpu_cfg->default_attr);
        for (i = 0; i < pmpu_cfg->mpu_regions; ++i) {
                const mpu_desc_t* pdesc = &(pmpu_cfg->descriptors[i]);
                arc_mpu_region(i, pdesc->base, pdesc->size, pdesc->attr);
        }
        arc_mpu_enable();                        
}

int sstg_main(void)
{
        uint32_t core_id = (_lr(AUX_IDENTITY) >> 8) & 0xff;
        uint32_t i;

        mpu_config(&(mpu_cfg[core_id]));

        for (i = 0; i < MAX_MODULES; ++i) {
                volatile module_entry_t* pmodule = &(gp_shared_data->manifest.modules[i]);
                if (pmodule->module_id == (core_id+1)) {
                        if (pmodule->memory_address != pmodule->load_address) {
                                memcpy((void *)pmodule->memory_address, (void *)pmodule->load_address, pmodule->size_in_bytes);
                        }                        
                        uint32_t reset_vector = *(uint32_t *)pmodule->memory_address;
                        ((void(*)())reset_vector)();
                }
        }
        return 1;
}
