# dir declaration
MID_MBEDTLS_DIR = $(MIDDLEWARES_ROOT)/mbedtls

MID_MBEDTLS_CSRCDIR	= $(MID_MBEDTLS_DIR)/library $(MID_MBEDTLS_DIR)/embARC
MID_MBEDTLS_INCDIR	= $(MID_MBEDTLS_DIR)/include $(MID_MBEDTLS_DIR)/embARC

# find all the source files in the target directories
MID_MBEDTLS_CSRCS = $(call get_csrcs, $(MID_MBEDTLS_CSRCDIR))
MID_MBEDTLS_ASMSRCS = $(call get_asmsrcs, $(MID_MBEDTLS_ASMSRCDIR))

# get object files
MID_MBEDTLS_COBJS = $(call get_relobjs, $(MID_MBEDTLS_CSRCS))
MID_MBEDTLS_ASMOBJS = $(call get_relobjs, $(MID_MBEDTLS_ASMSRCS))
MID_MBEDTLS_OBJS = $(MID_MBEDTLS_COBJS) $(MID_MBEDTLS_ASMOBJS)

# get dependency files
MID_MBEDTLS_DEPS = $(call get_deps, $(MID_MBEDTLS_OBJS))

# extra macros to be defined
MID_MBEDTLS_DEFINES = -DMID_MBEDTLS

# genearte library
MID_LIB_MBEDTLS = $(OUT_DIR)/libmidmbedtls.a

# library generation rule
$(MID_LIB_MBEDTLS): $(MID_MBEDTLS_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_MBEDTLS_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_MBEDTLS_INCDIR)
MID_CSRCDIR += $(MID_MBEDTLS_CSRCDIR)
MID_ASMSRCDIR += $(MID_MBEDTLS_ASMSRCDIR)

MID_CSRCS += $(MID_MBEDTLS_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_MBEDTLS_ASMSRCS)
MID_ALLSRCS += $(MID_MBEDTLS_CSRCS) $(MID_MBEDTLS_ASMSRCS)

MID_COBJS += $(MID_MBEDTLS_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_MBEDTLS_ASMOBJS)
MID_ALLOBJS += $(MID_MBEDTLS_OBJS)

MID_DEFINES += $(MID_MBEDTLS_DEFINES)
MID_DEPS += $(MID_MBEDTLS_DEPS)
MID_LIBS += $(MID_LIB_MBEDTLS)