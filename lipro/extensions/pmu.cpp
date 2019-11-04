/*
<copyright>
     Copyright (C) 2012 Synopsys, Inc. This software and the associated
     documentation are confidential and proprietary to Synopsys, Inc.
     Your use or disclosure of this software is subject to the terms and
     conditions of a written license agreement between you, or your company,
     and Synopsys, Inc.
</copyright>
*/

#define OEM_USE_OF_NSIM_HEADER_FILES 1
#define BUILDING_DLL


#define NUM_CORES 4
#define PMU_MEM_BASE 0x40000000

#include <cstddef>
#include <string.h>
#include "api/ext/api_ext.h"
#include "api/ext/api_ext_mem.h"

struct PMUExtension : ARC_nsimext_mem_api {
private:
        ARC_nsimext_simulator_access* sim_access;
        static ARC_nsimext_simulator_access* sa_array[NUM_CORES];

        static struct core_ctrl {
                uint32 reset_address;
                uint32 control;
        } cores_ctrl[NUM_CORES];
public:

        void destroy() {
                delete this;
        }

        uint32 version() {
                return ARC_NSIMEXT_MEM_API_BASE_VERSION;
        }

        const char* get_name() {
                return "PMUExtension";
        }
        

        void set_simulator_access(ARC_nsimext_simulator_access* sa) {
                sim_access = sa;
                uint32 id;
                sa->read_aux_reg(4, &id, from_execution);
                id = (id >> 8) & 0xff;
                sa_array[id] = sa;
                sa->printf(0, "*****pmu.so MEM EXT set_simulator access: this=%p, id=%x, sa = %p\n", this, id, sa);
        }

        uint64 range_begin() {
                return PMU_MEM_BASE;
        }

        uint64 range_end() {
                return PMU_MEM_BASE + sizeof(cores_ctrl) - 1;
        }

        uint32 read(ARC_ext_mem_trans* trans) {
                uint8* reg_mem = (uint8*)cores_ctrl;
                uint32 offset = (uint32)trans->address - PMU_MEM_BASE;
                memcpy(trans->data_ptr, reg_mem + offset, trans->data_length);
                return 1;                
        }
        
        uint32 write(ARC_ext_mem_trans* trans) { 
                uint8* reg_mem = (uint8*)cores_ctrl;
                uint32 offset = (uint32)trans->address - PMU_MEM_BASE;
                memcpy(reg_mem + offset, trans->data_ptr, trans->data_length);

                if (offset % sizeof(core_ctrl) == offsetof(core_ctrl, control)) {
                        //writing to control register
                        uint32 core_id = offset / sizeof(core_ctrl);                        
                        uint32 status32;
 
                        sim_access->printf(0, "reseting core %d to address %p\n", core_id, cores_ctrl[core_id].reset_address);
                       
                        sa_array[core_id]->write_aux_reg(0x6, cores_ctrl[core_id].reset_address, from_agent);
                        sa_array[core_id]->read_aux_reg(0xA, &status32, from_agent);
                        status32 &= ~1;
                        sa_array[core_id]->write_aux_reg(0xA, status32, from_agent);
                }
                return 1;
        }
};



ARC_nsimext_simulator_access* PMUExtension::sa_array[NUM_CORES] = {0};
struct PMUExtension::core_ctrl PMUExtension::cores_ctrl[NUM_CORES] = {0};


#ifdef __cplusplus
extern "C" {
        #endif
        DLLEXPORT ARC_nsimext_mem_api* get_ARC_nsimext_mem_interface() {
                ARC_nsimext_mem_api *ext = new PMUExtension;  /* create extension object */
                return ext;
        }

        #ifdef __cplusplus
}
#endif
