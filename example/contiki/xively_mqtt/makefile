# Application name
APPL ?= contiki_xively_mqtt

EMSK_ID ?= 1

# Selected OS
OS_SEL ?= contiki

# Contiki options
CONTIKI_WITH_IPV4 = 1
APPS += mqtt-service
#
# root dir of embARC
#
EMBARC_ROOT = ../../..

# middlewares
MID_SEL = common

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
