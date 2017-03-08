##
# \defgroup 	MK_OS_OS_HAL		FreeRTOS Related Makefile Configurations
# \ingroup		MK_OS
# \brief		makefile related to os_hal os configurations
# @{
##

##
# \brief 		current os directory definition
##
OS_HAL_DIR = $(OSES_ROOT_DIR)/hal


##
# \brief 		os_hal os related source and header
##
OS_HAL_CSRCDIR		= $(OS_HAL_DIR)

OS_HAL_ASMSRCDIR	= $(OS_HAL_DIR)

OS_HAL_INCDIR		= $(OS_HAL_DIR)/inc

# find all the source files in the target directories
OS_HAL_CSRCS = $(call get_csrcs, $(OS_HAL_CSRCDIR))
OS_HAL_ASMSRCS = $(call get_asmsrcs, $(OS_HAL_ASMSRCDIR))

# get object files
OS_HAL_COBJS = $(call get_relobjs, $(OS_HAL_CSRCS))
OS_HAL_ASMOBJS = $(call get_relobjs, $(OS_HAL_ASMSRCS))
OS_HAL_OBJS = $(OS_HAL_COBJS) $(OS_HAL_ASMOBJS)

# get dependency files
OS_HAL_DEPS = $(call get_deps, $(OS_HAL_OBJS))

# extra macros to be defined
OS_HAL_DEFINES = -DOS_HAL

ifneq ($(strip $(OS_HAL_OBJS)),)
# genearte library
OS_LIB_HAL = $(OUT_DIR)/liboshal.a

# library generation rule
$(OS_LIB_HAL): $(OS_HAL_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(OS_HAL_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

endif

# Middleware Definitions
OS_INCDIR += $(OS_HAL_INCDIR)
OS_CSRCDIR += $(OS_HAL_CSRCDIR)
OS_ASMSRCDIR += $(OS_HAL_ASMSRCDIR)

OS_CSRCS += $(OS_HAL_CSRCS)
OS_CXXSRCS +=
OS_ASMSRCS += $(OS_HAL_ASMSRCS)
OS_ALLSRCS += $(OS_HAL_CSRCS) $(OS_HAL_ASMSRCS)

OS_COBJS += $(OS_HAL_COBJS)
OS_CXXOBJS +=
OS_ASMOBJS += $(OS_HAL_ASMOBJS)
OS_ALLOBJS += $(OS_HAL_OBJS)

OS_DEFINES += $(OS_HAL_DEFINES)
OS_DEPS += $(OS_HAL_DEPS)
OS_LIBS += $(OS_LIB_HAL)