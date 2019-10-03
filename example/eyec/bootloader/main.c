#include "arc_builtin.h"
#include "arc.h"

#define CORE_NUM 4
#define IVT0_BASE 0x10000
#define IVT1_BASE 0x10400
#define IVT2_BASE 0x10800
#define IVT3_BASE 0x10C00

const unsigned long IVT[CORE_NUM] = { IVT0_BASE, IVT1_BASE, IVT2_BASE, IVT3_BASE };
int core_complete[CORE_NUM] = {0};
int main()
{
        _disable();
        unsigned int cpuid = (_arc_lr_reg(AUX_IDENTITY) & 0xFF00) >> 8;
        if (cpuid >= CORE_NUM) {
                _arc_flag(0);
        }
        _arc_sr_reg(AUX_INT_VECT_BASE, IVT[cpuid]);
        if (cpuid != 0) {
                _arc_sleep(0);
        }
        core_complete[cpuid] = 1;
}
