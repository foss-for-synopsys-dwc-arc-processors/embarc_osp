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
#include "api/ext/api_ext.h"

struct PMUExtension : ARC_nsimext {
private:
        ARC_nsimext_simulator_access* sim_access;
        static ARC_nsimext_simulator_access* sa_array[4];
        uint32       my_aux_reg_list[3];
        
        uint32       auxr;
        
public:
        PMUExtension()
                : auxr(0)
        {
                my_aux_reg_list[0] = 0x4242;
                my_aux_reg_list[1] = 0x4242;
                my_aux_reg_list[2] = 0;
                
        }

        void destroy() {
                delete this;
        }
        
        uint32 version() {
                return ARC_NSIMEXT_BASE_VERSION;
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
                sa->printf(0, "*****pmu.so set_simulator access: this=%p, id=%x, sa = %p\n", this, id, sa);
        }
        
        uint32 prepare_for_new_simulation() {
                auxr = 0;
                return 1;
        }
        /*** Extension aux register ***/
        
        uint32* aux_reg_list() {
                return my_aux_reg_list;
        }
        
        uint32 read_aux_reg(uint32 r, uint32 *value, uint8 context) {
                *value = auxr;
                return 1;
        }

        uint32 write_aux_reg(uint32 r, uint32 value, uint8 context) {
                auxr=value;
                uint32 status32;
                sim_access->printf(0, "aux reg written\n");
                sa_array[1]->read_aux_reg(0xA, &status32, from_execution);
                sim_access->printf(0, "read status32 %x from core 1\n", status32);
                status32 &= ~1;
                sa_array[1]->write_aux_reg(0xA, 0, from_agent);
                sa_array[1]->read_aux_reg(0xA, &status32, from_execution);
                sim_access->printf(0, "read status32 %x from core 1\n", status32);
                return 1;
        }
        
};

ARC_nsimext_simulator_access* PMUExtension::sa_array[4] = {0};

#ifdef __cplusplus
extern "C" {
#endif
        DLLEXPORT ARC_nsimext* get_ARC_nsimext_interface() {
                ARC_nsimext *ext = new PMUExtension;  /* create extension object */
                return ext;
        }
        
#ifdef __cplusplus
}
#endif
