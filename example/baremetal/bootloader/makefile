# Application name
APPL ?= emsk_bootloader

EXT_DEV_LIST += sensor/temperature/adt7420

##
# link script
#

APPL_DEFINES += -DENABLE_BANNER=0 -DUSE_APPL_MEM_CONFIG

USE_BOARD_MAIN = 0

#
# root dir of embARC
#
EMBARC_ROOT = ../../..

# middlewares
MID_SEL = common ntshell ihex fatfs parson

# application source dirs
APPL_CSRC_DIR = .
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = .

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
include $(EMBARC_ROOT)/options/options.mk