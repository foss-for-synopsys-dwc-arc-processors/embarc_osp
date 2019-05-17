# dir declaration
MID_CODEC_DIR = $(MIDDLEWARES_ROOT)/codec

MID_CODEC_ASMSRCDIR	=
MID_CODEC_CSRCDIR	= $(MID_CODEC_DIR)
MID_CODEC_INCDIR	= $(MID_CODEC_DIR)

# find all the source files in the target directories
MID_CODEC_CSRCS 	= $(call get_csrcs, $(MID_CODEC_CSRCDIR))
MID_CODEC_ASMSRCS 	= $(call get_asmsrcs, $(MID_CODEC_ASMSRCDIR))

# get object files
MID_CODEC_COBJS 	= $(call get_relobjs, $(MID_CODEC_CSRCS))
MID_CODEC_ASMOBJS 	= $(call get_relobjs, $(MID_CODEC_ASMSRCS))
MID_CODEC_OBJS 		= $(MID_CODEC_COBJS) $(MID_CODEC_ASMOBJS)

# get dependency files
MID_CODEC_DEPS = $(call get_deps, $(MID_CODEC_OBJS))

# extra macros to be defined
MID_CODEC_DEFINES = -DMID_CODEC

# genearte library
MID_LIB_CODEC = $(OUT_DIR)/libmidcodec.a

# library generation rule
$(MID_LIB_CODEC): $(MID_CODEC_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_CODEC_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR 		+= $(MID_CODEC_INCDIR)
MID_CSRCDIR 	+= $(MID_CODEC_CSRCDIR)
MID_ASMSRCDIR 	+= $(MID_CODEC_ASMSRCDIR)

MID_CSRCS 	+= $(MID_CODEC_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_CODEC_ASMSRCS)
MID_ALLSRCS += $(MID_CODEC_CSRCS) $(MID_CODEC_ASMSRCS)

MID_COBJS 	+= $(MID_CODEC_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_CODEC_ASMOBJS)
MID_ALLOBJS += $(MID_CODEC_OBJS)

MID_DEFINES += $(MID_CODEC_DEFINES)
MID_DEPS 	+= $(MID_CODEC_DEPS)
MID_LIBS 	+= $(MID_LIB_CODEC)