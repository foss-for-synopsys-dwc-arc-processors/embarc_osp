##
# \defgroup	MK_OS_FREERTOS	FreeRTOS Related Makefile Configurations
# \ingroup	MK_OS
# \brief	makefile related to freertos os configurations
##

##
# \brief	current os directory definition
##
OS_FREERTOS_DIR = $(OSES_ROOT_DIR)/freertos
FREERTOS_HEAP_SEL = 2

##
# \brief 		freertos port sources and includes definition
##
ifeq ($(LIB_SECURESHIELD_VERSION), 2)
OS_FREERTOS_PORT_CSRCDIR	= $(OS_FREERTOS_DIR)/portable/Synopsys/ARC_EM_SECURESHIELD \
				$(OS_FREERTOS_DIR)/portable
OS_FREERTOS_PORT_ASMSRCDIR	= $(OS_FREERTOS_DIR)/portable/Synopsys/ARC_EM_SECURESHIELD
OS_FREERTOS_PORT_INCDIR		= $(OS_FREERTOS_DIR)/portable/Synopsys/ARC_EM_SECURESHIELD
else
OS_FREERTOS_PORT_CSRCDIR	= $(OS_FREERTOS_DIR)/portable/Synopsys/ARC \
				$(OS_FREERTOS_DIR)/portable
OS_FREERTOS_PORT_ASMSRCDIR	= $(OS_FREERTOS_DIR)/portable/Synopsys/ARC
OS_FREERTOS_PORT_INCDIR		= $(OS_FREERTOS_DIR)/portable/Synopsys/ARC
endif
##
# \brief 		freertos os related source and header
##
OS_FREERTOS_CSRCDIR	= $(OS_FREERTOS_PORT_CSRCDIR) $(OS_FREERTOS_DIR)
OS_FREERTOS_ASMSRCDIR	= $(OS_FREERTOS_PORT_ASMSRCDIR) $(OS_FREERTOS_DIR)
OS_FREERTOS_INCDIR	= $(OS_FREERTOS_PORT_INCDIR) $(OS_FREERTOS_DIR)/include

# find all the source files in the target directories
OS_FREERTOS_CSRCS = $(call get_csrcs, $(OS_FREERTOS_CSRCDIR))
OS_FREERTOS_ASMSRCS = $(call get_asmsrcs, $(OS_FREERTOS_ASMSRCDIR))

# get object files
OS_FREERTOS_COBJS = $(call get_relobjs, $(OS_FREERTOS_CSRCS))
OS_FREERTOS_ASMOBJS = $(call get_relobjs, $(OS_FREERTOS_ASMSRCS))
OS_FREERTOS_OBJS = $(OS_FREERTOS_COBJS) $(OS_FREERTOS_ASMOBJS)

# get dependency files
OS_FREERTOS_DEPS = $(call get_deps, $(OS_FREERTOS_OBJS))

# extra macros to be defined
OS_FREERTOS_DEFINES = -DFREERTOS_HEAP_SEL=$(FREERTOS_HEAP_SEL) \
		-DconfigNUM_THREAD_LOCAL_STORAGE_POINTERS=1 -DconfigUSE_RECURSIVE_MUTEXES=1

ifeq ($(DEBUG), 1)
	OS_FREERTOS_DEFINES += -DconfigGENERATE_RUN_TIME_STATS=1
endif

# genearte library
OS_LIB_FREERTOS = $(OUT_DIR)/libosfreertos.a

# library generation rule
$(OS_LIB_FREERTOS): $(OS_FREERTOS_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(OS_FREERTOS_OBJS)

# specific compile rules
# user can add rules to compile this OS
# if not rules specified to this OS, it will use default compiling rules

# OS Definitions
OS_INCDIR += $(OS_FREERTOS_INCDIR)
OS_CSRCDIR += $(OS_FREERTOS_CSRCDIR)
OS_ASMSRCDIR += $(OS_FREERTOS_ASMSRCDIR)

OS_CSRCS += $(OS_FREERTOS_CSRCS)
OS_CXXSRCS +=
OS_ASMSRCS += $(OS_FREERTOS_ASMSRCS)
OS_ALLSRCS += $(OS_FREERTOS_CSRCS) $(OS_FREERTOS_ASMSRCS)

OS_COBJS += $(OS_FREERTOS_COBJS)
OS_CXXOBJS +=
OS_ASMOBJS += $(OS_FREERTOS_ASMOBJS)
OS_ALLOBJS += $(OS_FREERTOS_OBJS)

OS_DEFINES += $(OS_FREERTOS_DEFINES)
OS_DEPS += $(OS_FREERTOS_DEPS)
OS_LIBS += $(OS_LIB_FREERTOS)
