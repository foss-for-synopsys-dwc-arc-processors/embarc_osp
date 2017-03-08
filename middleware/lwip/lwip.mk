# dir declaration
MID_LWIP_DIR = $(MIDDLEWARES_ROOT)/lwip

MID_LWIP_ASMSRCDIR	=
MID_LWIP_CSRCDIR	= $(MID_LWIP_DIR)/src/core \
				$(MID_LWIP_DIR)/src/core/ipv4 \
				$(MID_LWIP_DIR)/src/core/ipv6 \
				$(MID_LWIP_DIR)/src/netif \
				$(MID_LWIP_DIR)/src/netif/ppp \
				$(MID_LWIP_DIR)/src/netif/polarssl \
				$(MID_LWIP_DIR)/src/api

MID_LWIP_INCDIR		= $(MID_LWIP_DIR)/src/include \
				$(MID_LWIP_DIR)/src/include/posix

# find all the source files in the target directories
MID_LWIP_CSRCS = $(call get_csrcs, $(MID_LWIP_CSRCDIR))
MID_LWIP_ASMSRCS = $(call get_asmsrcs, $(MID_LWIP_ASMSRCDIR))

# get object files
MID_LWIP_COBJS = $(call get_relobjs, $(MID_LWIP_CSRCS))
MID_LWIP_ASMOBJS = $(call get_relobjs, $(MID_LWIP_ASMSRCS))
MID_LWIP_OBJS = $(MID_LWIP_COBJS) $(MID_LWIP_ASMOBJS)

# get dependency files
MID_LWIP_DEPS = $(call get_deps, $(MID_LWIP_OBJS))

# extra macros to be defined
MID_LWIP_DEFINES = -DMID_LWIP

# genearte library
MID_LIB_LWIP = $(OUT_DIR)/libmidlwip.a

# library generation rule
$(MID_LIB_LWIP): $(MID_LWIP_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_LWIP_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_LWIP_INCDIR)
MID_CSRCDIR += $(MID_LWIP_CSRCDIR)
MID_ASMSRCDIR += $(MID_LWIP_ASMSRCDIR)

MID_CSRCS += $(MID_LWIP_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_LWIP_ASMSRCS)
MID_ALLSRCS += $(MID_LWIP_CSRCS) $(MID_LWIP_ASMSRCS)

MID_COBJS += $(MID_LWIP_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_LWIP_ASMOBJS)
MID_ALLOBJS += $(MID_LWIP_OBJS)

MID_DEFINES += $(MID_LWIP_DEFINES)
MID_DEPS += $(MID_LWIP_DEPS)
MID_LIBS += $(MID_LIB_LWIP)