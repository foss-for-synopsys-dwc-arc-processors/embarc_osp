LIBXIVELY := $(shell git rev-parse --show-toplevel)

include $(LIBXIVELY)/Makefile.include

XI_EXAMPLE_SOURCES = $(wildcard *.c)
XI_CFLAGS += -I../libxively/
XI_EXAMPLES = $(addprefix $(XI_BINDIR)/,$(XI_EXAMPLE_SOURCES:.c=))

all: $(XI_EXAMPLES)

$(XI_BINDIR)/%: %.c $(XI)
	@-mkdir -p $(dir $@)
	$(CC) $(XI_CFLAGS) $^ -o $@

$(XI):
	$(MAKE) -C .. libxively

include $(LIBXIVELY)/Makefile.rules
