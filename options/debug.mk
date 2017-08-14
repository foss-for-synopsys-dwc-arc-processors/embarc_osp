##
# \brief	define different debug flags for different debug tools
#
#    notes:
#      The user may want to just run. Instead of passing -nogoifmain, can pass -cl -run.
#      When just running, do want -noptions.
##

##
# JTAG
# Select JTAG cable type
# Starter kit board supports two different JTAG cables:
#
#    usb     -JTAG over USB connection. A standard usb cable is used
#    opella  -Ashling Opella XD cable
##
SUPPORTED_JTAGS = usb opella
## Another define in options.mk
JTAG ?= usb
DIG_NAME ?=

override JTAG := $(strip $(JTAG))
override DIG_NAME := $(strip $(DIG_NAME))
override DIG_CHOICE := $(strip $(DIG_CHOICE))

## Set Valid JTAG
VALID_JTAG = $(call check_item_exist, $(JTAG), $(SUPPORTED_JTAGS))

## Try Check JTAG is valid
ifeq ($(VALID_JTAG), )
$(info JTAG - $(SUPPORTED_JTAGS) are supported)
$(error JTAG $(JTAG) is not supported, please check it!)
endif

ifneq ($(strip $(DIG_NAME)), )
DIGILENT_PROP += -prop=dig_device=$(strip $(DIG_NAME))
else
DIGILENT_PROP +=
endif

ifneq ($(strip $(DIG_CHOICE)), )
DIGILENT_PROP += -prop=dig_device_choice=$(strip $(DIG_CHOICE))
else
DIGILENT_PROP +=
endif

ifneq ($(strip $(DIG_SPEED)), )
DIGILENT_PROP += -prop=dig_speed=$(strip $(DIG_SPEED))
else
DIGILENT_PROP +=
endif

ifeq ($(DBG), mdb)
## Common options for mdb ##
DBG_HW_FLAGS += -nooptions -nogoifmain -toggle=include_local_symbols=1
ifeq ($(OS_SEL), freertos)
DBG_HW_FLAGS += -OS=FreeRTOS
endif
endif

ifneq ($(BOARD), nsim) # start of non-nsim case #

## Provide debugger options for Metaware Debugger
ifeq ($(DBG), mdb) # start of mdb case #
### Choose hardware as target
DBG_HW_FLAGS += -hard
ifeq ($(VALID_JTAG), opella)
	DBG_HW_FLAGS += -DLL=$(ASH_PATH) -prop=jtag_frequency=8MHz -prop=blast_frequency=6Mhz
else
ifeq ($(VALID_JTAG), usb)
	DBG_HW_FLAGS += -digilent $(DIGILENT_PROP) #-on=verify_download -on=spot_verify_download
endif
endif

endif # end of mdb case #

## Provide debugger options for gnu gdb debugger and openocd
ifeq ($(DBG), arc-elf32-gdb) # start of arc-elf32-gdb case #
DBG_GOALS = run gui
VALID_DBG_GOAL = $(call check_item_exist, $(MAKECMDGOALS), $(DBG_GOALS))

ifeq ($(VALID_JTAG), opella)
ifneq ($(VALID_DBG_GOAL), )
$(error arc-elf32-gdb does not support opella debugger!)
endif
else
ifneq ($(VALID_DBG_GOAL), )
###
## Check existence of openocd configuration file
## OPENOCD_CFG_FILE must be defined in board related makefile like board/emsk/emsk.mk
###
ifeq ($(wildcard $(OPENOCD_CFG_FILE)), )
$(info OPENOCD_SCRIPT_ROOT variable set in <embARC>/options/toolchain/toolchain_gnu.mk is $(OPENOCD_SCRIPT_ROOT))
$(info If OPENOCD_SCRIPT_ROOT variable value is not the path where your openocd script root locates, please remember to change it)
$(error $(OPENOCD_CFG_FILE) doesn't exist, please choose the right openocd configuration file path!)
endif
endif
ifeq ($(VALID_JTAG), usb)
DBG_HW_FLAGS += -ex "target remote | openocd --pipe $(OPENOCD_OPTIONS)" -ex "load"
endif
endif

endif # end of arc-elf32-gdb case #

endif # end of non-nsim case #

ifeq ($(BOARD), nsim) # start of nsim case #
## Provide debugger options for Metaware Debugger
ifeq ($(DBG), mdb)
DBG_HW_FLAGS += -nsim -off=binary_stdin -off=binary_stdout  -on=load_at_paddr\
		-on=reset_upon_restart -off=flush_pipe -off=cr_for_more -OKN  $(MDB_NSIM_OPT)
endif

ifeq ($(DBG), arc-elf32-gdb)
DBG_HW_FLAGS += -ex "target remote localhost:1234" -ex "load"
endif

endif # end of nsim case #

## Run target options
ifeq ($(MAKECMDGOALS),run)
ifeq ($(DBG), mdb)
DBG_HW_FLAGS += -run
endif
ifeq ($(DBG), arc-elf32-gdb)
ifeq ($(BOARD), nsim)
DBG = $(NSIMDRV)
DBG_HW_FLAGS =  -p nsim_emt=1 $(NSIMDRV_OPT)
else
DBG_HW_FLAGS += -ex "c"
endif
endif
endif