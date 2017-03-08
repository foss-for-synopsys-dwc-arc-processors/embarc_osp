# Application name
APPL ?= contiki_coap_server

EMSK_ID ?= 1

# Selected OS
OS_SEL ?= contiki

# Contiki options
CONTIKI_WITH_IPV4 = 1
APPL_DEFINES += -DREST=coap_rest_implementation
APPL_DEFINES += -DUIP_CONF_TCP=0
APPS += er-coap
APPS += rest-engine

APPL_DEFINES += -DEMSK_ID=$(EMSK_ID)

#
# root dir of embARC
#
EMBARC_ROOT = ../../..

# middlewares
MID_SEL = common

# application source dirs
APPL_CSRC_DIR = . ./resources
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = .

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
include $(EMBARC_ROOT)/options/options.mk
