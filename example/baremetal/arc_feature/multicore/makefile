# Application name
APPL ?= arc_smp

BOARD ?= nsim
CUR_CORE ?= archs38_smp
TOOLCHAIN = mw


APPL_DEFINES  = -DUSE_APPL_MEM_CONFIG -DARC_FEATURE_MP_NUM_CPUS=2
##
# select debugging jtag
##
JTAG ?= usb

#
# root dir of embARC
#
EMBARC_ROOT = ../../../..

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

# rules for multicore application
export NSIM_MULTICORE=1
mp_run : $(APPL_FULL_NAME).elf
	@$(ECHO) "Download & Run $<"
	$(DBG) -cmpd=soc -pset=1 -psetname=core0 -prop=ident=0x52 $(DBG_HW_FLAGS) $<
	$(DBG) -cmpd=soc -pset=2 -psetname=core1 -prop=ident=0x152 $(DBG_HW_FLAGS) $<
	$(DBG) -multifiles=core0,core1 -cmpd=soc -run $< $(CMD_LINE)

mp_gui : $(APPL_FULL_NAME).elf
	@$(ECHO) "Download & Debug $<"
	$(DBG) -cmpd=soc -pset=1 -psetname=core0 -prop=ident=0x52 $(DBG_HW_FLAGS) $<
	$(DBG) -cmpd=soc -pset=2 -psetname=core1 -prop=ident=0x152 $(DBG_HW_FLAGS) $<
	$(DBG) -multifiles=core0,core1 -cmpd=soc -OKN $< $(CMD_LINE)
