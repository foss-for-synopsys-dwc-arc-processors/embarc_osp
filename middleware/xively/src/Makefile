LIBXIVELY := $(shell git rev-parse --show-toplevel)

include $(LIBXIVELY)/Makefile.include

ifeq ($(MAKECMDGOALS),tests)
  include tests/Makefile.defaults
endif

XI_IO_LAYER ?= posix
XI_NOB_ENABLED ?= false

export XI_IO_LAYER
export XI_NOB_ENABLED

export XI_BINDIR
export XI_OBJDIR

.PHONY: libxively examples tests clean

libxively:
	$(MAKE) -C $@ deps
	$(MAKE) -C $@

examples:
	$(MAKE) -C $@

tests: clean libxively
	$(MAKE) -C $@

include $(LIBXIVELY)/Makefile.rules
