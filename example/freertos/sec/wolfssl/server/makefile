# Application name
APPL ?= wolfssl_server

# Heap Size Settings
HEAPSZ ?= 1048576
STACKSZ ?= 1048576

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

MID_SEL = common wolfssl lwip lwip-contrib fatfs ntshell

# application source dirs
APPL_CSRC_DIR = .
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = .

# application defines (configuration for wolfssl library)
APPL_DEFINES = -DFREERTOS -DWOLFSSL_LWIP -DSINGLE_THREADED -DWOLFSSL_SMALL_STACK \
	       -DWOLFSSL_DTLS -DDEBUG_WOLFSSL -DWOLFSSL_STATIC_PSK -DWOLFSSL_STATIC_DH -DWOLFSSL_STATIC_RSA

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
include $(EMBARC_ROOT)/options/options.mk
