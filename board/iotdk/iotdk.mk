##
# \brief	current board directory definition
##
BOARD_IOTDK_DIR = $(BOARDS_ROOT)/iotdk
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
BOARD_CORE_DIR = $(BOARD_IOTDK_DIR)/configs/$(VALID_BD_VER)
COMMON_COMPILE_PREREQUISITES += $(BOARD_IOTDK_DIR)/configs/core_configs.mk
include $(BOARD_IOTDK_DIR)/configs/core_configs.mk

COMMON_COMPILE_PREREQUISITES += $(BOARD_IOTDK_DIR)/configs/core_compiler.mk
include $(BOARD_IOTDK_DIR)/configs/core_compiler.mk

## Board Related Settings, GNU is not supported
OPENOCD_CFG_FILE ?= $(BOARD_IOTDK_DIR)/configs/openocd/snps_iotdk.cfg

OPENOCD_OPTIONS ?= -s $(OPENOCD_SCRIPT_ROOT) -f $(OPENOCD_CFG_FILE)

## hardware debug options
# debug speed config, iotdk starts as 16 Mhz, so the max jtag freq. is
# 8 Mhz, for mdb, the default value is 10 Mhz
ifeq ($(strip $(DIG_SPEED)), )
DIG_SPEED = 8000000
endif

## Build Rules
# onchip ip object rules
ifdef ONCHIP_IP_LIST
	BOARD_IOTDK_DEV_CSRCDIR += $(foreach ONCHIP_IP_OBJ, $(ONCHIP_IP_LIST), $(addprefix $(BOARD_IOTDK_DIR)/drivers/ip/, $(ONCHIP_IP_OBJ)))
endif

include $(EMBARC_ROOT)/device/device.mk

##
# \brief	iotdk device driver related
##
BOARD_IOTDK_DEV_CSRCDIR		+= $(DEV_CSRCDIR)
BOARD_IOTDK_DEV_ASMSRCDIR	+= $(DEV_ASMSRCDIR)
BOARD_IOTDK_DEV_INCDIR		+= $(DEV_INCDIR)

##
# \brief	iotdk board related source and header
##
BOARD_IOTDK_CSRCDIR		+= $(BOARD_IOTDK_DEV_CSRCDIR) $(BOARD_CORE_DIR) \
				$(BOARD_IOTDK_DIR)/common \
				$(BOARD_IOTDK_DIR)/drivers/sdcard \
				$(BOARD_IOTDK_DIR)/drivers/mux \
				$(BOARD_IOTDK_DIR)/drivers/sysconf \
				$(BOARD_IOTDK_DIR)/drivers/smic/smic_adc \
				$(BOARD_IOTDK_DIR)/drivers/smic/smic_eflash \
				$(BOARD_IOTDK_DIR)/drivers/smic/smic_bootspi \
				$(BOARD_IOTDK_DIR)/drivers/flash_obj
# select dirvers according to middleware
ifneq ($(findstring ntshell, $(MID_SEL)), )
BOARD_IOTDK_CSRCDIR 		+= $(BOARD_IOTDK_DIR)/drivers/ntshell
endif

ifneq ($(findstring fatfs, $(MID_SEL)), )
BOARD_IOTDK_CSRCDIR 		+= $(BOARD_IOTDK_DIR)/drivers/sdcard
endif

ifneq ($(findstring lwip, $(MID_SEL)), )
BOARD_IOTDK_CSRCDIR 		+= $(BOARD_IOTDK_DIR)/drivers/pmwifi
endif

##
# set board image information for MCUBoot
# TODO: needs to modify
##
ifneq ($(findstring mcuboot, $(LIB_SEL)), )
IMAGE_FLASH_ALIGN ?= 8
IMAGE_SLOT_SIZE ?= 0x000C0000
IMAGE_HEADER_SIZE ?= 0x400
endif

BOARD_IOTDK_ASMSRCDIR		+= $(BOARD_IOTDK_DEV_ASMSRCDIR) $(BOARD_CORE_DIR)
BOARD_IOTDK_INCDIR 		+= $(BOARD_IOTDK_DEV_INCDIR) $(BOARD_CORE_DIR)

# find all the source files in the target directories
BOARD_IOTDK_CSRCS = $(call get_csrcs, $(BOARD_IOTDK_CSRCDIR))
BOARD_IOTDK_ASMSRCS = $(call get_asmsrcs, $(BOARD_IOTDK_ASMSRCDIR))

# get object files
BOARD_IOTDK_COBJS = $(call get_relobjs, $(BOARD_IOTDK_CSRCS))
BOARD_IOTDK_ASMOBJS = $(call get_relobjs, $(BOARD_IOTDK_ASMSRCS))
BOARD_IOTDK_OBJS = $(BOARD_IOTDK_COBJS) $(BOARD_IOTDK_ASMOBJS)

# get dependency files
BOARD_IOTDK_DEPS = $(call get_deps, $(BOARD_IOTDK_OBJS))

# extra macros to be defined
BOARD_IOTDK_DEFINES += $(CORE_DEFINES) $(DEV_DEFINES)

# genearte library
BOARD_LIB_IOTDK = $(OUT_DIR)/libboard_iotdk.a

# library generation rule
$(BOARD_LIB_IOTDK): $(BOARD_IOTDK_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(BOARD_IOTDK_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules


# Middleware Definitions
BOARD_INCDIR += $(BOARD_IOTDK_INCDIR)
BOARD_CSRCDIR += $(BOARD_IOTDK_CSRCDIR)
BOARD_ASMSRCDIR += $(BOARD_IOTDK_ASMSRCDIR)

BOARD_CSRCS += $(BOARD_IOTDK_CSRCS)
BOARD_CXXSRCS +=
BOARD_ASMSRCS += $(BOARD_IOTDK_ASMSRCS)
BOARD_ALLSRCS += $(BOARD_IOTDK_CSRCS) $(BOARD_IOTDK_ASMSRCS)

BOARD_COBJS += $(BOARD_IOTDK_COBJS)
BOARD_CXXOBJS +=
BOARD_ASMOBJS += $(BOARD_IOTDK_ASMOBJS)
BOARD_ALLOBJS += $(BOARD_IOTDK_OBJS)

BOARD_DEFINES += $(BOARD_IOTDK_DEFINES)
BOARD_DEPS += $(BOARD_IOTDK_DEPS)
BOARD_LIB = $(BOARD_LIB_IOTDK)
