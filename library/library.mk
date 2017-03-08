# library root declaration
LIBRARIES_ROOT = $(EMBARC_ROOT)/library
LIB_REQUIRED = clib

LIB_INCDIR =
LIB_CSRCDIR =
LIB_CXXSRCDIR =
LIB_ASMSRCDIR =

LIB_CSRCS =
LIB_CXXSRCS =
LIB_ASMSRCS =
LIB_ALLSRCS =

LIB_COBJS =
LIB_CXXOBJS =
LIB_ASMOBJS =
LIB_ALLOBJS =

LIB_DEFINES =
LIB_DEPS =
LIB_LIBS =

override LIB_SEL := $(strip $(LIB_SEL)) $(LIB_REQUIRED)
ifdef LIB_SEL
	LIB_SEL_SORTED = $(sort $(LIB_SEL))
	LIB_MKS = $(foreach LIB, $(LIB_SEL_SORTED), $(join $(LIB)/, $(LIB).mk))
	LIB_INCLUDES = $(foreach LIB_MK, $(LIB_MKS), $(wildcard $(addprefix $(LIBRARIES_ROOT)/, $(LIB_MK))))
	COMMON_COMPILE_PREREQUISITES += $(LIB_INCLUDES)
	include $(LIB_INCLUDES)

	# include dependency files
	ifneq ($(MAKECMDGOALS),clean)
	-include $(LIB_DEPS)
	endif
endif