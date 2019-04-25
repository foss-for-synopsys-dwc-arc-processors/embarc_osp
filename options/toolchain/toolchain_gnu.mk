ifeq ($(DEBUG), 1)
	CDEBUG_OPTION := -g
endif

ifeq ($(MAP), 1)
	LMAP_OPTION = -Wl,-M,-Map=$(APPL_FULL_NAME).map
endif

ifdef HEAPSZ
	HEAP_DEFINES := -D_HEAPSIZE=$(strip $(HEAPSZ))
else
	HEAP_DEFINES := -D_HEAPSIZE=$(strip $(DEFAULT_HEAPSZ))
endif

ifdef STACKSZ
	STACK_DEFINES := -D_STACKSIZE=$(strip $(STACKSZ))
else
	STACK_DEFINES := -D_STACKSIZE=$(strip $(DEFAULT_STACKSZ))
endif

TOOLCHAIN_DEFINES += $(HEAP_DEFINES) $(STACK_DEFINES) -D__GNU__

SUPPORTED_OLEVELS = O O0 O1 O2 O3 Os Os1 Oz Ofast Og
##
# Optimization level settings
# GCC Version For ARC
# refer to Using the GNU Compiler Collection (GCC)
# 3.10 Options That Control Optimization
# You can invoke GCC with ‘-Q --help=optimizers’
# to find out the exact set of optimizations
# that are enabled at each level.
##
## -O<number>	Set optimization level to <number>
## -Ofast	Optimize for speed disregarding exact standards compliance
## -Og		Optimize for debugging experience rather than speed or size
## -Os		Optimize for space rather than speed
ifeq ($(OLEVEL), O)
	#Optimize. Optimizing compilation takes
	#somewhat more time, and a lot more memory
	#for a large function.
	#equivalent to -O1
	OPT_OLEVEL = -O
else
ifeq ($(OLEVEL), O0)
	#Reduce compilation time and
	#make debugging produce the expected results
	#This is the default.
	OPT_OLEVEL = -O0
else
ifeq ($(OLEVEL), O1)
	#equivalent to -O
	OPT_OLEVEL = -O1
else
ifeq ($(OLEVEL), O2)
	#Optimize even more. GCC performs nearly all
	#supported optimizations that do not involve
	#a space-speed tradeoff. As compared to ‘-O’,
	#this option increases both compilation time
	#and the performance of the generated code
	OPT_OLEVEL = -O2
else
ifeq ($(OLEVEL), O3)
	#Optimize yet more. ‘-O3’ turns on all optimizations
	#specified by ‘-O2’ and also turns on the ‘-finline-functions’,
	#‘-funswitch-loops’, ‘-fpredictive-commoning’, ‘-fgcse-after-reload’,
	#‘-ftree-vectorize’, ‘-fvect-cost-model’,
	#‘-ftree-partial-pre’ and ‘-fipa-cp-clone’ options.
	OPT_OLEVEL = -O3
else
ifeq ($(OLEVEL), Os)
	#Optimize for size. ‘-Os’ enables all ‘-O2’ optimizations
	#that do not typically increase code size. It also performs
	#further optimizations designed to reduce code size
	OPT_OLEVEL = -Os
else
ifeq ($(OLEVEL), Os1)
	#here equivalent to -Os
	OPT_OLEVEL = -Os
else
ifeq ($(OLEVEL), Oz)
	#here equivalent to -Os
	OPT_OLEVEL = -Os
else
ifeq ($(OLEVEL), Ofast)
	#Disregard strict standards compliance.
	#‘-Ofast’ enables all ‘-O3’ optimizations. It also enables
	#optimizations that are not valid for all standardcompliant programs.
	OPT_OLEVEL = -Ofast
else
ifeq ($(OLEVEL), Og)
	#Optimize debugging experience. ‘-Og’ enables optimizations
	#that do not interfere with debugging.
	OPT_OLEVEL = -Og
else
	#no optimization option defined
	OPT_OLEVEL =
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif

##
# define tools
##
GNU_TOOLCHAIN_PREFIX := $(strip $(GNU_TOOLPATH))

ifneq ($(GNU_TOOLCHAIN_PREFIX), )
GNU_TOOLCHAIN_PREFIX := $(wildcard $(GNU_TOOLCHAIN_PREFIX))
endif

