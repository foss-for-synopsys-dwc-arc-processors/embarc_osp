LIBXIVELY := $(shell git rev-parse --show-toplevel)

include $(LIBXIVELY)/Makefile.include
include Makefile.defaults

XI_TEST_SUITE ?= core

XI_TEST_SOURCES := $(XI_TEST_SUITE).c $(XI) $(XI_OBJDIR)/tinytest/tinytest.o

TINYTEST_SRCDIR ?= ../import/tinytest/

XI_CFLAGS += -I../libxively/ -I$(TINYTEST_SRCDIR)

ifneq ($(XI_UNIT_TEST_TARGET),native)
  XI_CFLAGS += -DNO_FORKING
endif

XI_TEST = $(XI_BINDIR)/libxively_$(XI_UNIT_TEST_TARGET)_unit_test

.PHONY: $(XI_TEST)
all: $(XI_TEST)

include Makefile.$(XI_UNIT_TEST_TARGET)

$(XI_OBJDIR)/tinytest/tinytest.o: $(TINYTEST_SRCDIR)/tinytest.c
	@-mkdir -p $(dir $@)
	$(CC) $(XI_CFLAGS) -c $< -o $@

$(XI):
	$(MAKE) -C .. libxively

include $(LIBXIVELY)/Makefile.rules
