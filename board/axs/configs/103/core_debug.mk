ifeq ($(VALID_CUR_CORE),archs36)
OPENOCD_CFG_FILE ?= $(OPENOCD_SCRIPT_ROOT)/board/snps_axs103_hs36.cfg
else
ifeq ($(VALID_CUR_CORE),archs38)
OPENOCD_CFG_FILE ?= $(OPENOCD_SCRIPT_ROOT)/board/snps_axs103_hs38.cfg
else
OPENOCD_CFG_FILE ?= $(OPENOCD_SCRIPT_ROOT)/board/snps_axs103_hs36.cfg
endif
endif



