# dir declaration
MID_PARSON_DIR = $(MIDDLEWARES_ROOT)/parson

MID_PARSON_ASMSRCDIR	=
MID_PARSON_CSRCDIR	= $(MID_PARSON_DIR)
MID_PARSON_INCDIR	= $(MID_PARSON_DIR)

# find all the source files in the target directories
MID_PARSON_CSRCS = $(call get_csrcs, $(MID_PARSON_CSRCDIR))
MID_PARSON_ASMSRCS = $(call get_asmsrcs, $(MID_PARSON_ASMSRCDIR))

# get object files
MID_PARSON_COBJS = $(call get_relobjs, $(MID_PARSON_CSRCS))
MID_PARSON_ASMOBJS = $(call get_relobjs, $(MID_PARSON_ASMSRCS))
MID_PARSON_OBJS = $(MID_PARSON_COBJS) $(MID_PARSON_ASMOBJS)

# get dependency files
MID_PARSON_DEPS = $(call get_deps, $(MID_PARSON_OBJS))

# extra macros to be defined
MID_PARSON_DEFINES = -DMID_PARSON

# genearte library
MID_LIB_PARSON = $(OUT_DIR)/libmidparson.a

# library generation rule
$(MID_LIB_PARSON): $(MID_PARSON_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_PARSON_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_PARSON_INCDIR)
MID_CSRCDIR += $(MID_PARSON_CSRCDIR)
MID_ASMSRCDIR += $(MID_PARSON_ASMSRCDIR)

MID_CSRCS += $(MID_PARSON_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_PARSON_ASMSRCS)
MID_ALLSRCS += $(MID_PARSON_CSRCS) $(MID_PARSON_ASMSRCS)

MID_COBJS += $(MID_PARSON_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_PARSON_ASMOBJS)
MID_ALLOBJS += $(MID_PARSON_OBJS)

MID_DEFINES += $(MID_PARSON_DEFINES)
MID_DEPS += $(MID_PARSON_DEPS)
MID_LIBS += $(MID_LIB_PARSON)