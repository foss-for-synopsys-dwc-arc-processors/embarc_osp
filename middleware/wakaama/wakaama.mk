# dir declaration
MID_WAKAAMA_DIR = $(MIDDLEWARES_ROOT)/wakaama

MID_WAKAAMA_ASMSRCDIR	=
MID_WAKAAMA_CSRCDIR	= $(MID_WAKAAMA_DIR)/core \
				$(MID_WAKAAMA_DIR)/core/er-coap-13 \
				$(MID_WAKAAMA_DIR)/port \
				$(MID_WAKAAMA_DIR)/port/conn/udp

MID_WAKAAMA_INCDIR	= $(MID_WAKAAMA_DIR)/core \
				$(MID_WAKAAMA_DIR)/core/er-coap-13 \
				$(MID_WAKAAMA_DIR)/port/conn/udp

# find all the source files in the target directories
MID_WAKAAMA_CSRCS = $(call get_csrcs, $(MID_WAKAAMA_CSRCDIR))
MID_WAKAAMA_ASMSRCS = $(call get_asmsrcs, $(MID_WAKAAMA_ASMSRCDIR))

# get object files
MID_WAKAAMA_COBJS = $(call get_relobjs, $(MID_WAKAAMA_CSRCS))
MID_WAKAAMA_ASMOBJS = $(call get_relobjs, $(MID_WAKAAMA_ASMSRCS))
MID_WAKAAMA_OBJS = $(MID_WAKAAMA_COBJS) $(MID_WAKAAMA_ASMOBJS)

# get dependency files
MID_WAKAAMA_DEPS = $(call get_deps, $(MID_WAKAAMA_OBJS))

# extra macros to be defined
MID_WAKAAMA_DEFINES = -DMID_WAKAAMA -DLWM2M_EMBEDDED_MODE -DLWM2M_LITTLE_ENDIAN

# genearte library
MID_LIB_WAKAAMA = $(OUT_DIR)/libmidwakaama.a

# library generation rule
$(MID_LIB_WAKAAMA): $(MID_WAKAAMA_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_WAKAAMA_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_WAKAAMA_INCDIR)
MID_CSRCDIR += $(MID_WAKAAMA_CSRCDIR)
MID_ASMSRCDIR += $(MID_WAKAAMA_ASMSRCDIR)

MID_CSRCS += $(MID_WAKAAMA_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_WAKAAMA_ASMSRCS)
MID_ALLSRCS += $(MID_WAKAAMA_CSRCS) $(MID_WAKAAMA_ASMSRCS)

MID_COBJS += $(MID_WAKAAMA_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_WAKAAMA_ASMOBJS)
MID_ALLOBJS += $(MID_WAKAAMA_OBJS)

MID_DEFINES += $(MID_WAKAAMA_DEFINES)
MID_DEPS += $(MID_WAKAAMA_DEPS)
MID_LIBS += $(MID_LIB_WAKAAMA)