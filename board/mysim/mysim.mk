##
# \brief	current board directory definition
##
BOARD_NSIM_DIR = $(BOARDS_ROOT)/nsim
##
# nsim version definition
# When you don't special BD_VER
# in any other place or through make options
# it will select a proper BD_VER according to your cur_core
##
SUPPORTED_BD_VERS = 10

## Select Board Version
BD_VER ?= 10

override BD_VER := $(strip $(BD_VER))

## Extra Hostlink Options
LCORE_OPT_MW  += -Hhostlink
LCORE_OPT_GNU += --specs=nsim.specs

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
BOARD_CORE_DIR = $(BOARD_NSIM_DIR)/configs/$(VALID_BD_VER)
COMMON_COMPILE_PREREQUISITES += $(BOARD_NSIM_DIR)/configs/core_configs.mk
include $(BOARD_NSIM_DIR)/configs/core_configs.mk

COMMON_COMPILE_PREREQUISITES += $(BOARD_NSIM_DIR)/configs/core_compiler.mk
include $(BOARD_NSIM_DIR)/configs/core_compiler.mk


include $(EMBARC_ROOT)/device/device.mk

##
# \brief	nsim device driver related
##
BOARD_NSIM_DEV_CSRCDIR		+= $(DEV_CSRCDIR)
BOARD_NSIM_DEV_ASMSRCDIR	+= $(DEV_ASMSRCDIR)
BOARD_NSIM_DEV_INCDIR		+= $(DEV_INCDIR)

##
# \brief	nsim board related source and header
##
BOARD_NSIM_CSRCDIR		+= $(BOARD_NSIM_DEV_CSRCDIR) $(BOARD_CORE_DIR) \
				$(BOARD_NSIM_DIR)/common \
				$(BOARD_NSIM_DIR)/drivers/uart \

# select dirvers according to middleware
ifneq ($(findstring ntshell, $(MID_SEL)), )
BOARD_NSIM_CSRCDIR += $(BOARD_NSIM_DIR)/drivers/ntshell
endif

##
# set board image information for MCUBoot
##
ifneq ($(findstring mcuboot, $(LIB_SEL)), )
$(error MCUBoot does not support BOARD $(BOARD) now, please check it!)
endif

BOARD_NSIM_ASMSRCDIR	+= $(BOARD_NSIM_DEV_ASMSRCDIR) $(BOARD_CORE_DIR)
BOARD_NSIM_INCDIR	+= $(BOARD_NSIM_DEV_INCDIR) $(BOARD_CORE_DIR)

# find all the source files in the target directories
BOARD_NSIM_CSRCS = $(call get_csrcs, $(BOARD_NSIM_CSRCDIR))
BOARD_NSIM_ASMSRCS = $(call get_asmsrcs, $(BOARD_NSIM_ASMSRCDIR))

# get object files
BOARD_NSIM_COBJS = $(call get_relobjs, $(BOARD_NSIM_CSRCS))
BOARD_NSIM_ASMOBJS = $(call get_relobjs, $(BOARD_NSIM_ASMSRCS))
BOARD_NSIM_OBJS = $(BOARD_NSIM_COBJS) $(BOARD_NSIM_ASMOBJS)

# get dependency files
BOARD_NSIM_DEPS = $(call get_deps, $(BOARD_NSIM_OBJS))

# extra macros to be defined
BOARD_NSIM_DEFINES += $(CORE_DEFINES) -D_HOSTLINK_ -D_NSIM_

# genearte library
BOARD_LIB_NSIM = $(OUT_DIR)/libboard_nsim.a

# library generation rule
$(BOARD_LIB_NSIM): $(BOARD_NSIM_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(BOARD_NSIM_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules


# Middleware Definitions
BOARD_INCDIR += $(BOARD_NSIM_INCDIR)
BOARD_CSRCDIR += $(BOARD_NSIM_CSRCDIR)
BOARD_ASMSRCDIR += $(BOARD_NSIM_ASMSRCDIR)

BOARD_CSRCS += $(BOARD_NSIM_CSRCS)
BOARD_CXXSRCS +=
BOARD_ASMSRCS += $(BOARD_NSIM_ASMSRCS)
BOARD_ALLSRCS += $(BOARD_NSIM_CSRCS) $(BOARD_NSIM_ASMSRCS)

BOARD_COBJS += $(BOARD_NSIM_COBJS)
BOARD_CXXOBJS +=
BOARD_ASMOBJS += $(BOARD_NSIM_ASMOBJS)
BOARD_ALLOBJS += $(BOARD_NSIM_OBJS)

BOARD_DEFINES += $(BOARD_NSIM_DEFINES)
BOARD_DEPS += $(BOARD_NSIM_DEPS)
BOARD_LIB = $(BOARD_LIB_NSIM)