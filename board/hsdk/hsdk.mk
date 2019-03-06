##
# \brief	current board directory definition
##
BOARD_HSDK_DIR = $(BOARDS_ROOT)/hsdk
##
# board version definition
# 10 stands for v1.0
# When you don't special BD_VER
# in any other place or through make options
# it will select a proper BD_VER according to your cur_core
##
SUPPORTED_BD_VERS = 10

## Select Board Version
BD_VER ?= 10

override BD_VER := $(strip $(BD_VER))

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

## If CUR_CORE is not in SUPPORTED_BD_VERS list, then force BD_VER and VALID_BD_VER to be 10
ifeq ($(TCFFILE_IS_VALID),1)
ifeq ($(VALID_BD_VER),)
override BD_VER := 10
override VALID_BD_VER := 10
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
BOARD_CORE_DIR = $(BOARD_HSDK_DIR)/configs/$(VALID_BD_VER)
COMMON_COMPILE_PREREQUISITES += $(BOARD_HSDK_DIR)/configs/core_configs.mk
include $(BOARD_HSDK_DIR)/configs/core_configs.mk

COMMON_COMPILE_PREREQUISITES += $(BOARD_HSDK_DIR)/configs/core_compiler.mk
include $(BOARD_HSDK_DIR)/configs/core_compiler.mk

## Board Related Settings
OPENOCD_CFG_FILE = $(OPENOCD_SCRIPT_ROOT)/board/snps_hsdk.cfg

OPENOCD_OPTIONS  = -s $(OPENOCD_SCRIPT_ROOT) -f $(OPENOCD_CFG_FILE)

## Build Rules
# onchip ip object rules
ifdef ONCHIP_IP_LIST
	BOARD_HSDK_DEV_CSRCDIR += $(foreach ONCHIP_IP_OBJ, $(ONCHIP_IP_LIST), $(addprefix $(BOARD_HSDK_DIR)/drivers/ip/, $(ONCHIP_IP_OBJ)))
endif

# onboard device rules
EXT_DEV_LIST += io/cy8c95xx

include $(EMBARC_ROOT)/device/device.mk

##
# \brief	hsdk device driver related
##
BOARD_HSDK_DEV_CSRCDIR		+= $(DEV_CSRCDIR)
BOARD_HSDK_DEV_ASMSRCDIR	+= $(DEV_ASMSRCDIR)
BOARD_HSDK_DEV_INCDIR		+= $(DEV_INCDIR)

##
# \brief	hsdk board related source and header
##
BOARD_HSDK_CSRCDIR		+= $(BOARD_HSDK_DEV_CSRCDIR) $(BOARD_CORE_DIR) \
				$(BOARD_HSDK_DIR)/common \
				$(BOARD_HSDK_DIR)/drivers/creg \
				$(BOARD_HSDK_DIR)/drivers/cgu \
				$(BOARD_HSDK_DIR)/drivers/mux

# select dirvers according to middleware
ifneq ($(findstring ntshell, $(MID_SEL)), )
BOARD_HSDK_CSRCDIR += $(BOARD_HSDK_DIR)/drivers/ntshell
endif

ifneq ($(findstring fatfs, $(MID_SEL)), )
BOARD_HSDK_CSRCDIR += $(BOARD_HSDK_DIR)/drivers/sdcard
endif

ifneq ($(findstring lwip, $(MID_SEL)), )
BOARD_HSDK_CSRCDIR += $(BOARD_HSDK_DIR)/drivers/pmwifi
endif

##
# set board image information for MCUBoot
##
ifneq ($(findstring mcuboot, $(LIB_SEL)), )
$(error MCUBoot is not support BOARD $(BOARD) now, please check it!)
endif

BOARD_HSDK_ASMSRCDIR	+= $(BOARD_HSDK_DEV_ASMSRCDIR) $(BOARD_CORE_DIR)
BOARD_HSDK_INCDIR	+= $(BOARD_HSDK_DEV_INCDIR) $(BOARD_CORE_DIR)


# find all the source files in the target directories
BOARD_HSDK_CSRCS = $(call get_csrcs, $(BOARD_HSDK_CSRCDIR))
BOARD_HSDK_ASMSRCS = $(call get_asmsrcs, $(BOARD_HSDK_ASMSRCDIR))

# get object files
BOARD_HSDK_COBJS = $(call get_relobjs, $(BOARD_HSDK_CSRCS))
BOARD_HSDK_ASMOBJS = $(call get_relobjs, $(BOARD_HSDK_ASMSRCS))
BOARD_HSDK_OBJS = $(BOARD_HSDK_COBJS) $(BOARD_HSDK_ASMOBJS)

# get dependency files
BOARD_HSDK_DEPS = $(call get_deps, $(BOARD_HSDK_OBJS))

# extra macros to be defined
BOARD_HSDK_DEFINES += $(CORE_DEFINES) $(DEV_DEFINES) -DEMBARC_OVERRIDE_ARC_INTERRUPT_MANAGEMENT

# genearte library
BOARD_LIB_HSDK = $(OUT_DIR)/libboard_hsdk.a

# library generation rule
$(BOARD_LIB_HSDK): $(BOARD_HSDK_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(BOARD_HSDK_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules


# Middleware Definitions
BOARD_INCDIR += $(BOARD_HSDK_INCDIR)
BOARD_CSRCDIR += $(BOARD_HSDK_CSRCDIR)
BOARD_ASMSRCDIR += $(BOARD_HSDK_ASMSRCDIR)

BOARD_CSRCS += $(BOARD_HSDK_CSRCS)
BOARD_CXXSRCS +=
BOARD_ASMSRCS += $(BOARD_HSDK_ASMSRCS)
BOARD_ALLSRCS += $(BOARD_HSDK_CSRCS) $(BOARD_HSDK_ASMSRCS)

BOARD_COBJS += $(BOARD_HSDK_COBJS)
BOARD_CXXOBJS +=
BOARD_ASMOBJS += $(BOARD_HSDK_ASMOBJS)
BOARD_ALLOBJS += $(BOARD_HSDK_OBJS)

BOARD_DEFINES += $(BOARD_HSDK_DEFINES)
BOARD_DEPS += $(BOARD_HSDK_DEPS)
BOARD_LIB = $(BOARD_LIB_HSDK)