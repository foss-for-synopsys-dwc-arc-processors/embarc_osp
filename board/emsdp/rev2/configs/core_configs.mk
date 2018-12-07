## Current Supported Core Configurations
CORE_DIRS = $(sort $(dir $(wildcard $(BOARD_EMSDP_DIR)/$(VALID_BD_VER)/configs/*/*.h)))
SUPPORTED_CORES := $(patsubst $(BOARD_EMSDP_DIR)/$(VALID_BD_VER)/configs/%/, %,$(CORE_DIRS))

CORE ?= $(firstword $(SUPPORTED_CORES))

override CORE := $(strip $(CORE))

## Set Valid Core Configuration For Board
VALID_CORE = $(call check_item_exist, $(CORE), $(SUPPORTED_CORES))

BOARD_CORE_DIR = $(BOARD_EMSDP_DIR)/$(VALID_BD_VER)/configs/$(VALID_CORE)

ONCHIP_IP_LIST ?= . designware/spi designware/uart designware/gpio designware/sdio designware/trng designware/pwm_timer subsystem/spi subsystem/gpio subsystem/uart subsystem/iic subsystem/i2s

## Check TCF file existence
ifneq ($(TCFFILE_IS_VALID),1)
TCF ?= $(wildcard $(BOARD_CORE_DIR)/*/*.tcf)
endif

## If CORE is not in SUPPORT_CORES list, then force CORE and VALID_CORE to be TCF filename
ifeq ($(TCFFILE_IS_VALID),1)
ifeq ($(VALID_CORE),)
override CORE := $(TCFFILE_NAME)
override VALID_CORE := $(TCFFILE_NAME)
endif
endif

## Check Core Configuration Supported
ifeq ($(TCFFILE_IS_VALID),1)
ifeq ($(VALID_CORE),)
$(info BOARD $(BOARD)-$(BD_VER) Core Configurations - $(SUPPORTED_CORES) are supported)
$(error $(CORE) is not supported in $(BOARD)-$(BD_VER), please check it!)
endif
endif

CORE_DEFINES += -DCURRENT_CORE=$(VALID_CORE)