# -*- makefile -*-

ifndef CONTIKI
  ${error CONTIKI not defined! You must specify where Contiki resides}
endif

TARGET = $(BOARD)

# Include IPv6, IPv4, and/or Rime
ifeq ($(CONTIKI_WITH_IPV4),1)
  CONTIKI_CFLAGS += -DNETSTACK_CONF_WITH_IPV4=1
  MODULES += core/net/ipv4 core/net/ip
endif

ifeq ($(CONTIKI_WITH_RIME),1)
  CONTIKI_CFLAGS += -DNETSTACK_CONF_WITH_RIME=1
  MODULES += core/net/rime
endif

ifeq ($(CONTIKI_WITH_IPV6),1)
  CONTIKI_CFLAGS += -DNETSTACK_CONF_WITH_IPV6=1
  MODULES += core/net/ipv6 core/net/ip

ifeq ($(CONTIKI_WITH_IPV6_IP64), 1)
  CONTIKI_CFLAGS += -DUIP_CONF_IPV6_IP64=1
  MODULES += core/net/ip64
endif

endif

ifeq ($(CONTIKI_WITH_RPL),1)
    CONTIKI_CFLAGS += -DUIP_CONF_IPV6_RPL=1
    MODULES += core/net/rpl
else
    CONTIKI_CFLAGS += -DUIP_CONF_IPV6_RPL=0
endif

TARGET_UPPERCASE := ${call uc, $(TARGET)}

CONTIKI_CFLAGS += -DCONTIKI=1 -DCONTIKI_TARGET_$(TARGET_UPPERCASE)=1

MODULES += core/sys core/lib core/dev

ifdef CONTIKIFILES
CONTIKI_SOURCEFILES += $(CONTIKIFILES)
endif

CONTIKIDIRS += ${addprefix $(CONTIKI)/core/,dev lib net net/mac net/rime \
                 net/rpl sys cfs ctk lib/ctk loader . }

oname = ${patsubst %.c,%.o,${patsubst %.S,%.o,$(1)}}


uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))

### Include application makefiles

ifdef APPS
  APPDS = ${wildcard ${addprefix $(CONTIKI)/apps/, $(APPS)} \
          ${addprefix $(CONTIKI)/platform/$(TARGET)/apps/, $(APPS)} \
          $(APPS)}
  APPINCLUDES = ${foreach APP, $(APPS), ${wildcard ${foreach DIR, $(APPDS), $(DIR)/Makefile.$(APP)}}}
  -include $(APPINCLUDES)
  APP_SOURCES = ${foreach APP, $(APPS), $($(APP)_src)}
  DSC_SOURCES = ${foreach APP, $(APPS), $($(APP)_dsc)}
  CONTIKI_SOURCEFILES += $(APP_SOURCES) $(DSC_SOURCES)
endif

### Include target makefile (TODO Unsafe?)

target_makefile := $(wildcard $(CONTIKI)/platform/$(TARGET)/Makefile.$(TARGET))

# Check if the target makefile exists, and create the object directory if necessary.
ifeq ($(strip $(target_makefile)),)
  ${error The target platform "$(TARGET)" does not exist (maybe it was misspelled?)}
else
  ifneq (1, ${words $(target_makefile)})
    ${error More than one TARGET Makefile found: $(target_makefile)}
  endif
  include $(target_makefile)
endif

ifdef MODULES
  UNIQUEMODULES = $(call uniq,$(MODULES))
  MODULESSUBST = ${subst /,-,$(UNIQUEMODULES)}
  MODULEDIRS = ${wildcard ${addprefix $(CONTIKI)/, $(UNIQUEMODULES)}}
  MODULES_SOURCES = ${foreach d, $(MODULEDIRS), ${subst ${d}/,,${wildcard $(d)/*.c}}}
  CONTIKI_SOURCEFILES += $(MODULES_SOURCES)
  APPDS += $(MODULEDIRS)
endif

### Forward comma-separated list of arbitrary defines to the compiler

### Setup directory search path for source and header files

CONTIKI_TARGET_DIRS_CONCAT = ${addprefix ${dir $(target_makefile)}, \
				$(CONTIKI_TARGET_DIRS)}

CONTIKI_CFLAGS += -DAUTOSTART_ENABLE

OS_CONTIKI_CSRCDIR	= $(CONTIKI_TARGET_DIRS_CONCAT) \
				$(APPDS) $(CONTIKI)/core
OS_CONTIKI_INCDIR	= $(OS_CONTIKI_CSRCDIR)
OS_CONTIKI_ASMSRCDIR	= $(OS_CONTIKI_CSRCDIR)

OS_CONTIKI_OBJDIR	= $(OUT_DIR)/os/contiki

OS_CONTIKI_CSRCS = $(sort $(filter %.c, $(CONTIKI_SOURCEFILES)))
OS_CONTIKI_ASMSRCS = $(sort $(filter %.S %.s, $(CONTIKI_SOURCEFILES)))

OS_CONTIKI_COBJS = $(call get_objs, $(OS_CONTIKI_CSRCS), $(OS_CONTIKI_OBJDIR))
OS_CONTIKI_ASMOBJS = $(call get_objs, $(OS_CONTIKI_ASMSRCS), $(OS_CONTIKI_OBJDIR))
OS_CONTIKI_OBJS = $(OS_CONTIKI_COBJS) $(OS_CONTIKI_ASMOBJS)

OS_CONTIKI_DEFINES += $(CONTIKI_CFLAGS)

# genearte library
OS_LIB_CONTIKI = $(OUT_DIR)/libcontiki.a

$(OS_LIB_CONTIKI): $(OS_CONTIKI_OBJDIR) $(OS_CONTIKI_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(OS_CONTIKI_OBJS)

.SECONDEXPANSION:
$(OS_CONTIKI_COBJS): $(OS_CONTIKI_OBJDIR)/%.o :$(call get_c_prerequisite, $(OS_CONTIKI_CSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(CONTIKI_COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(OS_CONTIKI_ASMOBJS): $(OS_CONTIKI_OBJDIR)/%.o :$(call get_asm_prerequisite, $(OS_CONTIKI_ASMSRCDIR)) $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(CONTIKI_ASM_OPT) $< -o $@

$(OS_CONTIKI_OBJDIR):
	$(TRACE_CREATE_DIR)
	$(TRY_MKDIR)


# OS Definitions
OS_INCDIR += $(OS_CONTIKI_INCDIR)
OS_CSRCDIR += $(OS_CONTIKI_CSRCDIR)
OS_ASMSRCDIR += $(OS_CONTIKI_ASMSRCDIR)

OS_CSRCS += $(OS_CONTIKI_CSRCS)
OS_CXXSRCS +=
OS_ASMSRCS += $(OS_CONTIKI_ASMSRCS)
OS_ALLSRCS += $(OS_CONTIKI_CSRCS) $(OS_CONTIKI_ASMSRCS)

OS_COBJS += $(OS_CONTIKI_COBJS)
OS_CXXOBJS +=
OS_ASMOBJS += $(OS_CONTIKI_ASMOBJS)
OS_ALLOBJS += $(OS_CONTIKI_OBJS)

OS_DEFINES += $(OS_CONTIKI_DEFINES)
OS_DEPS += $(OS_CONTIKI_DEPS)
OS_LIBS += $(OS_LIB_CONTIKI)
