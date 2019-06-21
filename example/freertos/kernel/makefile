# Application name
APPL ?= freertos_kernel

# Selected OS
OS_SEL ?= freertos

#
# root dir of embARC
#
EMBARC_ROOT = ../../..

MID_SEL = common

# for benchmark purporse, no stack check, no code density, no dsp and fpu
APPL_DEFINES = -DARC_FEATURE_STACK_CHECK=0 -DARC_FEATURE_FPU_DSP_CONTEXT=0 \
		-DARC_FEATURE_CODE_DENSITY=0

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
