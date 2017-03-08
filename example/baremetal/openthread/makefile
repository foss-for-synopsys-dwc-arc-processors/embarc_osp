# Application name
APPL ?= openthread

# Optimization Level
OLEVEL = O2

#
# root dir of embARC
#
EMBARC_ROOT = ../../..

# Select Middleware Packages
MID_SEL = common mbedtls openthread

# application source dirs
APPL_CSRC_DIR = .
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = .

# application defines
APPL_DEFINES = -DMBEDTLS_CONFIG_FILE=\"mbedtls_config.h\" -DOPENTHREAD_ENABLE_MAC_WHITELIST=1

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

# add compile option, both supported by MW and GNU
CXX_COMPILE_OPT += -fno-rtti -fno-exceptions

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
include $(EMBARC_ROOT)/options/options.mk

