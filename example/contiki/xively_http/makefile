# Application name
APPL ?= contiki_xively_http

# Selected OS
OS_SEL ?= contiki

# Contiki options
CONTIKI_WITH_IPV4 = 1

# Heap Size Settings
HEAPSZ ?= 65536

# options for xively
XI_IO_LAYER ?= contiki
XI_NOB_ENABLED ?= true
XI_IO_LAYER_POSIX_COMPAT ?= 0
XI_DEBUG_PRINTF ?= xprintf
XI_ALLOCATOR_USE ?= stdlib

#
# root dir of embARC
#
EMBARC_ROOT = ../../..

# middlewares
MID_SEL = common xively

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
