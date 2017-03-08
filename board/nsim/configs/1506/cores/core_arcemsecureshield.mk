### This is a virtual em processor based on nsim. It has almost all the features that
### are supported in ARCv2 ISA, e.g., DSP, MPU and XY memory etc.. It has enough memory
### but has no peripherals except hostlink. It is mainly user for test and evaluation.

## Core Configuration Defines ##
ifeq ($(TOOLCHAIN), gnu)
CORE_DEFINES = -D_ARCVER=0x42
endif

CORE_DEFINES += -DNUM_EXC_INT=8 -DINT_PRI_MIN=-16 -DARC_FEATURE_CODE_DENSITY -DARC_FEATURE_SEC_PRESENT -DARC_FEATURE_MPU_BUILD_S=1

## Metaware Toolchain Options For Core ##
COMMON_CORE_OPT_MW = -arcv2em -core3 -Xsec_modes -Xcode_density -Xdiv_rem=radix2 -Xlib \
		-Xbitscan -Xmpy_option=6 -Xbarrel_shifter\
		-Xtimer0 -Xtimer1 -Xsec_timer0 -Xsec_timer1

CCORE_OPT_MW   += $(COMMON_CORE_OPT_MW)
CXXCORE_OPT_MW += $(COMMON_CORE_OPT_MW)
ACORE_OPT_MW   += $(COMMON_CORE_OPT_MW)
LCORE_OPT_MW   += $(COMMON_CORE_OPT_MW)

## GNU Toolchain Options For Core ##
COMMON_CORE_OPT_GNU = -mcpu=arcem -mlittle-endian -mmpy-option=6 \
		-mbarrel-shifter -mnorm -mswap

CCORE_OPT_GNU   += $(COMMON_CORE_OPT_GNU)
CXXCORE_OPT_GNU += $(COMMON_CORE_OPT_GNU)
ACORE_OPT_GNU   += $(COMMON_CORE_OPT_GNU)
LCORE_OPT_GNU   += $(COMMON_CORE_OPT_GNU)

## Metaware Debugger and nSIM Related Options ##
MDB_NSIM_OPT += $(COMMON_CORE_OPT_MW) \
		-mpuv4 -mpu_sid -mpu_regions=16 \
		-interrupts=8 -ext_interrupts=6 -interrupt_priorities=16 \
		-iccm_base=0x00000000 -iccm_size=0x00080000 \
		-dccm_base=0x80000000 -dccm_size=0x00080000 \
		-iccm0_sec_lvl=S -dccm_sec_lvl=S \
		-pc_width=32 -prop=nsim_isa_has_secure=1 -prop=nsim_isa_sec_modes_option=1 -prop=mpu_version=4 \
		-prop=mpu_sid_option=1

NSIMDRV_OPT = -propsfile $(CPU_CORE_DIR)/nsim_$(CUR_CORE).props