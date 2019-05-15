## Core Configuration Folder ##
CORE_TCF_DIR = $(BOARD_CORE_DIR)/tcf
CORE_CONFIG_MK = $(BOARD_CORE_DIR)/core_config.mk

## Current Supported Core Configurations
CORE_TCF_FILES = $(wildcard $(CORE_TCF_DIR)/*.tcf)
SUPPORTED_CORES := $(basename $(notdir $(CORE_TCF_FILES)))

#### core_config.mk existed in sub version folder of the board
COMMON_COMPILE_PREREQUISITES += $(CORE_CONFIG_MK)
include $(CORE_CONFIG_MK)

override CUR_CORE := arcem9d

VALID_CUR_CORE = $(CUR_CORE)

TCF = $(BOARD_CORE_DIR)/tcf/$(CUR_CORE).tcf
