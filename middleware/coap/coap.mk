# dir declaration
MID_COAP_DIR = $(MIDDLEWARES_ROOT)/coap

MID_COAP_ASMSRCDIR	= $(MID_COAP_DIR)
MID_COAP_CSRCDIR	= $(MID_COAP_DIR)
MID_COAP_INCDIR		= $(MID_COAP_DIR)

# find all the source files in the target directories
MID_COAP_CSRCS = $(call get_csrcs, $(MID_COAP_CSRCDIR))
MID_COAP_ASMSRCS = $(call get_asmsrcs, $(MID_COAP_ASMSRCDIR))

# get object files
MID_COAP_COBJS = $(call get_relobjs, $(MID_COAP_CSRCS))
MID_COAP_ASMOBJS = $(call get_relobjs, $(MID_COAP_ASMSRCS))
MID_COAP_OBJS = $(MID_COAP_COBJS) $(MID_COAP_ASMOBJS)

# get dependency files
MID_COAP_DEPS = $(call get_deps, $(MID_COAP_OBJS))

# extra macros to be defined
MID_COAP_DEFINES = -DMID_COAP

# genearte library
MID_LIB_COAP = $(OUT_DIR)/libmidcoap.a

# library generation rule
$(MID_LIB_COAP): $(MID_COAP_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_COAP_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_COAP_INCDIR)
MID_CSRCDIR += $(MID_COAP_CSRCDIR)
MID_ASMSRCDIR += $(MID_COAP_ASMSRCDIR)

MID_CSRCS += $(MID_COAP_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_COAP_ASMSRCS)
MID_ALLSRCS += $(MID_COAP_CSRCS) $(MID_COAP_ASMSRCS)

MID_COBJS += $(MID_COAP_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_COAP_ASMOBJS)
MID_ALLOBJS += $(MID_COAP_OBJS)

MID_DEFINES += $(MID_COAP_DEFINES)
MID_DEPS += $(MID_COAP_DEPS)
MID_LIBS += $(MID_LIB_COAP)