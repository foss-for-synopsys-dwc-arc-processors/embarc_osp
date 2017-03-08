# dir declaration
MID_AWS_DIR = $(MIDDLEWARES_ROOT)/aws

MID_AWS_CSRCDIR	= $(MID_AWS_DIR)/src \
		  $(MID_AWS_DIR)/external_libs/jsmn \
		  $(MID_AWS_DIR)/platform/embARC/common \
		  $(MID_AWS_DIR)/platform/embARC/mbedtls \
		  $(MID_AWS_DIR)/platform/embARC/freertos

MID_AWS_INCDIR	= $(MID_AWS_DIR)/include \
		  $(MID_AWS_DIR)/external_libs/jsmn \
		  $(MID_AWS_DIR)/platform/embARC/common \
		  $(MID_AWS_DIR)/platform/embARC/mbedtls \
		  $(MID_AWS_DIR)/platform/embARC/freertos

# find all the source files in the target directories
MID_AWS_CSRCS = $(call get_csrcs, $(MID_AWS_CSRCDIR))
MID_AWS_ASMSRCS = $(call get_asmsrcs, $(MID_AWS_ASMSRCDIR))

# get object files
MID_AWS_COBJS = $(call get_relobjs, $(MID_AWS_CSRCS))
MID_AWS_ASMOBJS = $(call get_relobjs, $(MID_AWS_ASMSRCS))
MID_AWS_OBJS = $(MID_AWS_COBJS) $(MID_AWS_ASMOBJS)

# get dependency files
MID_AWS_DEPS = $(call get_deps, $(MID_AWS_OBJS))

# extra macros to be defined
MID_AWS_DEFINES = -DMID_AWS

# genearte library
MID_LIB_AWS = $(OUT_DIR)/libmidaws.a

# library generation rule
$(MID_LIB_AWS): $(MID_AWS_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_AWS_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_AWS_INCDIR)
MID_CSRCDIR += $(MID_AWS_CSRCDIR)
MID_ASMSRCDIR += $(MID_AWS_ASMSRCDIR)

MID_CSRCS += $(MID_AWS_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_AWS_ASMSRCS)
MID_ALLSRCS += $(MID_AWS_CSRCS) $(MID_AWS_ASMSRCS)

MID_COBJS += $(MID_AWS_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_AWS_ASMOBJS)
MID_ALLOBJS += $(MID_AWS_OBJS)

MID_DEFINES += $(MID_AWS_DEFINES)
MID_DEPS += $(MID_AWS_DEPS)
MID_LIBS += $(MID_LIB_AWS)