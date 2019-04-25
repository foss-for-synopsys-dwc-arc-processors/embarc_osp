##
# TOOLCHAIN
# Select which TOOLCHAIN should be used
# Supported toolchains:
#
#    mw  -Metaware development tools
#    gnu -ARC GNU Tool chain
##
SUPPORTED_TOOLCHAINS = gnu mw
## Another define in options.mk
TOOLCHAIN ?= mw

##
# Definition of Tool Paths
##
MW_TOOLPATH  ?=
GNU_TOOLPATH ?=

override TOOLCHAIN := $(strip $(TOOLCHAIN))

## Set Valid Toolchain
VALID_TOOLCHAIN = $(call check_item_exist, $(TOOLCHAIN), $(SUPPORTED_TOOLCHAINS))

## Try include toolchain makefile
ifneq ($(VALID_TOOLCHAIN), )
COMMON_COMPILE_PREREQUISITES += $(EMBARC_ROOT)/options/toolchain/toolchain_$(VALID_TOOLCHAIN).mk
include $(EMBARC_ROOT)/options/toolchain/toolchain_$(VALID_TOOLCHAIN).mk
else
$(info TOOLCHAIN - $(SUPPORTED_TOOLCHAINS) are supported)
$(error Toolchain $(TOOLCHAIN) is not supported, please check it!)
endif
