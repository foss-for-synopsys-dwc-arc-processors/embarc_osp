# Application name
APPL ?= mbedtls_ssl_server2

BOARD ?= emsk

ifeq ($(BOARD), emsdp)
EXT_DEV_LIST += wifi/rs9113
else
EXT_DEV_LIST += wifi/mrf24g sensor/temperature/adt7420
endif

# Heap Size Settings
HEAPSZ ?= 1024*96
STACKSZ ?= 2048

APPL_DEFINES  += -DNTCONF_EDITOR_MAXLEN=256 -DMBEDTLS_CONFIG_FILE=\"mbedtls_config.h\" \
				 -DTASK_STACK_SIZE_WIFI=1024 \
				 -DTASK_STACK_SIZE_NTSHELL=4096 \
				 -DNTSHELL_USE_CMDS_ARC=0 \
				 -DNTSHELL_USE_CMDS_PERIPHERAL=0

#
# root dir of embARC
#
EMBARC_ROOT = ../../../../../..

OS_SEL = freertos

MID_SEL = common ntshell mbedtls fatfs lwip lwip-contrib

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
