##
# Batch processing makefile (Cygwin version)
# Note: enter make in Terminal and it will make file one by one.
# If any errors occur, the makefile process stop.
# You can enter make -k to ignore errors and keep going.
##

##
# Optimization Level
# Please Refer to toolchain_xxx.mk for this option
##
OLEVEL ?=

##
# Select Which Board to be built
# Refer to board.mk
##
BOARD ?=

##
# Select which board version to use
# Refer to board/xxx/xxx.mk
##
BD_VER ?=

##
# Select the operation cpu core
##
CUR_CORE ?=

##
# Select ToolChain to build
# Refer to toolchain.mk
##
TOOLCHAIN ?=

##
# Definition of Tool Paths
##
MW_TOOLPATH  ?=
GNU_TOOLPATH ?=

##
# Make Tool Definition
##
MAKE := make
ECHO := echo
##
# Search makefile in current directory
##
ifdef LINUX_FIND
DIRS_m := $(shell find . -mindepth 2 -type f -name "makefile")
DIRS_m := $(patsubst ./%, %, $(DIRS_m))
DIRS_m := $(patsubst %/makefile, %, $(DIRS_m))

DIRS_M := $(shell find . -mindepth 2 -type f -name "Makefile")
DIRS_M := $(patsubst ./%, %, $(DIRS_M))
DIRS_M := $(patsubst %/Makefile, %, $(DIRS_M))
else
##
# Support directory depth from 1 - 5
# if you want to add more depth support
# you need to add ./*/*/*/* ... /* like to DIRS_TO_FIND
##
DIRS_TO_FIND := ./* ./*/* ./*/*/* ./*/*/*/* ./*/*/*/*/*
DIRS_m := $(foreach dir2find, $(DIRS_TO_FIND), $(sort $(dir $(wildcard $(dir2find)/makefile))))
DIRS_M := $(foreach dir2find, $(DIRS_TO_FIND), $(sort $(dir $(wildcard $(dir2find)/Makefile))))
#$(sort $(dir $(wildcard ./*/Makefile ./*/*/Makefile ./*/*/*/Makefile)))
endif

##
# Get all directories containing makefile
##
DIRS := $(sort $(DIRS_m) $(DIRS_M))

##
# Get Build, Clean, Bin Objects
##
BUILD_DIRS	:= $(addprefix _BUILD_, $(DIRS))
CLEAN_DIRS	:= $(addprefix _CLEAN_, $(DIRS))
DIST_CLEAN_DIRS	:= $(addprefix _DIST_CLEAN_, $(DIRS))
BIN_DIRS	:= $(addprefix _BIN_, $(DIRS))

##
# Make file automatically and generate testlist of makefile
##

SILENT ?= 1
KEEP ?= 1


MAKE_CFG_T :=
MAKE_OPT_T :=
BUILD_CFG_T :=

##
# Makefile Options Definitions
##
ifeq ($(SILENT), 1)
	MAKE_OPT_T += -s
	MAKE_CFG_T += SILENT=1
endif

ifeq ($(KEEP), 1)
	MAKE_OPT_T += -k
endif

##
# Makefile Configuration Passed
##

ifneq ($(BOARD), )
	MAKE_CFG_T += BOARD=$(BOARD)
	BUILD_CFG_T += "${BOARD}"
endif

ifneq ($(BD_VER), )
	MAKE_CFG_T += BD_VER=$(BD_VER)
	BUILD_CFG_T += "- ${BD_VER}"
endif

ifneq ($(CUR_CORE), )
	MAKE_CFG_T += CUR_CORE=$(CUR_CORE)
	BUILD_CFG_T += "- ${CUR_CORE}"
endif

ifneq ($(OLEVEL), )
	MAKE_CFG_T += OLEVEL=$(OLEVEL)
	BUILD_CFG_T += "- ${OLEVEL}"
endif

ifneq ($(TOOLCHAIN), )
	MAKE_CFG_T += TOOLCHAIN=$(TOOLCHAIN)
	BUILD_CFG_T += "- ${TOOLCHAIN}"
endif

ifneq ($(OUT_DIR_ROOT), )
	MAKE_CFG_T += OUT_DIR_ROOT=$(OUT_DIR_ROOT)
endif

ifneq ($(MW_TOOLPATH), )
	MAKE_CFG_T += MW_TOOLPATH=$(MW_TOOLPATH)
endif

ifneq ($(GNU_TOOLPATH), )
	MAKE_CFG_T += GNU_TOOLPATH=$(GNU_TOOLPATH)
endif

##
# Output OBJS Directory
# Should Not Change This
##
OUT_DIR_ROOT :=
##
# Output Directory
##
ifeq ($(strip $(OUT_DIR_ROOT)), )
OUT_DIR_PREFIX = obj_
else
OUT_DIR_PREFIX = $(strip $(OUT_DIR_ROOT))/obj_
endif
## Board Information
BOARD_INFO = $(strip $(BOARD))_$(strip $(BD_VER))
## Build Information
BUILD_INFO = $(strip $(TOOLCHAIN))_$(strip $(CUR_CORE))
## Selected Configuration
SELECTED_CONFIG=$(BOARD_INFO)-$(BUILD_INFO)

