# Application name
APPL ?= freertos_lwm2m_iotdemo

##
# HEAP & STACK SETTINGS
# For LwM2M Stack Usage
##
HEAPSZ ?= 81920
STACKSZ ?= 81920

#
# root dir of embARC
#
EMBARC_ROOT = ../../../../..

# Selected OS
OS_SEL ?= freertos

# Select Middleware Packages
MID_SEL = common lwip lwip-contrib wakaama fatfs parson u8glib

LWIP_CONTRIB_APPS ?=

# application source dirs
APPL_CSRC_DIR = . ./objects
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = . ./objects

# application defines
APPL_DEFINES = -DLWM2M_CLIENT_MODE

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
include $(EMBARC_ROOT)/options/options.mk