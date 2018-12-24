# Application name
APPL ?= dma_spiflash
BOARD = emsk
EXT_DEV_LIST += flash/w25qxx
#
# root dir of embARC
#
EMBARC_ROOT = ../../..

MID_SEL = common

#APPL_DEFINES += -DUSE_APPL_MEM_CONFIG

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