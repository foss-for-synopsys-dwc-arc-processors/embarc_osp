# Application name
APPL ?= bmain

#
# root dir of embARC
#
EMBARC_ROOT = ../../..
ROOT ?= ..
LINKER_SCRIPT_FILE ?= $(ROOT)/common/linker_template_mw.ld

BOARD ?= nsim
CUR_CORE ?= archs
TCF=archs38_c0.tcf

MID_SEL = common

# application source dirs
APPL_CSRC_DIR = .
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = .

# name the app
APPL_DEFINES += -DAPPL_M0

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile
print-%: ;  @echo $* = $($*)

### Options above must be added before include options.mk ###
# include key embARC build system makefile
include $(EMBARC_ROOT)/options/options.mk
