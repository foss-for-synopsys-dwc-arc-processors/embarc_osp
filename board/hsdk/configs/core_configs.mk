## Core Configuration Folder ##
CORE_TCF_DIR = $(BOARD_CORE_DIR)/tcf
CORE_CONFIG_MK = $(BOARD_CORE_DIR)/core_config.mk

## Current Supported Core Configurations
CORE_TCF_FILES = $(wildcard $(CORE_TCF_DIR)/*.tcf)
SUPPORTED_CORES := $(basename $(notdir $(CORE_TCF_FILES)))

##
# CURRENT CORE CONFIGURATION
# Select the core configuration loaded into FPGA chip
##
ifneq ($(wildcard $(CORE_CONFIG_MK)),)
#### core_config.mk existed in sub version folder of the board
COMMON_COMPILE_PREREQUISITES += $(CORE_CONFIG_MK)
include $(CORE_CONFIG_MK)
else
CUR_CORE ?= $(firstword $(SUPPORTED_CORES))
endif

override CUR_CORE := $(strip $(CUR_CORE))

## Set Valid Core Configuration For Board
VALID_CUR_CORE = $(call check_item_exist, $(CUR_CORE), $(SUPPORTED_CORES))

## Check TCF file existence
ifneq ($(TCFFILE_IS_VALID),1)
TCF ?= $(CORE_TCF_DIR)/$(VALID_CUR_CORE).tcf
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
