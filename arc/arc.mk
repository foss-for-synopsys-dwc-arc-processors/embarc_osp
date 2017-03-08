## CPU Related ##
# CPU root declaration
CPUS_ROOT = $(EMBARC_ROOT)/arc

EXTRA_CSRCDIR += $(EMBARC_ROOT)/arc/startup
EXTRA_ASMSRCDIR += $(EMBARC_ROOT)/arc/startup

CPU_INCDIR =
CPU_CSRCDIR =
CPU_CXXSRCDIR =
CPU_ASMSRCDIR =

# source files and object files
CPU_CSRCS =
CPU_CXXSRCS =
CPU_ASMSRCS =
CPU_ALLSRCS =

CPU_COBJS =
CPU_CXXOBJS =
CPU_ASMOBJS =
CPU_ALLOBJS =

CPU_DEFINES =
CPU_DEPS =
CPU_LIBS =

#-----------------------#
## ARC Related ##
# directory declaration
CPU_ARC_DIR = $(CPUS_ROOT)

CPU_ARC_ASMSRCDIR	= $(CPU_ARC_DIR)
CPU_ARC_CSRCDIR		= $(CPU_ARC_DIR)
CPU_ARC_INCDIR		= $(CPU_ARC_DIR)

# find all the source files in the target directories
CPU_ARC_CSRCS = $(call get_csrcs, $(CPU_ARC_CSRCDIR))
CPU_ARC_ASMSRCS = $(call get_asmsrcs, $(CPU_ARC_ASMSRCDIR))

# get object files
CPU_ARC_COBJS = $(call get_relobjs, $(CPU_ARC_CSRCS))
CPU_ARC_ASMOBJS = $(call get_relobjs, $(CPU_ARC_ASMSRCS))
CPU_ARC_OBJS = $(CPU_ARC_COBJS) $(CPU_ARC_ASMOBJS)

# get dependency files
CPU_ARC_DEPS = $(call get_deps, $(CPU_ARC_OBJS))

# extra macros to be defined
CPU_ARC_DEFINES = -DCPU_ARC

# generate library
CPU_LIB_ARC = $(OUT_DIR)/libcpuarc.a

# library generation rule
$(CPU_LIB_ARC): $(CPU_ARC_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(CPU_ARC_OBJS)

# specific compile rules
# user can add rules to compile this library
# if not rules specified to this library, it will use default compiling rules

# CPU Definitions
CPU_INCDIR += $(CPU_ARC_INCDIR)
CPU_CSRCDIR += $(CPU_ARC_CSRCDIR)
CPU_ASMSRCDIR += $(CPU_ARC_ASMSRCDIR)

CPU_CSRCS += $(CPU_ARC_CSRCS)
CPU_CXXSRCS +=
CPU_ASMSRCS += $(CPU_ARC_ASMSRCS)
CPU_ALLSRCS += $(CPU_ARC_CSRCS) $(CPU_ARC_ASMSRCS)

CPU_COBJS += $(CPU_ARC_COBJS)
CPU_CXXOBJS +=
CPU_ASMOBJS += $(CPU_ARC_ASMOBJS)
CPU_ALLOBJS += $(CPU_ARC_OBJS)

CPU_DEFINES += $(CPU_ARC_DEFINES)
CPU_DEPS += $(CPU_ARC_DEPS)
CPU_LIBS += $(CPU_LIB_ARC)
#-----------------------#

# include dependency files
ifneq ($(MAKECMDGOALS),clean)
-include $(CPU_DEPS)
endif