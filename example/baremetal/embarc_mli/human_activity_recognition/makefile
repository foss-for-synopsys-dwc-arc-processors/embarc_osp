# Application name
APPL ?= mli_har

BOARD ?= iotdk

EXT_DEV_LIST += sensor/imu/mpu9250

CUR_CORE = arcem9d

APPL_DEFINES = -DUSE_APPL_MEM_CONFIG -DV2DSP_XY -DMODEL_BIT_DEPTH=16 -g

# optimization Level
OLEVEL = O0

#
# root dir of embARC
#
EMBARC_ROOT = ../../../..

# use -Hpurge option to optimize the code size
ifeq ($(TOOLCHAIN), gnu)
ADT_COPT += -ffunction-sections -fdata-sections
ADT_LOPT += -Wl,--gc-sections
else
ADT_COPT += -Hpurge
ADT_LOPT += -Hpurge
endif

LIB_SEL = embarc_mli
MID_SEL = common

# application source dirs
APPL_CSRC_DIR = . model
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = . model

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
include $(EMBARC_ROOT)/options/options.mk
