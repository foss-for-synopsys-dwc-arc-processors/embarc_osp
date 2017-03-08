# dir declaration
MID_XIVELY_DIR = $(MIDDLEWARES_ROOT)/xively

XI_BUILD_TYPE ?= release
#XI_BUILD_TYPE ?= debug
XI_DEBUG_ASSERT ?= 1
XI_IO_LAYER ?= posix
XI_NOB_ENABLED ?= false
XI_IO_LAYER_POSIX_COMPAT ?= 1
XI_DEBUG_PRINTF ?= xprintf
XI_ALLOCATOR_USE ?= lwip
XI_USER_AGENT ?= \"libxively-$(XI_IO_LAYER)\"

## include xively makefile config
include $(MID_XIVELY_DIR)/Makefile.include

# Middleware Definitions
ifndef XI_USER_CONFIG

ifeq ($(XI_IO_LAYER),posix)
  XI_DEFINES += -DXI_IO_LAYER=0
endif

ifeq ($(XI_IO_LAYER),contiki)
  XI_DEFINES += -DXI_IO_LAYER=4
endif

  XI_DEFINES += $(foreach constant,$(XI_CONFIG),-D$(constant))
  XI_DEFINES += -DXI_USER_AGENT=$(XI_USER_AGENT)
  XI_DEFINES += -DXI_IO_LAYER_POSIX_COMPAT=$(XI_IO_LAYER_POSIX_COMPAT)
ifeq ($(XI_ALLOCATOR_USE), lwip)
  XI_DEFINES += -DXI_ALLOCATOR=2
endif
else
  $(warning "Using XI_USER_CONFIG instead of: '$(XI_CONFIG)'")
  XI_DEFINES += -DXI_USER_CONFIG=1
  ifdef XI_USER_CONFIG_PATH
    $(info "Will include '$(XI_USER_CONFIG_PATH)/xi_user_config.h'")
    XI_DEFINES += -I$(XI_USER_CONFIG_PATH)
  endif
endif

MID_XIVELY_ASMSRCDIR	=
MID_XIVELY_CSRCDIR	= $(MID_XIVELY_DIR)/src/libxively \
				$(MID_XIVELY_DIR)/src/libxively/io/$(XI_IO_LAYER)
MID_XIVELY_INCDIR	= $(MID_XIVELY_DIR)/src/libxively \
				$(MID_XIVELY_DIR)/src/libxively/io/$(XI_IO_LAYER)

ifeq ($(XI_NOB_ENABLED),true)
MID_XIVELY_CSRCDIR	+= $(MID_XIVELY_DIR)/src/libxively/nob
MID_XIVELY_INCDIR	+= $(MID_XIVELY_DIR)/src/libxively/nob
endif

# find all the source files in the target directories
MID_XIVELY_CSRCS = $(call get_csrcs, $(MID_XIVELY_CSRCDIR))
MID_XIVELY_ASMSRCS = $(call get_asmsrcs, $(MID_XIVELY_ASMSRCDIR))

# get object files
MID_XIVELY_COBJS = $(call get_relobjs, $(MID_XIVELY_CSRCS))
MID_XIVELY_ASMOBJS = $(call get_relobjs, $(MID_XIVELY_ASMSRCS))
MID_XIVELY_OBJS = $(MID_XIVELY_COBJS) $(MID_XIVELY_ASMOBJS)

# get dependency files
MID_XIVELY_DEPS = $(call get_deps, $(MID_XIVELY_OBJS))

# extra macros to be defined
MID_XIVELY_DEFINES = -DMID_XIVELY $(XI_DEFINES)

# genearte library
MID_LIB_XIVELY = $(OUT_DIR)/libmidxively.a

# library generation rule
$(MID_LIB_XIVELY): $(MID_XIVELY_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_XIVELY_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_XIVELY_INCDIR)
MID_CSRCDIR += $(MID_XIVELY_CSRCDIR)
MID_ASMSRCDIR += $(MID_XIVELY_ASMSRCDIR)

MID_CSRCS += $(MID_XIVELY_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_XIVELY_ASMSRCS)
MID_ALLSRCS += $(MID_XIVELY_CSRCS) $(MID_XIVELY_ASMSRCS)

MID_COBJS += $(MID_XIVELY_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_XIVELY_ASMOBJS)
MID_ALLOBJS += $(MID_XIVELY_OBJS)

MID_DEFINES += $(MID_XIVELY_DEFINES)
MID_DEPS += $(MID_XIVELY_DEPS)
MID_LIBS += $(MID_LIB_XIVELY)