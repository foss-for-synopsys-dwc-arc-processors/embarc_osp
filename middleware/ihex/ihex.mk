# dir declaration
MID_IHEX_DIR = $(MIDDLEWARES_ROOT)/ihex

MID_IHEX_ASMSRCDIR	=
MID_IHEX_CSRCDIR	= $(MID_IHEX_DIR)
MID_IHEX_INCDIR		= $(MID_IHEX_DIR)

# find all the source files in the target directories
MID_IHEX_CSRCS = $(call get_csrcs, $(MID_IHEX_CSRCDIR))
MID_IHEX_ASMSRCS = $(call get_asmsrcs, $(MID_IHEX_ASMSRCDIR))

# get object files
MID_IHEX_COBJS = $(call get_relobjs, $(MID_IHEX_CSRCS))
MID_IHEX_ASMOBJS = $(call get_relobjs, $(MID_IHEX_ASMSRCS))
MID_IHEX_OBJS = $(MID_IHEX_COBJS) $(MID_IHEX_ASMOBJS)

# get dependency files
MID_IHEX_DEPS = $(call get_deps, $(MID_IHEX_OBJS))

# extra macros to be defined
MID_IHEX_DEFINES = -DMID_IHEX

# genearte library
MID_LIB_IHEX = $(OUT_DIR)/libmidihex.a

# library generation rule
$(MID_LIB_IHEX): $(MID_IHEX_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_IHEX_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_IHEX_INCDIR)
MID_CSRCDIR += $(MID_IHEX_CSRCDIR)
MID_ASMSRCDIR += $(MID_IHEX_ASMSRCDIR)

MID_CSRCS += $(MID_IHEX_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_IHEX_ASMSRCS)
MID_ALLSRCS += $(MID_IHEX_CSRCS) $(MID_IHEX_ASMSRCS)

MID_COBJS += $(MID_IHEX_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_IHEX_ASMOBJS)
MID_ALLOBJS += $(MID_IHEX_OBJS)

MID_DEFINES += $(MID_IHEX_DEFINES)
MID_DEPS += $(MID_IHEX_DEPS)
MID_LIBS += $(MID_LIB_IHEX)