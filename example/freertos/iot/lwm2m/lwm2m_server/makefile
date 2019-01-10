# Application name
APPL ?= freertos_iot_lwm2m_server

ifeq ($(BOARD), emsdp)
EXT_DEV_LIST += wifi/rs9113
else
EXT_DEV_LIST += wifi/mrf24g
endif

##
# HEAP & STACK SETTINGS
# For LwM2M Stack Usage
##
HEAPSZ ?= 8192
STACKSZ ?= 2048

ifeq ($(TOOLCHAIN), gnu)
OLEVEL = Os
endif

#
# root dir of embARC
#
EMBARC_ROOT = ../../../../..

# Selected OS
OS_SEL ?= freertos
# Select Middleware Packages
MID_SEL = common lwip lwip-contrib wakaama ntshell
LWIP_CONTRIB_APPS ?=

# application source dirs
APPL_CSRC_DIR = .
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = .

# application defines
APPL_DEFINES += -DLWM2M_SERVER_MODE -DNTSHELL_USE_CMDS_FS=0 \
	-DNTSHELL_USE_CMDS_ARC=0 -DNTSHELL_USE_CMDS_EXTEND=0 \
	-DNTSHELL_USE_CMDS_PERIPHERAL=0

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
include $(EMBARC_ROOT)/options/options.mk

