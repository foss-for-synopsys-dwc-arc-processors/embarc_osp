### This is a virtual em processor based on nsim. It has almost all the features that
### are supported in ARCv2 ISA, e.g., DSP, MPU and XY memory etc.. It has enough memory
### but has no peripherals except hostlink. It is mainly user for test and evaluation.

## Core Configuration Defines ##
ifeq ($(TOOLCHAIN), gnu)
CORE_DEFINES = -D_ARCVER=0x42
endif

CORE_DEFINES += -DNUM_EXC_INT=8 -DINT_PRI_MIN=-16 -DARC_FEATURE_RGF_BANKED_REGS=32 \
	-DARC_FEATURE_FIRQ=1 -DARC_FEATURE_RGF_NUM_BANKS=2 -DARC_FEATURE_CODE_DENSITY

## Metaware Toolchain Options For Core ##
COMMON_CORE_OPT_MW = -arcv2em -core2 -Hrgf_banked_regs=32 -Xcode_density -Xdiv_rem=radix2 -Xlib \
		-Xbitscan -Xmpy_option=6 -Xbarrel_shifter -Xdsp2 -Xdsp_complex -Xatomic \
		-Xdsp_divsqrt=radix2 -Xdsp_itu -Xdsp_accshift=full -Xagu_large -Xxy -Xbitstream \
		-Xfpus_div -Xfpu_mac -Xfpus_mpy_slow -Xfpus_div_slow \
		-Xtimer0 -Xtimer1 -Xstack_check -Xdmac

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
MDB_NSIM_OPT += -arcv2em -core2 -Xtimer0 -Xtimer1 -Xlib -Xmpy_option=6	\
		-Xdiv_rem=radix2 -Xcode_density -Xatomic -Xbitscan -Xbarrel_shifter \
		-mpu -mpu_regions=16 \
		-Xdsp2 -Xdsp_complex -Xdsp_divsqrt=radix2 -Xdsp_itu -Xdsp_accshift=full \
		-Xagu_large -Xxy -Xbitstream \
		-Xfpus_div -Xfpu_mac -Xfpus_mpy_slow -Xfpus_div_slow \
		-firq -interrupts=8 -ext_interrupts=6 -interrupt_priorities=16 \
		-icache_feature=2 -iccm_base=0x00000000 -iccm_size=0x00080000 \
		-dcache_feature=2 -dccm_base=0x80000000 -dccm_size=0x00080000 \
		-rgf_num_banks=2 -rgf_banked_regs=32 -pc_width=32 \
		-dmac -dmac_channels=2 -dmac_registers=0 -dmac_fifo_depth=2 -dmac_int_config=single_internal

NSIMDRV_OPT = -propsfile $(CPU_CORE_DIR)/nsim_$(CUR_CORE).props