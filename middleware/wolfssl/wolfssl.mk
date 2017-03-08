# dir declaration
MID_WOLFSSL_DIR = $(MIDDLEWARES_ROOT)/wolfssl

MID_WOLFSSL_ASMSRCDIR	=
MID_WOLFSSL_CSRCDIR	= $(MID_WOLFSSL_DIR)/src $(MID_WOLFSSL_DIR)/wolfcrypt/src
MID_WOLFSSL_INCDIR	= $(MID_WOLFSSL_DIR)

# find all the source files in the target directories
MID_WOLFSSL_CSRCS = $(call get_csrcs, $(MID_WOLFSSL_CSRCDIR))
MID_WOLFSSL_ASMSRCS = $(call get_asmsrcs, $(MID_WOLFSSL_ASMSRCDIR))

# get object files
MID_WOLFSSL_COBJS = $(call get_relobjs, $(MID_WOLFSSL_CSRCS))
MID_WOLFSSL_ASMOBJS = $(call get_relobjs, $(MID_WOLFSSL_ASMSRCS))
MID_WOLFSSL_OBJS = $(MID_WOLFSSL_COBJS) $(MID_WOLFSSL_ASMOBJS)

# get dependency files
MID_WOLFSSL_DEPS = $(call get_deps, $(MID_WOLFSSL_OBJS))

# extra macros to be defined
MID_WOLFSSL_DEFINES = -DMID_WOLFSSL

# genearte library
MID_LIB_WOLFSSL = $(OUT_DIR)/libmidwolfssl.a

# library generation rule
$(MID_LIB_WOLFSSL): $(MID_WOLFSSL_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_WOLFSSL_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_WOLFSSL_INCDIR)
MID_CSRCDIR += $(MID_WOLFSSL_CSRCDIR)
MID_ASMSRCDIR += $(MID_WOLFSSL_ASMSRCDIR)

MID_CSRCS += $(MID_WOLFSSL_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_WOLFSSL_ASMSRCS)
MID_ALLSRCS += $(MID_WOLFSSL_CSRCS) $(MID_WOLFSSL_ASMSRCS)

MID_COBJS += $(MID_WOLFSSL_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_WOLFSSL_ASMOBJS)
MID_ALLOBJS += $(MID_WOLFSSL_OBJS)

MID_DEFINES += $(MID_WOLFSSL_DEFINES)
MID_DEPS += $(MID_WOLFSSL_DEPS)
MID_LIBS += $(MID_LIB_WOLFSSL)