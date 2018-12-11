# dir declaration
MID_FATFS_DIR = $(MIDDLEWARES_ROOT)/fatfs

MID_FATFS_ASMSRCDIR	= $(MID_FATFS_DIR)/source
MID_FATFS_CSRCDIR	= $(MID_FATFS_DIR)/source $(MID_FATFS_DIR)/source/diskdrv
MID_FATFS_INCDIR	= $(MID_FATFS_DIR)/source $(MID_FATFS_DIR)/source/diskdrv

# find all the source files in the target directories
MID_FATFS_CSRCS = $(call get_csrcs, $(MID_FATFS_CSRCDIR))
MID_FATFS_ASMSRCS = $(call get_asmsrcs, $(MID_FATFS_ASMSRCDIR))

# get object files
MID_FATFS_COBJS = $(call get_relobjs, $(MID_FATFS_CSRCS))
MID_FATFS_ASMOBJS = $(call get_relobjs, $(MID_FATFS_ASMSRCS))
MID_FATFS_OBJS = $(MID_FATFS_COBJS) $(MID_FATFS_ASMOBJS)

# get dependency files
MID_FATFS_DEPS = $(call get_deps, $(MID_FATFS_OBJS))

# extra macros to be defined
MID_FATFS_DEFINES = -DMID_FATFS

# genearte library
MID_LIB_FATFS = $(OUT_DIR)/libmidfatfs.a

# library generation rule
$(MID_LIB_FATFS): $(MID_FATFS_OBJS)
	$(TRACE_ARCHIVE)
	$(Q)$(AR) $(AR_OPT) $@ $(MID_FATFS_OBJS)

# specific compile rules
# user can add rules to compile this middleware
# if not rules specified to this middleware, it will use default compiling rules

# Middleware Definitions
MID_INCDIR += $(MID_FATFS_INCDIR)
MID_CSRCDIR += $(MID_FATFS_CSRCDIR)
MID_ASMSRCDIR += $(MID_FATFS_ASMSRCDIR)

MID_CSRCS += $(MID_FATFS_CSRCS)
MID_CXXSRCS +=
MID_ASMSRCS += $(MID_FATFS_ASMSRCS)
MID_ALLSRCS += $(MID_FATFS_CSRCS) $(MID_FATFS_ASMSRCS)

MID_COBJS += $(MID_FATFS_COBJS)
MID_CXXOBJS +=
MID_ASMOBJS += $(MID_FATFS_ASMOBJS)
MID_ALLOBJS += $(MID_FATFS_OBJS)

MID_DEFINES += $(MID_FATFS_DEFINES)
MID_DEPS += $(MID_FATFS_DEPS)
MID_LIBS += $(MID_LIB_FATFS)