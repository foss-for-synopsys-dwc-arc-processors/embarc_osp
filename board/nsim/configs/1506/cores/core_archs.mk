### This is a virtual hs processor based on nsim. It has almost all the features that
### are supported in ARCv2 ISA, e.g., DSP, MPU and XY memory etc.. It has enough memory
### but has no peripherals except hostlink. It is mainly user for test and evaluation.

## Core Configuration Defines ##
ifeq ($(TOOLCHAIN), gnu)
CORE_DEFINES = -D_ARCVER=0x52
endif

CORE_DEFINES += -DNUM_EXC_INT=8 -DINT_PRI_MIN=-16 -DARC_FEATURE_CODE_DENSITY

## Metaware Toolchain Options For Core ##
COMMON_CORE_OPT_MW = -arcv2hs -core2 -Xatomic -Xll64 -Xcode_density -Xdiv_rem=radix4 \
		-Xbitscan -Xmpy_option=9 -Xbarrel_shifter \
		-Xfpus_div -Xfpu_mac -Xtimer0 -Xtimer1 -Xstack_check

CCORE_OPT_MW   += $(COMMON_CORE_OPT_MW)
CXXCORE_OPT_MW += $(COMMON_CORE_OPT_MW)
ACORE_OPT_MW   += $(COMMON_CORE_OPT_MW)
LCORE_OPT_MW   += $(COMMON_CORE_OPT_MW)

## GNU Toolchain Options For Core ##
COMMON_CORE_OPT_GNU = -mcpu=archs -mlittle-endian -mmpy-option=9 \
		-matomic -mfpu=fpus_div -mll64 -mdiv-rem -mbarrel-shifter -mcode-density

CCORE_OPT_GNU   += $(COMMON_CORE_OPT_GNU)
CXXCORE_OPT_GNU += $(COMMON_CORE_OPT_GNU)
ACORE_OPT_GNU   += $(COMMON_CORE_OPT_GNU)
LCORE_OPT_GNU   += $(COMMON_CORE_OPT_GNU)

## Metaware Debugger and nSIM Related Options ##

MDB_NSIM_OPT += -arcv2hs -core2 -arcnum=5 -Xtimer0 -Xtimer1 -Xlib -Xmpy_option=9 \
		-rgf_num_banks=1 -Xatomic -Xll64 -Xunaligned \
		-Xdiv_rem=radix4 -Xcode_density -Xbitscan -Xbarrel_shifter \
		-mpu -mpu_regions=8 -Xrtc -Xstack_check -action_points=8 \
		-bpu_bc_entries=512 -bpu_pt_entries=8192 -bpu_rs_entries=4 \
		-bpu_bc_full_tag=1 -bpu_bc_tag_size=20 -bpu_tosq_entries=5 \
		-bpu_fb_entries=2 -smart_stack_entries=8 \
		-Xfpus_div -Xfpu_mac \
		-interrupts=8 -ext_interrupts=6 -interrupt_priorities=16 \
		-dcache=65536,32,2,a -icache=65536,32,2,a \
		-icache_feature=2 -iccm_base=0x00000000 -iccm_size=0x00080000 \
		-dcache_feature=2 -dccm_base=0x80000000 -dccm_size=0x00080000 \
		-pc_width=32

NSIMDRV_OPT = -propsfile $(CPU_CORE_DIR)/nsim_$(CUR_CORE).props