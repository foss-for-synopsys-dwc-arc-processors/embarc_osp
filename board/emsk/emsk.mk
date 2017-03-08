##
# \defgroup 	MK_BOARD_EMSK	EM Starter Kit Board Related Makefile Configurations
# \ingroup	MK_BOARD
# \brief	makefile related to emsk board configurations
##

##
# \brief	current board directory definition
##
BOARD_EMSK_DIR = $(BOARDS_ROOT)/emsk
##
# board version definition
# 10 stands for v1.0 (no support anymore)
# 11 stands for v1.1
# 20 stands for v2.0 (no support anymore)
# 21 stands for v2.1 (no support anymore)
# 22 stands for v2.2
# 23 stands for v2.3
# When you don't special BD_VER
# in any other place or through make options
# it will select a proper BD_VER according to your cur_core
##
SUPPORTED_BD_VERS = 11 22 23

## Select Board Version
BD_VER ?= 22

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

## If CUR_CORE is not in SUPPORTED_BD_VERS list, then force BD_VER and VALID_BD_VER to be 23
ifeq ($(TCFFILE_IS_VALID),1)
ifeq ($(VALID_BD_VER),)
override BD_VER := 23
override VALID_BD_VER := 23
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
BOARD_CORE_DIR = $(BOARD_EMSK_DIR)/configs/$(VALID_BD_VER)
COMMON_COMPILE_PREREQUISITES += $(BOARD_EMSK_DIR)/configs/core_configs.mk
include $(BOARD_EMSK_DIR)/configs/core_configs.mk

COMMON_COMPILE_PREREQUISITES += $(BOARD_EMSK_DIR)/configs/core_compiler.mk
include $(BOARD_EMSK_DIR)/configs/core_compiler.mk

## Board Related Settings
ifeq ($(VALID_BD_VER), 11)
OPENOCD_CFG_FILE = $(OPENOCD_SCRIPT_ROOT)/board/snps_em_sk_v1.cfg
else
OPENOCD_CFG_FILE = $(OPENOCD_SCRIPT_ROOT)/board/snps_em_sk_v2.2.cfg
endif
OPENOCD_OPTIONS  = -s $(OPENOCD_SCRIPT_ROOT) -f $(OPENOCD_CFG_FILE)
BOARD_EMSK_DEFINES += -DBOARD_SPI_FREQ=800000

## Build Rules
DEVICES_ROOT = $(EMBARC_ROOT)/device

##
# \brief	emsk device driver related
##
BOARD_EMSK_DEV_CSRCDIR		= $(DEVICES_ROOT)/designware/iic \
				$(DEVICES_ROOT)/designware/spi \
				$(DEVICES_ROOT)/designware/uart \
				$(DEVICES_ROOT)/designware/gpio \
				$(DEVICES_ROOT)/microchip/mrf24g \
				$(DEVICES_ROOT)/microchip/mrf24g/driver \
				$(DEVICES_ROOT)/rtthread/rw009

BOARD_EMSK_DEV_ASMSRCDIR	=
BOARD_EMSK_DEV_INCDIR		= $(DEVICES_ROOT)/designware/iic \
				$(DEVICES_ROOT)/designware/spi \
				$(DEVICES_ROOT)/designware/uart \
				$(DEVICES_ROOT)/designware/gpio \
				$(DEVICES_ROOT)/microchip/mrf24g \
				$(DEVICES_ROOT)/rtthread/rw009

##
# \brief	emsk board related source and header
##
BOARD_EMSK_CSRCDIR		+= $(BOARD_EMSK_DEV_CSRCDIR) $(BOARD_CORE_DIR) \
				$(BOARD_EMSK_DIR)/common \
				$(BOARD_EMSK_DIR)/drivers/mux \
				$(BOARD_EMSK_DIR)/drivers/uart \
				$(BOARD_EMSK_DIR)/drivers/iic \
				$(BOARD_EMSK_DIR)/drivers/spi \
				$(BOARD_EMSK_DIR)/drivers/gpio \
				$(BOARD_EMSK_DIR)/drivers/spiflash \
				$(BOARD_EMSK_DIR)/drivers/pmwifi \
				$(BOARD_EMSK_DIR)/drivers/temperature \
				$(BOARD_EMSK_DIR)/drivers/sdcard \
				$(BOARD_EMSK_DIR)/drivers/ntshell \
				$(BOARD_EMSK_DIR)/drivers/ble \
				$(BOARD_EMSK_DIR)/drivers/pmrf

BOARD_EMSK_ASMSRCDIR	+= $(BOARD_EMSK_DEV_ASMSRCDIR) $(BOARD_CORE_DIR)
BOARD_EMSK_INCDIR	+= $(BOARD_EMSK_DEV_INCDIR) $(BOARD_CORE_DIR)


# find all the source files in the target directories
BOARD_EMSK_CSRCS = $(call get_csrcs, $(BOARD_EMSK_CSRCDIR))
BOARD_EMSK_ASMSRCS = $(call get_asmsrcs, $(BOARD_EMSK_ASMSRCDIR))

# get object files
BOARD_EMSK_COBJS = $(call get_relobjs, $(BOARD_EMSK_CSRCS))
BOARD_EMSK_ASMOBJS = $(call get_relobjs, $(BOARD_EMSK_ASMSRCS))
BOARD_EMSK_OBJS = $(BOARD_EMSK_COBJS) $(BOARD_EMSK_ASMOBJS)

# get dependency files
BOARD_EMSK_DEPS = $(call get_deps, $(BOARD_EMSK_OBJS))

# extra macros to be defined
BOARD_EMSK_DEFINES += $(CORE_DEFINES)

# genearte library
BOARD_LIB_EMSK = $(OUT_DIR)/libboard_emsk.a

# library generation rule
$(BOARD_LIB_EMSK): $(BOARD_EMSK_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(BOARD_EMSK_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules


# Middleware Definitions
BOARD_INCDIR += $(BOARD_EMSK_INCDIR)
BOARD_CSRCDIR += $(BOARD_EMSK_CSRCDIR)
BOARD_ASMSRCDIR += $(BOARD_EMSK_ASMSRCDIR)

BOARD_CSRCS += $(BOARD_EMSK_CSRCS)
BOARD_CXXSRCS +=
BOARD_ASMSRCS += $(BOARD_EMSK_ASMSRCS)
BOARD_ALLSRCS += $(BOARD_EMSK_CSRCS) $(BOARD_EMSK_ASMSRCS)

BOARD_COBJS += $(BOARD_EMSK_COBJS)
BOARD_CXXOBJS +=
BOARD_ASMOBJS += $(BOARD_EMSK_ASMOBJS)
BOARD_ALLOBJS += $(BOARD_EMSK_OBJS)

BOARD_DEFINES += $(BOARD_EMSK_DEFINES)
BOARD_DEPS += $(BOARD_EMSK_DEPS)
BOARD_LIB = $(BOARD_LIB_EMSK)