# Application name
APPL ?= contiki_ipv6_multicast_intermediate

EMSK_ID ?= 2

# Selected OS
OS_SEL ?= contiki

# Contiki related options
CONTIKI_WITH_IPV6 = 1
CONTIKI_WITH_RPL = 1

APPL_DEFINES += -DEMSK_ID=$(EMSK_ID) -DPROJECT_CONF_H=\"project-conf.h\"
MODULES += core/net/ipv6/multicast

#
# root dir of embARC
#
EMBARC_ROOT = ../../../../..

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

