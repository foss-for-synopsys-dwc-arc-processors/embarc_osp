# Application name
APPL ?= mbedtls_dtls_client

BOARD ?= emsk

ifeq ($(BOARD), emsdp)
EXT_DEV_LIST += wifi/rs9113
else
EXT_DEV_LIST += wifi/mrf24g
endif

# Heap Size Settings
HEAPSZ ?= 8192
STACKSZ ?= 2048

APPL_DEFINES  = -DMBEDTLS_CONFIG_FILE=\"mbedtls_config.h\"

#
# root dir of embARC
#
EMBARC_ROOT = ../../../../../..

OS_SEL = freertos

MID_SEL = common ntshell mbedtls lwip lwip-contrib fatfs

# application source dirs
APPL_CSRC_DIR = .
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = .

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
include $(EMBARC_ROOT)/options/options.mk
