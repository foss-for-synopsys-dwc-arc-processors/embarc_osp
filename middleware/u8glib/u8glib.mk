# dir declaration
MID_U8GLIB_DIR = $(MIDDLEWARES_ROOT)/u8glib

MID_U8GLIB_ASMSRCDIR	=
MID_U8GLIB_CSRCDIR	= $(MID_U8GLIB_DIR)/fntsrc $(MID_U8GLIB_DIR)/csrc
MID_U8GLIB_INCDIR	= $(MID_U8GLIB_DIR)/csrc

# find all the source files in the target directories
MID_U8GLIB_CSRCS = $(call get_csrcs, $(MID_U8GLIB_CSRCDIR))
MID_U8GLIB_ASMSRCS = $(call get_asmsrcs, $(MID_U8GLIB_ASMSRCDIR))

# get object files
MID_U8GLIB_COBJS = $(call get_relobjs, $(MID_U8GLIB_CSRCS))
MID_U8GLIB_ASMOBJS = $(call get_relobjs, $(MID_U8GLIB_ASMSRCS))
MID_U8GLIB_OBJS = $(MID_U8GLIB_COBJS) $(MID_U8GLIB_ASMOBJS)

# get dependency files
MID_U8GLIB_DEPS = $(call get_deps, $(MID_U8GLIB_OBJS))

# Divide all object files into 10 part
MID_U8GLIB_OBJ_CNT = $(words $(MID_U8GLIB_OBJS))
MID_U8GLIB_OBJS_PART1  = $(wordlist   1,  50, $(MID_U8GLIB_OBJS))
MID_U8GLIB_OBJS_PART2  = $(wordlist  51, 100, $(MID_U8GLIB_OBJS))
MID_U8GLIB_OBJS_PART3  = $(wordlist 101, 150, $(MID_U8GLIB_OBJS))
MID_U8GLIB_OBJS_PART4  = $(wordlist 151, 200, $(MID_U8GLIB_OBJS))
MID_U8GLIB_OBJS_PART5  = $(wordlist 201, 250, $(MID_U8GLIB_OBJS))
MID_U8GLIB_OBJS_PART6  = $(wordlist 251, 300, $(MID_U8GLIB_OBJS))
MID_U8GLIB_OBJS_PART7  = $(wordlist 301, 350, $(MID_U8GLIB_OBJS))
MID_U8GLIB_OBJS_PART8  = $(wordlist 351, 400, $(MID_U8GLIB_OBJS))
MID_U8GLIB_OBJS_PART9  = $(wordlist 401, 450, $(MID_U8GLIB_OBJS))
MID_U8GLIB_OBJS_PART10 = $(wordlist 451, $(MID_U8GLIB_OBJ_CNT), $(MID_U8GLIB_OBJS))

# extra macros to be defined
MID_U8GLIB_DEFINES = -DMID_U8GLIB

# genearte library
MID_LIB_U8GLIB = $(OUT_DIR)/libmidu8glib.a

## A strange way to avoid limit argument length of windows CreateProcess function
## http://msdn.microsoft.com/en-us/library/windows/desktop/ms682425
# library generation rule
$(MID_LIB_U8GLIB): $(MID_U8GLIB_OBJS)
	$(TRACE_ARCHIVE)
ifneq ($(strip $(MID_U8GLIB_OBJS_PART10)),)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_U8GLIB_OBJS_PART10)
endif
ifneq ($(strip $(MID_U8GLIB_OBJS_PART9)),)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_U8GLIB_OBJS_PART9)
endif
ifneq ($(strip $(MID_U8GLIB_OBJS_PART8)),)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_U8GLIB_OBJS_PART8)
endif
ifneq ($(strip $(MID_U8GLIB_OBJS_PART7)),)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_U8GLIB_OBJS_PART7)
endif
ifneq ($(strip $(MID_U8GLIB_OBJS_PART6)),)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_U8GLIB_OBJS_PART6)
endif
ifneq ($(strip $(MID_U8GLIB_OBJS_PART5)),)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_U8GLIB_OBJS_PART5)
endif
ifneq ($(strip $(MID_U8GLIB_OBJS_PART4)),)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_U8GLIB_OBJS_PART4)
endif
ifneq ($(strip $(MID_U8GLIB_OBJS_PART3)),)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_U8GLIB_OBJS_PART3)
endif
ifneq ($(strip $(MID_U8GLIB_OBJS_PART2)),)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_U8GLIB_OBJS_PART2)
endif
ifneq ($(strip $(MID_U8GLIB_OBJS_PART1)),)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_U8GLIB_OBJS_PART1)
endif

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_U8GLIB_INCDIR)
MID_CSRCDIR += $(MID_U8GLIB_CSRCDIR)
MID_ASMSRCDIR += $(MID_U8GLIB_ASMSRCDIR)

MID_CSRCS += $(MID_U8GLIB_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_U8GLIB_ASMSRCS)
MID_ALLSRCS += $(MID_U8GLIB_CSRCS) $(MID_U8GLIB_ASMSRCS)

MID_COBJS += $(MID_U8GLIB_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_U8GLIB_ASMOBJS)
MID_ALLOBJS += $(MID_U8GLIB_OBJS)

MID_DEFINES += $(MID_U8GLIB_DEFINES)
MID_DEPS += $(MID_U8GLIB_DEPS)
MID_LIBS += $(MID_LIB_U8GLIB)