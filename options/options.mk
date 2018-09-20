##
# Special Default Make Goal to all
# so if no make goal specified,
# all will be the make goal,
# not the first target in the makefile
##
.DEFAULT_GOAL = all

## Force convert windows path to linux format
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))

## Default options settings ##
COMMON_COMPILE_PREREQUISITES += $(EMBARC_ROOT)/options/options.mk

##
# Output OBJS Root Directory
##
OUT_DIR_ROOT ?=

##
# Compile Toolchain
# Refer to toolchain.mk
##
TOOLCHAIN ?= mw

# Optimization Level
# Please Refer to toolchain_xxx.mk for this option
OLEVEL ?= O2

##
# additional options
# compile link assembler
##
ADT_COPT ?=
ADT_CXXOPT ?=
ADT_LOPT ?=
ADT_AOPT ?=

##
# Default HEAP SIZE & STACK SIZE
##
DEFAULT_HEAPSZ := 8192
DEFAULT_STACKSZ := 2048

##
# HEAP SIZE Set
##
HEAPSZ ?= 8192

##
# STACK SIZE Set
##
STACKSZ ?= 2048

##
# Debugger Select
# Refer to debug.mk
##
JTAG ?= usb

##
# Digilent JTAG Name Specify(Only for Metaware)
# This is especially useful if you have more than one
# Digilent device connected to your host.
# You can open digilent adept tool to see what digilent
# jtag is connected, leave this blank if don't know the name of digilent jtag
# I have see two digilent name: JtagHs1  TE0604-02
# Simple wrapper of -prop=dig_device=name option of Metaware Debugger(mdb)
##
DIG_NAME ?=

##
# Digilent JTAG Choice Select(Only for Metaware)
# Simple wrapper of -prop=dig_device_choice=N option of Metaware Debugger(mdb)
##
DIG_CHOICE ?=

##
# Set Digilent JTAG frequency(in Hz)(Only for Metaware)
# This is especially useful when you want to specify
# the digilent JTAG frequency when your board freq is quite low.
# Simple wrapper of -prop=dig_speed=SSS option of Metaware Debugger(mdb)
##
DIG_SPEED ?=

##
# DEBUG
# 1 for enable
# other for disable
##
DEBUG ?= 1

##
# generate map
# 1 for enable
# other for disable
##
MAP ?= 1

##
# Control Compiler Message Show
# 1: show compile total options
# 0: just show compile file info
##
V ?= 0

##
# Suppress All Messages
##
SILENT ?= 0

##
# Overwrite some configurations
# Never modify this if you don't know about it
##
override APPL := $(strip $(APPL))
override OUT_DIR_ROOT := $(strip $(OUT_DIR_ROOT))

override ADT_COPT := $(strip $(ADT_COPT))
override ADT_CXXOPT := $(strip $(ADT_CXXOPT))
override ADT_LOPT := $(strip $(ADT_LOPT))
override ADT_AOPT := $(strip $(ADT_AOPT))
override HEAPSZ := $(strip $(HEAPSZ))
override STACKSZ := $(strip $(STACKSZ))

GENE_TCF = arc.tcf
GENE_BCR_CONTENTS_TXT = bcr_contents.txt
##
# Argument files' filename generated from TCF
# Don't do any modification here
##
GENE_CCAC_ARG = ccac.arg
GENE_GCC_ARG = gcc.arg
GENE_NSIM_PROPS = nsim.props
GENE_MDB_ARG = mdb.arg
GENE_CORE_CONFIG_H = core_config.h
GENE_CORE_CONFIG_S = core_config.s
##
# Apex possiblely not generated
##
GENE_APEXEXTENSIONS_H = apexextensions.h
GENE_APEXEXTENSIONS_S = apexextensions.s

##
# Argument files not generated
##
GENE_LINK_CMD_TXT = mw_linker.ld
GENE_MEMORY_X = gnu_linker.ld

## File list which might need to be generated
GENE_FILE_LIST = $(GENE_TCF) $(GENE_CCAC_ARG) $(GENE_GCC_ARG) $(GENE_NSIM_PROPS) \
	$(GENE_MDB_ARG) $(GENE_CORE_CONFIG_H) $(GENE_CORE_CONFIG_S)

