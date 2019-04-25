## Current Supported Core Configurations
CORE_DIRS = $(sort $(dir $(wildcard $(BOARD_EMSDP_DIR)/$(VALID_BD_VER)/configs/*/tool_config/arc.tcf)))
SUPPORTED_CORES := $(patsubst $(BOARD_EMSDP_DIR)/$(VALID_BD_VER)/configs/%/tool_config/, %,$(CORE_DIRS))

CUR_CORE ?= $(firstword $(SUPPORTED_CORES))

override CUR_CORE := $(strip $(CUR_CORE))

## Set Valid Core Configuration For Board
VALID_CUR_CORE = $(call check_item_exist, $(CUR_CORE), $(SUPPORTED_CORES))

BOARD_CORE_DIR = $(BOARD_EMSDP_DIR)/$(VALID_BD_VER)/configs/$(VALID_CUR_CORE)

CORE_CONFIG_MK = $(BOARD_CORE_DIR)/core_config.mk

##
# load core specific makefie if exist
##
ifneq ($(wildcard $(CORE_CONFIG_MK)),)
COMMON_COMPILE_PREREQUISITES += $(CORE_CONFIG_MK)
include $(CORE_CONFIG_MK)
endif

ONCHIP_IP_LIST ?= . designware/spi designware/uart designware/gpio  \
		designware/sdio designware/trng designware/pwm_timer \
		subsystem/spi subsystem/gpio subsystem/uart subsystem/iic \
		subsystem/i2s

## Check TCF file existence
ifneq ($(TCFFILE_IS_VALID),1)
TCF ?= $(BOARD_CORE_DIR)/tool_config/arc.tcf
endif

## If CUR_CORE is not in SUPPORT_CORES list, then force CUR_CORE and VALID_CUR_CORE to be TCF filename
ifeq ($(TCFFILE_IS_VALID),1)
ifeq ($(VALID_CUR_CORE),)
override CUR_CORE := $(TCFFILE_NAME)
override VALID_CUR_CORE := $(TCFFILE_NAME)
endif
endif

## Check Core Configuration Supported
ifneq ($(TCFFILE_IS_VALID),1)
ifeq ($(VALID_CUR_CORE),)
$(info BOARD $(BOARD)-$(BD_VER) Core Configurations - $(SUPPORTED_CORES) are supported)
$(error $(CUR_CORE) is not supported in $(BOARD)-$(BD_VER), please check it!)
endif
endif