## Objects Output Directory
BOARD_OUT_DIR = $(OUT_DIR_PREFIX)$(BOARD_INFO)
OUT_DIR = $(BOARD_OUT_DIR)/$(BUILD_INFO)

##
# Build Result, It should generate an *.elf file
##
OBJ_DIRS := $(addsuffix $(OUT_DIR)/, $(DIRS))
MAKE_ELF_RESULT :=  $(foreach dir2find, $(OBJ_DIRS), $(wildcard $(dir2find)*.elf) )
MAKE_BIN_RESULT :=  $(foreach dir2find, $(OBJ_DIRS), $(wildcard $(dir2find)*.bin) )
## Build successfully or complete dirs result ##
## CP means ComPlete ##
MAKE_DIR_CP_RESULT := $(patsubst %$(OUT_DIR)/, %, $(sort $(dir $(MAKE_ELF_RESULT))))
## not build successfully or not complete dirs result ##
## NC means Not Complete ##
MAKE_DIR_NC_RESULT := $(patsubst %$(OUT_DIR)/, %, $(filter-out $(dir $(MAKE_ELF_RESULT)), $(dir $(OBJ_DIRS))))

##
# Make Rules
##
.PHONY : all bin clean distclean info opt status help

all : $(BUILD_DIRS)

clean : $(CLEAN_DIRS)

distclean : $(DIST_CLEAN_DIRS)

bin : $(BIN_DIRS)

help :
	@$(ECHO) 'Build Targets:'
	@$(ECHO) '  all         - Build all examples in current directory'
	@$(ECHO) '  bin         - Build and Generate binary for all examples'
	@$(ECHO) 'Clean Targets:'
	@$(ECHO) '  clean       - Remove object files of selected configuration'
	@$(ECHO) '  distclean   - Remove object files of all configurations'
	@$(ECHO) 'Other Targets:'
	@$(ECHO) '  info        - Display examples which will be built'
	@$(ECHO) '  opt         - Display MAKE options passed to sub make'
	@$(ECHO) '  status      - Display build status of current configuration'
	@$(ECHO) 'Available Configurations:'
	@$(ECHO) '  OLEVEL=Os|O0|O1|O2|O3                       - Optimization Level of examples to be built'
	@$(ECHO) '  CUR_CORE=arcem4|arcem4cr16|arcem6|arcem6gp|arcem7d|arcem5d|arcem7dfpu|<more cfgs> - Current core configuration'
	@$(ECHO) '  TOOLCHAIN=mw|gnu                            - Current selected compiling toolchain'
	@$(ECHO) '  SILENT=0|1                                  - Disable or enable message Output'
	@$(ECHO) 'Example Usages:'
	@$(ECHO) '  gmake all                                               - build all examples in current directory using default configuration'
	@$(ECHO) '  gmake OLEVEL=Os CUR_CORE=arcem6gp TOOLCHAIN=gnu all     - build examples using configuration(Os, arcem6gp, gnu)'

info :
	@$(ECHO) There are $(words $(DIRS)) examples to be built
	@$(ECHO) Here is the directory list to be operated :  $(DIRS)

status :
	@$(ECHO) Build Status: ${BUILD_CFG_T}
	@$(ECHO) There are $(words $(DIRS)) examples
	@$(ECHO) ==== BUILD SUCCESS RESULT ====
	@$(ECHO) There are $(words $(MAKE_DIR_CP_RESULT)) examples built complete
	@$(ECHO) $(MAKE_DIR_CP_RESULT)
	@$(ECHO) ==== BUILD FAILED OR NOT COMPLETED DIRECTORIES ====
	@$(ECHO) There are $(words $(MAKE_DIR_NC_RESULT)) examples built not complete or failed
	@$(ECHO) $(MAKE_DIR_NC_RESULT)
	@$(ECHO) ==== BIN BUILD RESULT ====
	@$(ECHO) There are $(words $(MAKE_BIN_RESULT)) examples bin generated complete
	@$(ECHO) $(MAKE_BIN_RESULT)

opt:
	@$(ECHO) MAKE_CFG_T : $(MAKE_CFG_T)
	@$(ECHO) MAKE_OPT_T : $(MAKE_OPT_T)

$(BUILD_DIRS) :
	$(MAKE) $(MAKE_OPT_T) $(MAKE_CFG_T) -C $(patsubst _BUILD_%, %, $@)	all

$(CLEAN_DIRS) :
	$(MAKE) $(MAKE_OPT_T) $(MAKE_CFG_T) -C $(patsubst _CLEAN_%, %, $@)	clean

$(DIST_CLEAN_DIRS) :
	$(MAKE) $(MAKE_OPT_T) $(MAKE_CFG_T) -C $(patsubst _DIST_CLEAN_%, %, $@)	distclean

$(BIN_DIRS) :
	$(MAKE) $(MAKE_OPT_T) $(MAKE_CFG_T) -C $(patsubst _BIN_%, %, $@)	bin

