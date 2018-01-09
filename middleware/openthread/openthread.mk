# dir declaration
MID_OPENTHREAD_DIR = $(MIDDLEWARES_ROOT)/openthread

MID_OPENTHREAD_ASMSRCDIR	=
MID_OPENTHREAD_CSRCDIR		= $(MID_OPENTHREAD_DIR)/mbedtls \
				$(MID_OPENTHREAD_DIR)/platforms/arc \
				$(MID_OPENTHREAD_DIR)/src/core/utils \
				$(MID_OPENTHREAD_DIR)/src/core/ncp \
				$(MID_OPENTHREAD_DIR)/src/ncp

MID_OPENTHREAD_CXXSRCDIR	= $(MID_OPENTHREAD_DIR)/platforms/utils \
				$(MID_OPENTHREAD_DIR)/src/core/api \
				$(MID_OPENTHREAD_DIR)/src/core/coap \
				$(MID_OPENTHREAD_DIR)/src/core/common \
				$(MID_OPENTHREAD_DIR)/src/core/crypto \
				$(MID_OPENTHREAD_DIR)/src/core/mac \
				$(MID_OPENTHREAD_DIR)/src/core/meshcop \
				$(MID_OPENTHREAD_DIR)/src/core/net \
				$(MID_OPENTHREAD_DIR)/src/core/thread \
				$(MID_OPENTHREAD_DIR)/src/core/utils \
				$(MID_OPENTHREAD_DIR)/src/diag \
				$(MID_OPENTHREAD_DIR)/src/ncp \
				$(MID_OPENTHREAD_DIR)/src/cli

MID_OPENTHREAD_INCDIR		= $(MID_OPENTHREAD_DIR)/include \
				$(MID_OPENTHREAD_DIR)/include/openthread \
				$(MID_OPENTHREAD_DIR)/include/openthread/platform \
				$(MID_OPENTHREAD_DIR)/platforms \
				$(MID_OPENTHREAD_DIR)/platforms/arc \
				$(MID_OPENTHREAD_DIR)/platforms/utils \
				$(MID_OPENTHREAD_DIR)/src \
				$(MID_OPENTHREAD_DIR)/src/cli \
				$(MID_OPENTHREAD_DIR)/src/core \
				$(MID_OPENTHREAD_DIR)/src/core/api \
				$(MID_OPENTHREAD_DIR)/src/core/coap \
				$(MID_OPENTHREAD_DIR)/src/core/common \
				$(MID_OPENTHREAD_DIR)/src/core/crypto \
				$(MID_OPENTHREAD_DIR)/src/core/mac \
				$(MID_OPENTHREAD_DIR)/src/core/meshcop \
				$(MID_OPENTHREAD_DIR)/src/core/net \
				$(MID_OPENTHREAD_DIR)/src/core/thread \
				$(MID_OPENTHREAD_DIR)/src/core/utils

# find all the srcs in the target dirs
MID_OPENTHREAD_CXXSRCS = $(call get_cxxsrcs, $(MID_OPENTHREAD_CXXSRCDIR))
MID_OPENTHREAD_CSRCS = $(call get_csrcs, $(MID_OPENTHREAD_CSRCDIR))
MID_OPENTHREAD_ASMSRCS = $(call get_asmsrcs, $(MID_OPENTHREAD_ASMSRCDIR))

# get obj files
MID_OPENTHREAD_CXXOBJS = $(call get_relobjs, $(MID_OPENTHREAD_CXXSRCS))
MID_OPENTHREAD_COBJS = $(call get_relobjs, $(MID_OPENTHREAD_CSRCS))
MID_OPENTHREAD_ASMOBJS = $(call get_relobjs, $(MID_OPENTHREAD_ASMSRCS))
MID_OPENTHREAD_OBJS = $(MID_OPENTHREAD_CXXOBJS) $(MID_OPENTHREAD_COBJS) $(MID_OPENTHREAD_ASMOBJS)

# get dependency files
MID_OPENTHREAD_DEPS = $(call get_deps, $(MID_OPENTHREAD_OBJS))

# extra macros to be defined
MID_OPENTHREAD_DEFINES = -DMID_OPENTHREAD

# genearte library
MID_LIB_OPENTHREAD = $(OUT_DIR)/libmidopenthread.a

$(MID_LIB_OPENTHREAD): $(MID_OPENTHREAD_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_OPENTHREAD_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_OPENTHREAD_INCDIR)
MID_CSRCDIR += $(MID_OPENTHREAD_CSRCDIR)
MID_CXXSRCDIR += $(MID_OPENTHREAD_CXXSRCDIR)
MID_ASMSRCDIR += $(MID_OPENTHREAD_ASMSRCDIR)

MID_CSRCS += $(MID_OPENTHREAD_CSRCS)
MID_CXXSRCS += $(MID_OPENTHREAD_CXXSRCS)
MID_ASMSRCS += $(MID_OPENTHREAD_ASMSRCS)
MID_ALLSRCS += $(MID_OPENTHREAD_CSRCS) $(MID_OPENTHREAD_CXXSRCS) $(MID_OPENTHREAD_ASMSRCS)

MID_COBJS += $(MID_OPENTHREAD_COBJS)
MID_CXXOBJS += $(MID_OPENTHREAD_CXXOBJS)
MID_ASMOBJS += $(MID_OPENTHREAD_ASMOBJS)
MID_ALLOBJS += $(MID_OPENTHREAD_OBJS)

MID_DEFINES += $(MID_OPENTHREAD_DEFINES)
MID_DEPS += $(MID_OPENTHREAD_DEPS)
MID_LIBS += $(MID_LIB_OPENTHREAD)