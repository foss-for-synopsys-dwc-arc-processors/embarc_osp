# Application name
APPL ?= arc_feature_udma

# Heap Size Settings
HEAPSZ ?= 20480

#
# root dir of embARC
#
EMBARC_ROOT = ../../../..

OS_SEL =
MID_SEL = common

#APPL_DEFINES += -DUSE_APPL_MEM_CONFIG

# application source dirs
APPL_CSRC_DIR = .
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = .

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
include $(EMBARC_ROOT)/options/options.mk