## Include Scripts and Functions ##
include $(EMBARC_ROOT)/options/scripts.mk
COMMON_COMPILE_PREREQUISITES += $(EMBARC_ROOT)/options/scripts.mk

##
# Output Directory Set
##
ifeq ($(strip $(OUT_DIR_ROOT)), )
OUT_DIR_PREFIX = obj_
else
### Check Root Out Directory Doesn't contain whitespace ##
ifneq ($(words $(OUT_DIR_ROOT)),1)
$(error PATH '$(OUT_DIR_ROOT)' contains whitespace, not supported!)
else
OUT_DIR_ROOT_FIXPATH = $(subst \,/, $(strip $(OUT_DIR_ROOT)))
OUT_DIR_ROOT_FIXPATH_EXIST = $(realpath $(OUT_DIR_ROOT_FIXPATH))
ifeq ($(OUT_DIR_ROOT_FIXPATH_EXIST),)
$(error PATH '$(OUT_DIR_ROOT)' doesn't exist, please check it!)
else
override OUT_DIR_ROOT := $(OUT_DIR_ROOT_FIXPATH)
OUT_DIR_PREFIX = $(strip $(OUT_DIR_ROOT))/obj_
endif
endif
endif
## Board Infomation
BOARD_INFO = $(strip $(BOARD))_$(strip $(BD_VER))
## Build Infomation
BUILD_INFO = $(strip $(TOOLCHAIN))_$(strip $(CUR_CORE))
## Selected Configuration
SELECTED_CONFIG=$(BOARD_INFO)-$(BUILD_INFO)

## Objects Output Directory
BOARD_OUT_DIR = $(OUT_DIR_PREFIX)$(BOARD_INFO)
OUT_DIR = $(BOARD_OUT_DIR)/$(BUILD_INFO)

##
# Application Path and Name Setting
##
APPL_NAME = $(strip $(APPL)_$(BUILD_INFO))
APPL_FULL_NAME = $(strip $(OUT_DIR)/$(APPL_NAME))
APPL_OUT_DIR = $(OUT_DIR)/application

##
# Generated directory - contains generated files
# Such as metaware, arc gnu, nsim argument files, generated link file
##
EMBARC_GENERATED_DIR = $(OUT_DIR)/embARC_generated

##
# Application Link file definition
##
APPL_LINK_FILE = $(strip $(OUT_DIR)/linker_$(TOOLCHAIN).ldf)

################## Pre processing #########################################
#   INCLUDE target specific configuration makefiles
COMMON_COMPILE_PREREQUISITES += $(EMBARC_ROOT)/options/rules.mk
COMMON_COMPILE_PREREQUISITES += $(EMBARC_ROOT)/options/toolchain.mk
COMMON_COMPILE_PREREQUISITES += $(EMBARC_ROOT)/options/debug.mk
COMMON_COMPILE_PREREQUISITES += $(EMBARC_ROOT)/options/files.mk
COMMON_COMPILE_PREREQUISITES += $(EMBARC_ROOT)/arc/arc.mk
COMMON_COMPILE_PREREQUISITES += $(EMBARC_ROOT)/board/board.mk

# include toolchain settings
include $(EMBARC_ROOT)/options/toolchain.mk

################# Components of embARC ###################################
# board specific settings
include $(EMBARC_ROOT)/board/board.mk

# ARC HAL
include $(EMBARC_ROOT)/arc/arc.mk

# library makefile
include $(EMBARC_ROOT)/library/library.mk

# os specific settings
ifdef OS_SEL
override OS_SEL := $(strip $(OS_SEL))
COMMON_COMPILE_PREREQUISITES += $(EMBARC_ROOT)/os/os.mk
include $(EMBARC_ROOT)/os/os.mk
endif

# middleware makefile
include $(EMBARC_ROOT)/middleware/middleware.mk

############## Post processing #############################################
# source directories and include directories settings
include $(EMBARC_ROOT)/options/files.mk

# include debug settings
include $(EMBARC_ROOT)/options/debug.mk

##
# Include Compiler / Linker / Other Rules
#
include $(EMBARC_ROOT)/options/rules.mk