## GNU TOOLCHAIN TOOLS NAME DEFINITIONS ##
	CC	= arc-elf32-gcc
	CXX	= arc-elf32-g++
	AS	= arc-elf32-as
	DMP	= arc-elf32-objdump
	LD	= arc-elf32-gcc
	AR	= arc-elf32-ar
	NM	= arc-elf32-nm
	OBJCOPY	= arc-elf32-objcopy
	ELF2HEX	= arc-elf32-objcopy
	SIZE	= arc-elf32-size

## GNU TOOLCHAIN EXIST TESTING ##
GNU_TOOLCHAIN_PREFIX_TEST := $(wildcard $(GNU_TOOLCHAIN_PREFIX)/$(DMP)*)
ifneq ($(GNU_TOOLCHAIN_PREFIX_TEST), )
	CC	:= $(GNU_TOOLCHAIN_PREFIX)/$(CC)
	CXX	:= $(GNU_TOOLCHAIN_PREFIX)/$(CXX)
	AS	:= $(GNU_TOOLCHAIN_PREFIX)/$(AS)
	DMP	:= $(GNU_TOOLCHAIN_PREFIX)/$(DMP)
	LD	:= $(GNU_TOOLCHAIN_PREFIX)/$(LD)
	AR	:= $(GNU_TOOLCHAIN_PREFIX)/$(AR)
	NM	:= $(GNU_TOOLCHAIN_PREFIX)/$(NM)
	OBJCOPY	:= $(GNU_TOOLCHAIN_PREFIX)/$(OBJCOPY)
	ELF2HEX	:= $(GNU_TOOLCHAIN_PREFIX)/$(OBJCOPY)
	SIZE	:= $(GNU_TOOLCHAIN_PREFIX)/$(SIZE)
endif

	MAKE	= make
	DBG	?= arc-elf32-gdb

	NSIMDRV	= nsimdrv
	TCFGEN  = tcfgen
	TCFTOOL = tcftool

OPENOCD_SCRIPT_ROOT = $(dir $(shell $(CC) --print-prog-name=ld))../../share/openocd/scripts

## Don't change this line
override OPENOCD_SCRIPT_ROOT := $(subst \,/, $(strip $(OPENOCD_SCRIPT_ROOT)))

##
#   toolchain flags
##
libnsim = $(shell $(CC) -print-file-name=libnsim.a)
ifeq ($(libnsim),libnsim.a)
    # File doens't exists - old newlib.
    TOOLCHAIN_DEFINES += -U_HAVE_LIBGLOSS_
else
    TOOLCHAIN_DEFINES += -D_HAVE_LIBGLOSS_
endif

##
#   build options
##
	## Common Options
	MKDEP_OPT	= -MMD -MT $@ -MF $@.d
	COMMON_COMPILE_OPT = -ffunction-sections -fdata-sections -mno-sdata \
			$(OPT_OLEVEL) $(CDEBUG_OPTION) $(ALL_DEFINES) $(ALL_INCLUDES) $(MKDEP_OPT)

	## C/CPP/ASM/LINK Options
	COMPILE_OPT	+= $(CCORE_OPT_GNU)   $(ADT_COPT)   $(COMMON_COMPILE_OPT) -std=gnu99
	CXX_COMPILE_OPT	+= $(CXXCORE_OPT_GNU) $(ADT_CXXOPT) $(COMMON_COMPILE_OPT)
	ASM_OPT		+= $(ACORE_OPT_GNU)   $(ADT_AOPT)   $(COMMON_COMPILE_OPT) -x assembler-with-cpp
	LINK_OPT	+= $(LCORE_OPT_GNU)   $(ADT_LOPT) -Wl,--gc-sections \
				-mno-sdata -nostartfiles $(LMAP_OPTION) -lm -Wl,--script=$(APPL_LINK_FILE)

	## Link File Generation Options
	LINK_FILE_OPT	+= -x assembler-with-cpp $(ALL_INCLUDES) -E -P -nostdinc -undef -D__GNU__
	LINK_FILE_DEPOPT = $(LINK_FILE_OPT) -DENABLE_GENERATE_DEPENDCY_FILE $(MKDEP_OPT)

	## Other Options
	AR_OPT		+= -rcsT
	DMP_OPT		+= -x
	DASM_OPT	+= -D
	SIZE_OPT	+=

	LD_START_GROUPLIB	= -Wl,--start-group
	LD_SYSTEMLIBS		= -lm -lc -lgcc
	LD_END_GROUPLIB		= -Wl,--end-group
##
#   additional options
##
	ELF2HEX_INOPT		= -O ihex
	ELF2HEX_OUTOPT		=
	ELF2BIN_OPT		= -O binary