##
# \defgroup 	MK_BOARD_EMSDP	EMSDP Board Related Makefile Configurations
# \ingroup	MK_BOARD
# \brief	makefile related to emsdp board configurations
##

##
# \brief	current board directory definition
##
BOARD_EMSDP_DIR = $(BOARDS_ROOT)/emsdp
##
# board version definition
# rev2 stands for revision 2
# When you don't specify BD_VER
# in any other place or through make options
# it will select a proper BD_VER according to your core
##
SUPPORTED_BD_VERS = rev2

## Select Board Version
BD_VER ?= rev2

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

## If CUR_CORE is not in SUPPORTED_BD_VERS list, then force BD_VER and VALID_BD_VER to be rev2
ifeq ($(TCFFILE_IS_VALID),1)
ifeq ($(VALID_BD_VER),)
override BD_VER := rev2
override VALID_BD_VER := rev2
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
COMMON_COMPILE_PREREQUISITES += $(BOARD_EMSDP_DIR)/$(VALID_BD_VER)/configs/core_configs.mk
include $(BOARD_EMSDP_DIR)/$(VALID_BD_VER)/configs/core_configs.mk

COMMON_COMPILE_PREREQUISITES += $(BOARD_EMSDP_DIR)/$(VALID_BD_VER)/configs/core_compiler.mk
include $(BOARD_EMSDP_DIR)/$(VALID_BD_VER)/configs/core_compiler.mk

## Board Related Settings
OPENOCD_CFG_FILE = $(BOARD_EMSDP_DIR)/openocd/snps_emsdp.cfg
OPENOCD_OPTIONS  = -s $(OPENOCD_SCRIPT_ROOT) -f $(OPENOCD_CFG_FILE)


##
# \brief	emsdp board related source and header
##
# onchip ip object rules
ifdef ONCHIP_IP_LIST
	BOARD_EMSDP_DEV_CSRCDIR += $(foreach ONCHIP_IP_OBJ, $(ONCHIP_IP_LIST), $(addprefix $(BOARD_EMSDP_DIR)/drivers/ip/, $(ONCHIP_IP_OBJ)))
endif

# onboard device rules
EXT_DEV_LIST +=

include $(EMBARC_ROOT)/device/device.mk

##
# \brief	emsdp device driver related
##
BOARD_EMSDP_DEV_CSRCDIR		+= $(DEV_CSRCDIR)
BOARD_EMSDP_DEV_ASMSRCDIR	+= $(DEV_ASMSRCDIR)
BOARD_EMSDP_DEV_INCDIR		+= $(DEV_INCDIR)

BOARD_EMSDP_CSRCDIR += $(BOARD_EMSDP_DEV_CSRCDIR) $(BOARD_CORE_DIR) \
			$(BOARD_EMSDP_DIR)/common \
			$(BOARD_EMSDP_DIR)/drivers/mux

# select dirvers according to middleware
ifneq ($(findstring ntshell, $(MID_SEL)), )
BOARD_EMSDP_CSRCDIR += $(BOARD_EMSDP_DIR)/drivers/ntshell
endif

ifneq ($(findstring fatfs, $(MID_SEL)), )
BOARD_EMSDP_CSRCDIR += $(BOARD_EMSDP_DIR)/drivers/sdcard
endif

ifneq ($(findstring lwip, $(MID_SEL)), )
BOARD_EMSDP_CSRCDIR += $(BOARD_EMSDP_DIR)/drivers/pmwifi
endif

BOARD_EMSDP_ASMSRCDIR	+= $(BOARD_EMSDP_DEV_ASMSRCDIR) $(BOARD_CORE_DIR)
BOARD_EMSDP_INCDIR	+= $(BOARD_EMSDP_DEV_INCDIR) \
			$(BOARD_EMSDP_DIR)/$(VALID_BD_VER)/configs \
			$(BOARD_CORE_DIR)/include


# find all the source files in the target directories
BOARD_EMSDP_CSRCS = $(call get_csrcs, $(BOARD_EMSDP_CSRCDIR))
BOARD_EMSDP_ASMSRCS = $(call get_asmsrcs, $(BOARD_EMSDP_ASMSRCDIR))

# get object files
BOARD_EMSDP_COBJS = $(call get_relobjs, $(BOARD_EMSDP_CSRCS))
BOARD_EMSDP_ASMOBJS = $(call get_relobjs, $(BOARD_EMSDP_ASMSRCS))
BOARD_EMSDP_OBJS = $(BOARD_EMSDP_COBJS) $(BOARD_EMSDP_ASMOBJS)

# get dependency files
BOARD_EMSDP_DEPS = $(call get_deps, $(BOARD_EMSDP_OBJS))

# extra macros to be defined
BOARD_EMSDP_DEFINES += $(CORE_DEFINES) $(DEV_DEFINES)

# genearte library
BOARD_LIB_EMSDP = $(OUT_DIR)/libboard_emsdp.a

# library generation rule
$(BOARD_LIB_EMSDP): $(BOARD_EMSDP_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(BOARD_EMSDP_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules


# Middleware Definitions
BOARD_INCDIR += $(BOARD_EMSDP_INCDIR)
BOARD_CSRCDIR += $(BOARD_EMSDP_CSRCDIR)
BOARD_ASMSRCDIR += $(BOARD_EMSDP_ASMSRCDIR)

BOARD_CSRCS += $(BOARD_EMSDP_CSRCS)
BOARD_CXXSRCS +=
BOARD_ASMSRCS += $(BOARD_EMSDP_ASMSRCS)
BOARD_ALLSRCS += $(BOARD_EMSDP_CSRCS) $(BOARD_EMSDP_ASMSRCS)

BOARD_COBJS += $(BOARD_EMSDP_COBJS)
BOARD_CXXOBJS +=
BOARD_ASMOBJS += $(BOARD_EMSDP_ASMOBJS)
BOARD_ALLOBJS += $(BOARD_EMSDP_OBJS)

BOARD_DEFINES += $(BOARD_EMSDP_DEFINES)
BOARD_DEPS += $(BOARD_EMSDP_DEPS)
BOARD_LIB = $(BOARD_LIB_EMSDP)