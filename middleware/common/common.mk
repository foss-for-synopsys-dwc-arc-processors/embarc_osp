# directory declaration
MID_COMMON_DIR = $(MIDDLEWARES_ROOT)/common

MID_COMMON_ASMSRCDIR	= $(MID_COMMON_DIR) $(MID_COMMON_DIR)/ez_sio $(MID_COMMON_DIR)/ringbuffer $(MID_COMMON_DIR)/xprintf
MID_COMMON_CSRCDIR	= $(MID_COMMON_DIR) $(MID_COMMON_DIR)/ez_sio $(MID_COMMON_DIR)/ringbuffer $(MID_COMMON_DIR)/xprintf
MID_COMMON_INCDIR	= $(MID_COMMON_DIR) $(MID_COMMON_DIR)/ez_sio $(MID_COMMON_DIR)/ringbuffer $(MID_COMMON_DIR)/xprintf

# find all the source files in the target directories
MID_COMMON_CSRCS = $(call get_csrcs, $(MID_COMMON_CSRCDIR))
MID_COMMON_ASMSRCS = $(call get_asmsrcs, $(MID_COMMON_ASMSRCDIR))

# get object files
MID_COMMON_COBJS = $(call get_relobjs, $(MID_COMMON_CSRCS))
MID_COMMON_ASMOBJS = $(call get_relobjs, $(MID_COMMON_ASMSRCS))
MID_COMMON_OBJS = $(MID_COMMON_COBJS) $(MID_COMMON_ASMOBJS)

# get dependency files
MID_COMMON_DEPS = $(call get_deps, $(MID_COMMON_OBJS))

# extra macros to be defined
MID_COMMON_DEFINES = -DMID_COMMON

# genearte library
MID_LIB_COMMON = $(OUT_DIR)/libmidcommon.a


# library generation rule
$(MID_LIB_COMMON): $(MID_COMMON_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_COMMON_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_COMMON_INCDIR)
MID_CSRCDIR += $(MID_COMMON_CSRCDIR)
MID_ASMSRCDIR += $(MID_COMMON_ASMSRCDIR)

MID_CSRCS += $(MID_COMMON_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_COMMON_ASMSRCS)
MID_ALLSRCS += $(MID_COMMON_CSRCS) $(MID_COMMON_ASMSRCS)

MID_COBJS += $(MID_COMMON_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_COMMON_ASMOBJS)
MID_ALLOBJS += $(MID_COMMON_OBJS)

MID_DEFINES += $(MID_COMMON_DEFINES)
MID_DEPS += $(MID_COMMON_DEPS)
MID_LIBS += $(MID_LIB_COMMON)