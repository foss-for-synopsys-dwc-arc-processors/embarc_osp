##
# \brief	current os directory definition
##
OS_UCOS_DIR = $(OSES_ROOT_DIR)/ucos

##
# \brief 		ucos port sources and includes definition
##
ifneq ($(LIB_SECURESHIELD_VERSION),)
$(error [os.ucos]: secureshield not support!!!)
else
OS_UCOS_PORT_CSRCDIR	= $(OS_UCOS_DIR)/Ports/Synopsys/ARC
OS_UCOS_PORT_ASMSRCDIR	= $(OS_UCOS_DIR)/Ports/Synopsys/ARC
OS_UCOS_PORT_INCDIR		= $(OS_UCOS_DIR)/Ports/Synopsys/ARC
endif
##
# \brief 		ucos os related source and header
##
OS_UCOS_CSRCDIR	= $(OS_UCOS_PORT_CSRCDIR) $(OS_UCOS_DIR)/uC-OS2/Source
OS_UCOS_ASMSRCDIR	= $(OS_UCOS_PORT_ASMSRCDIR) $(OS_UCOS_DIR)
OS_UCOS_INCDIR	= $(OS_UCOS_PORT_INCDIR) $(OS_UCOS_DIR)/uC-OS2/Source

# $(info [ucos.info]: OS_UCOS_INCDIR = $(OS_UCOS_INCDIR))

# find all the source files in the target directories
OS_UCOS_CSRCS_ALL = $(call get_csrcs, $(OS_UCOS_CSRCDIR))
# $(info [ucos.info]: OS_UCOS_CSRCS_ALL = $(OS_UCOS_CSRCS_ALL))
OS_UCOS_CSRCS_EXCLUDE = %ucos_ii.c
OS_UCOS_CSRCS = $(filter-out $(OS_UCOS_CSRCS_EXCLUDE),$(OS_UCOS_CSRCS_ALL))
# $(info [ucos.info]: OS_UCOS_CSRCS = $(OS_UCOS_CSRCS))

OS_UCOS_ASMSRCS = $(call get_asmsrcs, $(OS_UCOS_ASMSRCDIR))

# get object files
OS_UCOS_COBJS = $(call get_relobjs, $(OS_UCOS_CSRCS))
OS_UCOS_ASMOBJS = $(call get_relobjs, $(OS_UCOS_ASMSRCS))
OS_UCOS_OBJS = $(OS_UCOS_COBJS) $(OS_UCOS_ASMOBJS)

# get dependency files
OS_UCOS_DEPS = $(call get_deps, $(OS_UCOS_OBJS))

# extra macros to be defined
OS_UCOS_DEFINES = -DARC_FEATURE_STACK_CHECK=0 -DARC_FEATURE_UNALIGNED

ifeq ($(DEBUG), 1)
	OS_UCOS_DEFINES +=
endif

# genearte library
OS_LIB_UCOS = $(OUT_DIR)/libosucos.a

# library generation rule
$(OS_LIB_UCOS): $(OS_UCOS_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(OS_UCOS_OBJS)

# specific compile rules
# user can add rules to compile this OS
# if not rules specified to this OS, it will use default compiling rules

# OS Definitions
OS_INCDIR += $(OS_UCOS_INCDIR)
OS_CSRCDIR += $(OS_UCOS_CSRCDIR)
OS_ASMSRCDIR += $(OS_UCOS_ASMSRCDIR)

OS_CSRCS += $(OS_UCOS_CSRCS)
OS_CXXSRCS +=
OS_ASMSRCS += $(OS_UCOS_ASMSRCS)
OS_ALLSRCS += $(OS_UCOS_CSRCS) $(OS_UCOS_ASMSRCS)

OS_COBJS += $(OS_UCOS_COBJS)
OS_CXXOBJS +=
OS_ASMOBJS += $(OS_UCOS_ASMOBJS)
OS_ALLOBJS += $(OS_UCOS_OBJS)

OS_DEFINES += $(OS_UCOS_DEFINES)
OS_DEPS += $(OS_UCOS_DEPS)
OS_LIBS += $(OS_LIB_UCOS)
