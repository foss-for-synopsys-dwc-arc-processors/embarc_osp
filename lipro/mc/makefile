# Application name
APPL ?= mc

#
# root dir of embARC
#
EMBARC_ROOT = ../..
ROOT ?= ..
LINKER_SCRIPT_FILE ?= $(ROOT)/common/linker_template_mw.ld

BOARD ?= nsim
CUR_CORE ?= archs

MID_SEL = common

# application source dirs
APPL_CSRC_DIR = .
APPL_CXXSRCS += $(ROOT)/common/shared_data.cpp

APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = .
APPL_INC_DIR += $(ROOT)/common

# name the app
APPL_DEFINES += -DAPPL_MC

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

print-%: ;  @echo $* = $($*)

### Options above must be added before include options.mk ###
# include key embARC build system makefile
include $(EMBARC_ROOT)/options/options.mk
