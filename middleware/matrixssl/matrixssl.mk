# dir declaration
MID_MATRIXSSL_DIR = $(MIDDLEWARES_ROOT)/matrixssl

MID_MATRIXSSL_ASMSRCDIR	=
MID_MATRIXSSL_CSRCDIR	= $(MID_MATRIXSSL_DIR)/core/embARC \
				$(MID_MATRIXSSL_DIR)/core \
				$(MID_MATRIXSSL_DIR)/crypto/digest \
				$(MID_MATRIXSSL_DIR)/crypto/keyformat \
				$(MID_MATRIXSSL_DIR)/crypto/layer \
				$(MID_MATRIXSSL_DIR)/crypto/math \
				$(MID_MATRIXSSL_DIR)/crypto/pubkey \
				$(MID_MATRIXSSL_DIR)/crypto/symmetric \
				$(MID_MATRIXSSL_DIR)/crypto/prng \
				$(MID_MATRIXSSL_DIR)/matrixssl
MID_MATRIXSSL_INCDIR	= $(MID_MATRIXSSL_DIR)

# find all the source files in the target directories
MID_MATRIXSSL_CSRCS = $(call get_csrcs, $(MID_MATRIXSSL_CSRCDIR))
MID_MATRIXSSL_ASMSRCS = $(call get_asmsrcs, $(MID_MATRIXSSL_ASMSRCDIR))

# get object files
MID_MATRIXSSL_COBJS = $(call get_relobjs, $(MID_MATRIXSSL_CSRCS))
MID_MATRIXSSL_ASMOBJS = $(call get_relobjs, $(MID_MATRIXSSL_ASMSRCS))
MID_MATRIXSSL_OBJS = $(MID_MATRIXSSL_COBJS) $(MID_MATRIXSSL_ASMOBJS)

# get dependency files
MID_MATRIXSSL_DEPS = $(call get_deps, $(MID_MATRIXSSL_OBJS))

# extra macros to be defined
MID_MATRIXSSL_DEFINES = -DMID_MATRIXSSL -D__OPTIMIZE__ -D__OPTIMIZE_SIZE__

# genearte library
MID_LIB_MATRIXSSL = $(OUT_DIR)/libmidmatrixssl.a

# library generation rule
$(MID_LIB_MATRIXSSL): $(MID_MATRIXSSL_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_MATRIXSSL_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_MATRIXSSL_INCDIR)
MID_CSRCDIR += $(MID_MATRIXSSL_CSRCDIR)
MID_ASMSRCDIR += $(MID_MATRIXSSL_ASMSRCDIR)

MID_CSRCS += $(MID_MATRIXSSL_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_MATRIXSSL_ASMSRCS)
MID_ALLSRCS += $(MID_MATRIXSSL_CSRCS) $(MID_MATRIXSSL_ASMSRCS)

MID_COBJS += $(MID_MATRIXSSL_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_MATRIXSSL_ASMOBJS)
MID_ALLOBJS += $(MID_MATRIXSSL_OBJS)

MID_DEFINES += $(MID_MATRIXSSL_DEFINES)
MID_DEPS += $(MID_MATRIXSSL_DEPS)
MID_LIBS += $(MID_LIB_MATRIXSSL)