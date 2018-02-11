# Application name
APPL ?= audio_isr

EXT_DEV_LIST += audio/cs4344
##
# select debugging jtag
##
JTAG ?= usb

# Optimization Level
OLEVEL = O2

#
# root dir of embARC
#
EMBARC_ROOT = ../../..
MID_SEL = common

##
# Set toolchain
##
TOOLCHAIN ?= mw

##
# Current Board And Core
##
BOARD ?= huangshan

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