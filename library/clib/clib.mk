# dir declaration
LIB_CLIB_DIR = $(LIBRARIES_ROOT)/clib

LIB_CLIB_ASMSRCDIR	= $(LIB_CLIB_DIR)
LIB_CLIB_CSRCDIR	= $(LIB_CLIB_DIR)
LIB_CLIB_INCDIR		= $(LIB_CLIB_DIR)

# find all the source files in the target directories
LIB_CLIB_CSRCS = $(call get_csrcs, $(LIB_CLIB_CSRCDIR))
LIB_CLIB_ASMSRCS = $(call get_asmsrcs, $(LIB_CLIB_ASMSRCDIR))

# get object files
LIB_CLIB_COBJS = $(call get_relobjs, $(LIB_CLIB_CSRCS))
LIB_CLIB_ASMOBJS = $(call get_relobjs, $(LIB_CLIB_ASMSRCS))
LIB_CLIB_OBJS = $(LIB_CLIB_COBJS) $(LIB_CLIB_ASMOBJS)

# get dependency files
LIB_CLIB_DEPS = $(call get_deps, $(LIB_CLIB_OBJS))

# extra macros to be defined
LIB_CLIB_DEFINES = -DLIB_CLIB

# compile options only valid in this library
LIB_CLIB_COMPILE_OPT = -DHAVE_MMAP=0 -DHAVE_MREMAP=0 -DMMAP_CLEARS=0 -DLACKS_SYS_PARAM_H
LIB_CLIB_ASM_OPT = -DHAVE_MMAP=0 -DHAVE_MREMAP=0 -DMMAP_CLEARS=0 -DLACKS_SYS_PARAM_H

# genearte library
LIB_LIB_CLIB = $(OUT_DIR)/liblibclib.a

# library generation rule
$(LIB_LIB_CLIB): $(LIB_CLIB_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(LIB_CLIB_OBJS)

# specific compile rules
# user can add rules to compile this library
# if not rules specified to this library, it will use default compiling rules
.SECONDEXPANSION:
$(LIB_CLIB_COBJS): $(OUT_DIR)/%.o :$(EMBARC_ROOT)/%.c $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_COMPILE)
	$(Q)$(CC) -c $(COMPILE_OPT) $(LIB_CLIB_COMPILE_OPT) $< -o $@

.SECONDEXPANSION:
$(LIB_CLIB_ASMOBJS): $(OUT_DIR)/%.o :$(EMBARC_ROOT)/%.s $$(COMMON_COMPILE_PREREQUISITES)
	$(TRACE_ASSEMBLE)
	$(Q)$(CC) -c $(ASM_OPT) $(LIB_CLIB_ASM_OPT) $< -o $@

# Library Definitions
LIB_INCDIR += $(LIB_CLIB_INCDIR)
LIB_CSRCDIR += $(LIB_CLIB_CSRCDIR)
LIB_ASMSRCDIR += $(LIB_CLIB_ASMSRCDIR)

LIB_CSRCS += $(LIB_CLIB_CSRCS)
LIB_CXXSRCS +=
LIB_ASMSRCS += $(LIB_CLIB_ASMSRCS)
LIB_ALLSRCS += $(LIB_CLIB_CSRCS) $(LIB_CLIB_ASMSRCS)

LIB_COBJS += $(LIB_CLIB_COBJS)
LIB_CXXOBJS +=
LIB_ASMOBJS += $(LIB_CLIB_ASMOBJS)
LIB_ALLOBJS += $(LIB_CLIB_OBJS)

LIB_DEFINES += $(LIB_CLIB_DEFINES)
LIB_DEPS += $(LIB_CLIB_DEPS)
LIB_LIBS += $(LIB_LIB_CLIB)