# dir declaration
MID_NTSHELL_DIR = $(MIDDLEWARES_ROOT)/ntshell

MID_NTSHELL_ASMSRCDIR	= $(MID_NTSHELL_DIR)/src \
				$(MID_NTSHELL_DIR)/cmds \
				$(MID_NTSHELL_DIR)/cmds/cmds_arc \
				$(MID_NTSHELL_DIR)/cmds/cmds_fs \
				$(MID_NTSHELL_DIR)/cmds/cmds_peripheral \
				$(MID_NTSHELL_DIR)/cmds/cmds_extend \
				$(MID_NTSHELL_DIR)/cmds/cmds_fs/ymodem \
				$(MID_NTSHELL_DIR)/port
MID_NTSHELL_CSRCDIR	= $(MID_NTSHELL_DIR)/src \
				$(MID_NTSHELL_DIR)/cmds \
				$(MID_NTSHELL_DIR)/cmds/cmds_arc \
				$(MID_NTSHELL_DIR)/cmds/cmds_fs \
				$(MID_NTSHELL_DIR)/cmds/cmds_peripheral \
				$(MID_NTSHELL_DIR)/cmds/cmds_extend \
				$(MID_NTSHELL_DIR)/cmds/cmds_fs/ymodem \
				$(MID_NTSHELL_DIR)/port
MID_NTSHELL_INCDIR	= $(MID_NTSHELL_DIR)/src \
				$(MID_NTSHELL_DIR)/cmds \
				$(MID_NTSHELL_DIR)/cmds/cmds_arc \
				$(MID_NTSHELL_DIR)/cmds/cmds_fs \
				$(MID_NTSHELL_DIR)/cmds/cmds_peripheral \
				$(MID_NTSHELL_DIR)/cmds/cmds_extend \
				$(MID_NTSHELL_DIR)/cmds/cmds_fs/ymodem \
				$(MID_NTSHELL_DIR)/port

# find all the source files in the target directories
MID_NTSHELL_CSRCS = $(call get_csrcs, $(MID_NTSHELL_CSRCDIR))
MID_NTSHELL_ASMSRCS = $(call get_asmsrcs, $(MID_NTSHELL_ASMSRCDIR))

# get object files
MID_NTSHELL_COBJS = $(call get_relobjs, $(MID_NTSHELL_CSRCS))
MID_NTSHELL_ASMOBJS = $(call get_relobjs, $(MID_NTSHELL_ASMSRCS))
MID_NTSHELL_OBJS = $(MID_NTSHELL_COBJS) $(MID_NTSHELL_ASMOBJS)

# get dependency files
MID_NTSHELL_DEPS = $(call get_deps, $(MID_NTSHELL_OBJS))

# extra macros to be defined
MID_NTSHELL_DEFINES = -DMID_NTSHELL

# genearte library
MID_LIB_NTSHELL = $(OUT_DIR)/libmidntshell.a

# library generation rule
$(MID_LIB_NTSHELL): $(MID_NTSHELL_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_NTSHELL_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_NTSHELL_INCDIR)
MID_CSRCDIR += $(MID_NTSHELL_CSRCDIR)
MID_ASMSRCDIR += $(MID_NTSHELL_ASMSRCDIR)

MID_CSRCS += $(MID_NTSHELL_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_NTSHELL_ASMSRCS)
MID_ALLSRCS += $(MID_NTSHELL_CSRCS) $(MID_NTSHELL_ASMSRCS)

MID_COBJS += $(MID_NTSHELL_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_NTSHELL_ASMOBJS)
MID_ALLOBJS += $(MID_NTSHELL_OBJS)

MID_DEFINES += $(MID_NTSHELL_DEFINES)
MID_DEPS += $(MID_NTSHELL_DEPS)
MID_LIBS += $(MID_LIB_NTSHELL)