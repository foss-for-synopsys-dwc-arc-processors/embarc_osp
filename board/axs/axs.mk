##
# \brief	current board directory definition
##
BOARD_AXS_DIR = $(BOARDS_ROOT)/axs
##
# board version definition
# 103 stands for 103, the firmware version supported is v1.1
# When you don't special BD_VER
# in any other place or through make options
# it will select a proper BD_VER according to your cur_core
##
SUPPORTED_BD_VERS = 103

## Select Board Version
BD_VER ?= 103

override BD_VER := $(strip $(BD_VER))

## Extra Hostlink Options
LCORE_OPT_MW += -Hhostlib=

## Set Valid Board Version
VALID_BD_VER = $(call check_item_exist, $(BD_VER), $(SUPPORTED_BD_VERS))

## Check TCF file existence
ifneq ($(TCF),)
ifeq ($(wildcard $(TCF)),)
$(error Tool Configuration File(TCF) - $(TCF) doesn't exist, please check it!)
else
TCFFILE_IS_VALID = 1
TCFFILE_NAME = $(firstword $(basename $(notdir $(TCF))))
endif
endif

## If CUR_CORE is not in SUPPORTED_BD_VERS list, then force BD_VER and VALID_BD_VER to be 103
ifeq ($(TCFFILE_IS_VALID),1)
ifeq ($(VALID_BD_VER),)
override BD_VER := 103
override VALID_BD_VER := 103
endif
endif

## Try to include different board version makefiles
ifneq ($(TCFFILE_IS_VALID),1)
ifeq ($(VALID_BD_VER),)
$(info BOARD $(BOARD) Version - $(SUPPORTED_BD_VERS) are supported)
$(error $(BOARD) Version $(BD_VER) is not supported, please check it!)
endif
endif

## Compiler Options
BOARD_CORE_DIR = $(BOARD_AXS_DIR)/configs/$(VALID_BD_VER)
COMMON_COMPILE_PREREQUISITES += $(BOARD_AXS_DIR)/configs/core_configs.mk
include $(BOARD_AXS_DIR)/configs/core_configs.mk

COMMON_COMPILE_PREREQUISITES += $(BOARD_AXS_DIR)/configs/core_compiler.mk
include $(BOARD_AXS_DIR)/configs/core_compiler.mk

## Board Related Settings

##
# set board image information for MCUBoot
##
ifneq ($(findstring mcuboot, $(LIB_SEL)), )
$(error MCUBoot does not support BOARD $(BOARD) now, please check it!)
endif

## Build Rules

##
# \brief	AXS device driver related
##
# onchip ip object rules
ifdef ONCHIP_IP_LIST
	BOARD_AXS_DEV_CSRCDIR += $(foreach ONCHIP_IP_OBJ, $(ONCHIP_IP_LIST), $(addprefix $(BOARD_AXS_DIR)/drivers/ip/, $(ONCHIP_IP_OBJ)))
endif

include $(EMBARC_ROOT)/device/device.mk

##
# \brief	axs device driver related
##
BOARD_AXS_DEV_CSRCDIR	+= $(DEV_CSRCDIR)
BOARD_AXS_DEV_ASMSRCDIR	+= $(DEV_ASMSRCDIR)
BOARD_AXS_DEV_INCDIR	+= $(DEV_INCDIR)

##
# \brief	axs board related source and header
##
BOARD_AXS_CSRCDIR	+= $(BOARD_AXS_DEV_CSRCDIR) $(BOARD_CORE_DIR) \
				$(BOARD_AXS_DIR)/common \

BOARD_AXS_ASMSRCDIR	+= $(BOARD_AXS_DEV_ASMSRCDIR) $(BOARD_CORE_DIR)
BOARD_AXS_INCDIR	+= $(BOARD_AXS_DEV_INCDIR) $(BOARD_CORE_DIR)

# find all the source files in the target directories
BOARD_AXS_CSRCS = $(call get_csrcs, $(BOARD_AXS_CSRCDIR))
BOARD_AXS_ASMSRCS = $(call get_asmsrcs, $(BOARD_AXS_ASMSRCDIR))

# get object files
BOARD_AXS_COBJS = $(call get_relobjs, $(BOARD_AXS_CSRCS))
BOARD_AXS_ASMOBJS = $(call get_relobjs, $(BOARD_AXS_ASMSRCS))
BOARD_AXS_OBJS = $(BOARD_AXS_COBJS) $(BOARD_AXS_ASMOBJS)

# get dependency files
BOARD_AXS_DEPS = $(call get_deps, $(BOARD_AXS_OBJS))

# extra macros to be defined
BOARD_AXS_DEFINES += $(CORE_DEFINES) $(DEV_DEFINES) -DEMBARC_OVERRIDE_ARC_INTERRUPT_MANAGEMENT

# genearte library
BOARD_LIB_AXS = $(OUT_DIR)/libboard_axs.a

# library generation rule
$(BOARD_LIB_AXS): $(BOARD_AXS_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(BOARD_AXS_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules


# Middleware Definitions
BOARD_INCDIR += $(BOARD_AXS_INCDIR)
BOARD_CSRCDIR += $(BOARD_AXS_CSRCDIR)
BOARD_ASMSRCDIR += $(BOARD_AXS_ASMSRCDIR)

BOARD_CSRCS += $(BOARD_AXS_CSRCS)
BOARD_CXXSRCS +=
BOARD_ASMSRCS += $(BOARD_AXS_ASMSRCS)
BOARD_ALLSRCS += $(BOARD_AXS_CSRCS) $(BOARD_AXS_ASMSRCS)

BOARD_COBJS += $(BOARD_AXS_COBJS)
BOARD_CXXOBJS +=
BOARD_ASMOBJS += $(BOARD_AXS_ASMOBJS)
BOARD_ALLOBJS += $(BOARD_AXS_OBJS)

BOARD_DEFINES += $(BOARD_AXS_DEFINES)
BOARD_DEPS += $(BOARD_AXS_DEPS)
BOARD_LIB = $(BOARD_LIB_AXS)