## Current Supported Core Configurations
SUPPORTED_CORES := em11d_dfss em6_scss em7d_esp_dfss

CORE ?= $(firstword $(SUPPORTED_CORES))

override CORE := $(strip $(CORE))

## Set Valid Core Configuration For Board
VALID_CORE = $(call check_item_exist, $(CORE), $(SUPPORTED_CORES))

BOARD_CORE_DIR = $(BOARD_EMSDP_DIR)/$(VALID_BD_VER)/configs/$(VALID_CORE)

## Core Configuration Folder ##
CORE_TCF_DIR = $(BOARD_CORE_DIR)/tcf
CORE_CONFIG_MK = $(BOARD_CORE_DIR)/core_config.mk

include $(CORE_CONFIG_MK)

## Check TCF file existence
ifneq ($(TCFFILE_IS_VALID),1)
TCF ?= $(CORE_TCF_DIR)/$(VALID_CORE).tcf
endif

## If CORE is not in SUPPORT_CORES list, then force CORE and VALID_CORE to be TCF filename
ifeq ($(TCFFILE_IS_VALID),1)
ifeq ($(VALID_CORE),)
override CORE := $(TCFFILE_NAME)
override VALID_CORE := $(TCFFILE_NAME)
endif
endif

## Check Core Configuration Supported
ifneq ($(TCFFILE_IS_VALID),1)
ifeq ($(VALID_CORE),)
$(info BOARD $(BOARD)-$(BD_VER) Core Configurations - $(SUPPORTED_CORES) are supported)
$(error $(CORE) is not supported in $(BOARD)-$(BD_VER), please check it!)
endif
endif

CORE_DEFINES += -DCURRENT_CORE=$(VALID_CORE)