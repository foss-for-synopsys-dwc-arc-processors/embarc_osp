##
# \defgroup	MK_OS	OS Makefile Configurations
# \brief	makefile related to operation system configurations
##

##
# OS Root
##
OSES_ROOT_DIR = $(EMBARC_ROOT)/os

##
# OS
# select supported os
##
SUPPORTED_OSES = freertos contiki ucos
OS_SEL ?= freertos

override OS_SEL := $(strip $(OS_SEL))

## Set Valid OS
VALID_OS_SEL = $(call check_item_exist, $(OS_SEL), $(SUPPORTED_OSES))

## Try Check OS is valid
ifeq ($(VALID_OS_SEL), )
$(info OS - $(SUPPORTED_OSES) are supported)
$(error OS $(OS_SEL) is not supported, please check it!)
endif

OS_CSRCDIR	=
OS_ASMSRCDIR	=
OS_INCDIR	=

# source files and object files
OS_CSRCS =
OS_CXXSRCS =
OS_ASMSRCS =
OS_ALLSRCS =

OS_COBJS =
OS_CXXOBJS =
OS_ASMOBJS =
OS_ALLOBJS =

## OS HAL LAYER ##
COMMON_COMPILE_PREREQUISITES += $(OSES_ROOT_DIR)/hal/os_hal.mk
include $(OSES_ROOT_DIR)/hal/os_hal.mk

## OS Selection ##
ifeq ($(OS_SEL), freertos)
#os definition
OS_ID = OS_FREERTOS
#os usage settings
COMMON_COMPILE_PREREQUISITES += $(OSES_ROOT_DIR)/freertos/freertos.mk
include $(OSES_ROOT_DIR)/freertos/freertos.mk
else ifeq ($(OS_SEL), ucos)
#os definition
OS_ID = OS_UCOS
#os usage settings
COMMON_COMPILE_PREREQUISITES += $(OSES_ROOT_DIR)/ucos/ucos.mk
include $(OSES_ROOT_DIR)/ucos/ucos.mk
endif

##
# \brief	add defines for os
##
ifdef OS_ID
OS_DEFINES += -D$(OS_ID) -DENABLE_OS
else
OS_DEFINES +=
endif

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
-include $(OS_DEPS)
endif