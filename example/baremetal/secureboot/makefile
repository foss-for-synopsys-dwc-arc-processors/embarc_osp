# Application name
APPL ?= emsk_secureboot

EXT_DEV_LIST += sensor/temperature/adt7420

##
# link script
#

APPL_DEFINES += -DENABLE_BANNER=0 -DUSE_APPL_MEM_CONFIG -DMCUBOOT_SIGN_RSA -DMCUBOOT_VALIDATE_SLOT0 -DMCUBOOT_USE_FLASH_AREA_GET_SECTORS -DMBEDTLS_CONFIG_FILE=\"config-boot.h\"

USE_BOARD_MAIN = 0

#
# root dir of embARC
#
EMBARC_ROOT = ../../..

# middlewares
MID_SEL = common ntshell ihex fatfs parson mbedtls

# application source dirs
APPL_CSRC_DIR = . ./mcuboot/bootutil/src
APPL_ASMSRC_DIR = .

# application include dirs
APPL_INC_DIR = . ./mcuboot/include ./mcuboot/bootutil/include

# include current project makefile
COMMON_COMPILE_PREREQUISITES += makefile

### Options above must be added before include options.mk ###
# include key embARC build system makefile
override EMBARC_ROOT := $(strip $(subst \,/,$(EMBARC_ROOT)))
include $(EMBARC_ROOT)/options/options.mk