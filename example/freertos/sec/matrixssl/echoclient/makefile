# Application name
APPL ?= matrixssl_echoclient

# Heap Size Settings
HEAPSZ ?= 65536

# Optimization Level
# Please Refer to toolchain_xxx.mk for this option
ifeq "TOOLCHAIN" "mw"
OLEVEL ?= O0
endif

ifeq "TOOLCHAIN" "gnu"
OLEVEL ?= O2
endif

#
# root dir of embARC
#
EMBARC_ROOT = ../../../../..

OS_SEL = freertos
MID_SEL = common matrixssl fatfs lwip lwip-contrib

# application source dirs
APPL_CSRC_DIR = .
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = .

# application defines
APPL_DEFINES = -DUSE_CORE_TRACE -DHALT_ON_PS_ERROR -DMATRIX_USE_FILE_SYSTEM

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
include $(EMBARC_ROOT)/options/options.mk

