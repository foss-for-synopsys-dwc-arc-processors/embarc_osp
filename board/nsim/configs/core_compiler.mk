ifneq ($(CORE_COMPILER_DEFINED),1)
## Metaware Toolchain Options For Core ##
COMMON_CORE_OPT_MW = @$(EMBARC_GENERATED_DIR)/$(GENE_CCAC_ARG)

CCORE_OPT_MW   += $(COMMON_CORE_OPT_MW)
CXXCORE_OPT_MW += $(COMMON_CORE_OPT_MW)
ACORE_OPT_MW   += $(COMMON_CORE_OPT_MW)
LCORE_OPT_MW   += $(COMMON_CORE_OPT_MW)

## GNU Toolchain Options For Core ##
COMMON_CORE_OPT_GNU = @$(EMBARC_GENERATED_DIR)/$(GENE_GCC_ARG)

CCORE_OPT_GNU   += $(COMMON_CORE_OPT_GNU)
CXXCORE_OPT_GNU += $(COMMON_CORE_OPT_GNU)
ACORE_OPT_GNU   += $(COMMON_CORE_OPT_GNU)
LCORE_OPT_GNU   += $(COMMON_CORE_OPT_GNU)

## Metaware Debugger and nSIM Related Options ##
MDB_NSIM_OPT += -prop=nsim_isa_host_timer=1 -prop=nsim_isa_host_timer_mhz=10 \
		@$(EMBARC_GENERATED_DIR)/$(GENE_MDB_ARG)

NSIMDRV_OPT = -p nsim_isa_host_timer=1 -p nsim_isa_host_timer_mhz=10 \
		-propsfile $(EMBARC_GENERATED_DIR)/$(GENE_NSIM_PROPS)
endif