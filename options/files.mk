##
# \brief	definitions for source codes directories
##

## Device HAL Related ##
DEVHAL_ASMSRCDIR	=
DEVHAL_CSRCDIR		=
DEVHAL_CXXSRCDIR	=
DEVHAL_INCDIR		=

## Extra Related ##
### These source code will not be compiled and linked as library
EXTRA_ASMSRCDIR		+=
EXTRA_CSRCDIR		+=
EXTRA_CXXSRCDIR		+=
EXTRA_INCDIR		+= $(EMBARC_ROOT)/inc $(EMBARC_ROOT)/inc/arc $(EMBARC_GENERATED_DIR)

###############################################################################
## ALL ##
CSRCS_DIR		= $(CPU_CSRCDIR) $(OS_CSRCDIR) $(DEVHAL_CSRCDIR) $(BOARD_CSRCDIR) \
				$(MID_CSRCDIR) $(LIB_CSRCDIR) $(EXTRA_CSRCDIR) $(APPL_CSRC_DIR)
CXXSRCS_DIR		= $(CPU_CXXSRCDIR) $(OS_CXXSRCDIR) $(DEVHAL_CXXSRCDIR) $(BOARD_CXXSRCDIR) \
				$(MID_CXXSRCDIR) $(LIB_CXXSRCDIR) $(EXTRA_CXXSRCDIR) $(APPL_CXXSRC_DIR)
ASMSRCS_DIR		= $(CPU_ASMSRCDIR) $(OS_ASMSRCDIR) $(DEVHAL_ASMSRCDIR) $(BOARD_ASMSRCDIR) \
				$(MID_ASMSRCDIR) $(LIB_ASMSRCDIR) $(EXTRA_ASMSRCDIR) $(APPL_ASMSRC_DIR)
INCS_DIR		= $(CPU_INCDIR) $(OS_INCDIR) $(DEVHAL_INCDIR) $(BOARD_INCDIR) \
				$(MID_INCDIR) $(LIB_INCDIR) $(EXTRA_INCDIR) $(APPL_INC_DIR)
###############################################################################

##
# Collect Application Source Files
##
APPL_CSRCS += $(call get_csrcs, $(APPL_CSRC_DIR))
APPL_ASMSRCS += $(call get_asmsrcs, $(APPL_ASMSRC_DIR))
APPL_CXXSRCS += $(call get_cxxsrcs, $(APPL_CXXSRC_DIR))

##
# Collect Extra Source Files
##
EXTRA_CSRCS += $(call get_csrcs, $(EXTRA_CSRCDIR))
EXTRA_ASMSRCS += $(call get_asmsrcs, $(EXTRA_ASMSRCDIR))
EXTRA_CXXSRCS += $(call get_cxxsrcs, $(EXTRA_CXXSRCDIR))

##
# Collect Application Object Files
##
APPL_COBJS = $(call get_appobjs, $(APPL_CSRCS))
APPL_ASMOBJS = $(call get_appobjs, $(APPL_ASMSRCS))
APPL_CXXOBJS = $(call get_appobjs, $(APPL_CXXSRCS))
APPL_OBJS = $(APPL_COBJS) $(APPL_ASMOBJS) $(APPL_CXXOBJS)

##
# Collect Extra Object Files
##
EXTRA_COBJS = $(call get_relobjs, $(EXTRA_CSRCS))
EXTRA_ASMOBJS = $(call get_relobjs, $(EXTRA_ASMSRCS))
EXTRA_CXXOBJS = $(call get_relobjs, $(EXTRA_CXXSRCS))
EXTRA_OBJS = $(EXTRA_COBJS) $(EXTRA_ASMOBJS) $(EXTRA_CXXOBJS)

#########ALL DIRECTOIRES & SOURCE FILES & OBJECT FILES COLLECTION#############
##
# Collect all include/c/asm/cpp directories
##
ALL_INC_DIRS = $(sort $(INCS_DIR))
ALL_CSRC_DIRS = $(sort $(CSRCS_DIR))
ALL_ASMSRC_DIRS = $(sort $(ASMSRCS_DIR))
ALL_CXXSRC_DIRS = $(sort $(CXXSRCS_DIR))

##
# Collect all sources files
##
ALL_CSRCS = $(sort $(CPU_CSRCS) $(BOARD_CSRCS) $(LIB_CSRCS) $(MID_CSRCS) $(OS_CSRCS) $(APPL_CSRCS) $(EXTRA_CSRCS))
ALL_ASMSRCS = $(sort $(CPU_ASMSRCS) $(BOARD_ASMSRCS) $(LIB_ASMSRCS) $(MID_ASMSRCS) $(OS_ASMSRCS) $(APPL_ASMSRCS) $(EXTRA_ASMSRCS))
ALL_CXXSRCS = $(sort $(CPU_CXXSRCS) $(BOARD_CXXSRCS) $(LIB_CXXSRCS) $(MID_CXXSRCS) $(OS_CXXSRCS) $(APPL_CXXSRCS) $(EXTRA_CXXSRCS))
ALL_ALLSRCS = $(sort $(ALL_CSRCS) $(ALL_ASMSRCS) $(ALL_CXXSRCS))

##
# Collect all object files
##
ALL_COBJS = $(sort $(CPU_COBJS) $(BOARD_COBJS) $(LIB_COBJS) $(MID_COBJS) $(OS_COBJS) $(APPL_COBJS) $(EXTRA_COBJS))
ALL_ASMOBJS = $(sort $(CPU_ASMOBJS) $(BOARD_ASMOBJS) $(LIB_ASMOBJS) $(MID_ASMOBJS) $(OS_ASMOBJS) $(APPL_ASMOBJS) $(EXTRA_ASMOBJS))
ALL_CXXOBJS = $(sort $(CPU_CXXOBJS) $(BOARD_CXXOBJS) $(LIB_CXXOBJS) $(MID_CXXOBJS) $(OS_CXXOBJS) $(APPL_CXXOBJS) $(EXTRA_CXXOBJS))
ALL_ALLOBJS = $(sort $(ALL_COBJS) $(ALL_ASMOBJS) $(ALL_CXXOBJS))
##############################################################################