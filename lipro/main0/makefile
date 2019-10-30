# Application name
APPL ?= main0

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

TX_DIR = $(EMBARC_ROOT)/3rd_party/threadx/tx
TX_INC_DIR = $(TX_DIR)
TX_LIB_DIR = $(TX_DIR)/build

# application include dirs
APPL_INC_DIR = .
APPL_INC_DIR += $(ROOT)/common
APPL_INC_DIR += $(TX_INC_DIR)

OS_LIBS += $(TX_LIB_DIR)/tx.a

# name the app
APPL_DEFINES += -DAPPL_M0

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile
print-%: ;  @echo $* = $($*)

### Options above must be added before include options.mk ###
# include key embARC build system makefile
include $(EMBARC_ROOT)/options/options.mk
