# Application name
APPL ?= openthread_cli

# Extended Device Listcd
EXT_DEV_LIST += 802_15_4/mrf24j40

# Optimization Level
OLEVEL = O2

#
# root dir of embARC
#
EMBARC_ROOT = ../../../..

##
# HEAP & STACK SETTINGS
# For OpenThread Stack Usage
##
HEAPSZ ?= 81920
STACKSZ ?= 81920

# Select Middleware Packages
MID_SEL = common mbedtls openthread

# application source dirs
APPL_CSRC_DIR = .
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = .

# application defines
APPL_DEFINES = -DMBEDTLS_CONFIG_FILE=\"mbedtls-config.h\" \
		-DOPENTHREAD_CONFIG_FILE=\"openthread-windows-config.h\" \
		-DOPENTHREAD_FTD=1 \
		-DOPENTHREAD_ENABLE_CLI=1 \
		-DOPENTHREAD_ENABLE_CLI_MTD=0 \
		-DOPENTHREAD_ENABLE_CLI_FTD=1 \
		-DOPENTHREAD_ENABLE_APPLICATION_COAP=1 \
		-DOPENTHREAD_CLI_DEBUG_INFO=1

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

# add compile option, both supported by MW and GNU
CXX_COMPILE_OPT += -fno-rtti -fno-exceptions

### Options above must be added before include options.mk ###
# include key embARC build system makefile
include $(EMBARC_ROOT)/options/options.mk

