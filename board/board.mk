##
# \defgroup	MK_BOARD	Board Makefile Configurations
# \brief	makefile related to board configurations
##

# boards root declaration
BOARDS_ROOT = $(EMBARC_ROOT)/board

##
# BOARD
# select the target board
# scan the sub-dirs of board to get the supported boards
SUPPORTED_BOARDS = $(basename $(notdir $(wildcard $(BOARDS_ROOT)/*/*.mk)))
BOARD ?= emsk
USE_BOARD_MAIN ?= 1

override BOARD := $(strip $(BOARD))
override USE_BOARD_MAIN := $(strip $(USE_BOARD_MAIN))
override WIFI_SEL := $(strip $(WIFI_SEL))

BOARD_CSRCDIR	+= $(BOARDS_ROOT)
BOARD_ASMSRCDIR	+= $(BOARDS_ROOT)
BOARD_INCDIR	+= $(BOARDS_ROOT)

ifeq ($(USE_BOARD_MAIN), 1)
BOARD_MAIN_DEFINES = -DEMBARC_USE_BOARD_MAIN
else
BOARD_MAIN_DEFINES =
endif

EXTRA_BOARD_DEFINES += $(BOARD_MAIN_DEFINES)

ifeq ($(VALID_TOOLCHAIN), mw)
LINKER_SCRIPT_FILE ?= $(BOARDS_ROOT)/linker_template_mw.ld
else
LINKER_SCRIPT_FILE ?= $(BOARDS_ROOT)/linker_template_gnu.ld
endif

## CPU_FREQ & DEV_FREQ defined in each board support file ##

## Set Valid Board
VALID_BOARD = $(call check_item_exist, $(BOARD), $(SUPPORTED_BOARDS))

## Try Check BOARD is valid
ifeq ($(VALID_BOARD), )
$(info BOARD - $(SUPPORTED_BOARDS) are supported)
$(error BOARD $(BOARD) is not supported, please check it!)
endif

#board definition
BOARD_ID = $(call uc,BOARD_$(VALID_BOARD))
#device usage settings
#must be before include
COMMON_COMPILE_PREREQUISITES += $(BOARDS_ROOT)/$(VALID_BOARD)/$(VALID_BOARD).mk
include $(BOARDS_ROOT)/$(VALID_BOARD)/$(VALID_BOARD).mk

##
# \brief	add defines for board
##
BOARD_DEFINES += $(EXTRA_BOARD_DEFINES) -D$(BOARD_ID) -DHW_VERSION=$(BD_VER)
ifneq ($(CPU_FREQ), )
BOARD_DEFINES += -DBOARD_CPU_FREQ=$(CPU_FREQ)
endif
ifneq ($(DEV_FREQ), )
BOARD_DEFINES += -DBOARD_DEV_FREQ=$(DEV_FREQ)
endif

# extra directories need to be compiled
EXTRA_CSRCDIR += $(BOARDS_ROOT)

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
-include $(BOARD_DEPS)
endif