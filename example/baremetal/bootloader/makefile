# Application name
APPL ?= simple_bootloader

BOARD ?= emsk

OLEVEL ?= O2

# use mcuboot in bootloader
USE_MCUBOOT ?= 1

ifeq ($(BOARD), emsk)
EXT_DEV_LIST += sensor/temperature/adt7420
endif

# 16 KB stack
STACKSZ = 16384
HEAPSZ = 16384
##
# link script
#

APPL_DEFINES += -DENABLE_BANNER=0

USE_BOARD_MAIN = 0

#
# root dir of embARC
#
EMBARC_ROOT = ../../..

# middlewares
MID_SEL = common ntshell ihex fatfs parson

ifeq ($(USE_MCUBOOT), 1)
# middlewares
MID_SEL += mbedtls

# libraries
LIB_SEL += mcuboot

# decrease bootloader size
APPL_DEFINES += -DNTSHELL_USE_CMDS_ARC=0 \
				-DNTSHELL_USE_CMDS_PERIPHERAL=0 \
				-DFF_USE_LFN=0
endif

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