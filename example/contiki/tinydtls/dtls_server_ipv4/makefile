# Application name
APPL ?= contiki_tinydtls_dtls_server_ipv4

# Selected OS
OS_SEL ?= contiki

# Contiki options
CONTIKI_WITH_IPV4 = 1

APPS += tinydtls/aes tinydtls/sha2 tinydtls/ecc tinydtls
APPL_DEFINES += -DUIP_CONF_TCP=0 -DDTLSv12 -DSHA2_USE_INTTYPES_H -DWITH_SHA256
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

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
include $(EMBARC_ROOT)/options/options.mk

