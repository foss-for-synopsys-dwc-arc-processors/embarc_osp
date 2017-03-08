##
# Use compiler options defined by core itself
##
CORE_COMPILER_DEFINED = 1

## Current Supported Core Configurations
SUPPORTED_CORES := arcemfull arcemsecureshield archs

##
# CURRENT CORE CONFIGURATION
# Select the core configuration loaded into FPGA chip
##
CUR_CORE ?= arcemfull

override CUR_CORE := $(strip $(CUR_CORE))

## Set Valid Core Configuration For Board
VALID_CUR_CORE = $(call check_item_exist, $(CUR_CORE), $(SUPPORTED_CORES))

##
# \brief	development board cpu and device running frequency definition(HZ)
##
CPU_FREQ  ?= 25000000
DEV_FREQ  ?= 50000000

## Check Core Configuation Supported
ifneq ($(VALID_CUR_CORE),)
## Core Configuation Folder ##
CPU_CORE_DIR = $(BOARD_CORE_DIR)/cores
COMMON_COMPILE_PREREQUISITES += $(BOARD_CORE_DIR)/cores/core_$(VALID_CUR_CORE).mk
include $(BOARD_CORE_DIR)/cores/core_$(VALID_CUR_CORE).mk
else
$(info BOARD $(BOARD)-$(BD_VER) Core Configurations - $(SUPPORTED_CORES) are supported)
$(error $(CUR_CORE) is not supported in $(BOARD)-$(BD_VER), please check it!)
endif

CORE_MEM_CONFIG_DEFINE = -DCORE_MEM_CONFIG_FILE=\"cores/core_mem_config_$(VALID_CUR_CORE).h\"
CORE_DEFINES += $(CORE_MEM_CONFIG_DEFINE)