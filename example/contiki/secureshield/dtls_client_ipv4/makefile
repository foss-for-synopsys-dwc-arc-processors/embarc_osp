# Application name
APPL ?= contiki_secureshield_tinydtls_client_ipv4

BOARD = emsk
BD_VER = 22
CUR_CORE = arcem7d

# Selected OS
OS_SEL ?= contiki

# Contiki options
CONTIKI_WITH_IPV4 = 1

APPS += tinydtls/aes tinydtls/sha2 tinydtls/ecc tinydtls

APPL_DEFINES = -DUIP_CONF_TCP=0 -DSHA2_USE_INTTYPES_H -DWITH_SHA256 -DLIB_SECURESHIELD_OVERRIDES \
		-DUSE_SECURESHIELD_APPL_CONFIG


LIB_SEL = secureshield

#
# root dir of embARC
#
EMBARC_ROOT = ../../../..

# middlewares
MID_SEL = common

# application source dirs
APPL_CSRC_DIR = .
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = .

ifneq ($(TOOLCHAIN),gnu)
ADT_COPT = -Ml
else
ADT_COPT = -mlong-calls
endif

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
include $(EMBARC_ROOT)/options/options.mk

