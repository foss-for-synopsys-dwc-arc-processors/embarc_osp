# dir declaration
MID_QCBOR_DIR = $(MIDDLEWARES_ROOT)/qcbor

MID_QCBOR_ASMSRCDIR	=
MID_QCBOR_CSRCDIR	= $(MID_QCBOR_DIR)/src
MID_QCBOR_INCDIR	= $(MID_QCBOR_DIR)/inc

# find all the source files in the target directories
MID_QCBOR_CSRCS = $(call get_csrcs, $(MID_QCBOR_CSRCDIR))
MID_QCBOR_ASMSRCS = $(call get_asmsrcs, $(MID_QCBOR_ASMSRCDIR))

# get object files
MID_QCBOR_COBJS = $(call get_relobjs, $(MID_QCBOR_CSRCS))
MID_QCBOR_ASMOBJS = $(call get_relobjs, $(MID_QCBOR_ASMSRCS))
MID_QCBOR_OBJS = $(MID_QCBOR_COBJS) $(MID_QCBOR_ASMOBJS)

# get dependency files
MID_QCBOR_DEPS = $(call get_deps, $(MID_QCBOR_OBJS))

# extra macros to be defined
MID_QCBOR_DEFINES = -DMID_QCBOR

# genearte library
MID_LIB_QCBOR = $(OUT_DIR)/libmidqcbor.a

# library generation rule
$(MID_LIB_QCBOR): $(MID_QCBOR_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_QCBOR_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_QCBOR_INCDIR)
MID_CSRCDIR += $(MID_QCBOR_CSRCDIR)
MID_ASMSRCDIR += $(MID_QCBOR_ASMSRCDIR)

MID_CSRCS += $(MID_QCBOR_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_QCBOR_ASMSRCS)
MID_ALLSRCS += $(MID_QCBOR_CSRCS) $(MID_QCBOR_ASMSRCS)

MID_COBJS += $(MID_QCBOR_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_QCBOR_ASMOBJS)
MID_ALLOBJS += $(MID_QCBOR_OBJS)

MID_DEFINES += $(MID_QCBOR_DEFINES)
MID_DEPS += $(MID_QCBOR_DEPS)
MID_LIBS += $(MID_LIB_QCBOR)