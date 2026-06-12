# This test by default prints a simple "Hello World!" and then exits
#   % make clean; make
#
# An interactive-mode is provided in which the simulation can be terminated
# by typing dollar character '$'
# 	% make clean; make INTERACTIVE=1

TESTNAME = plic-uart

GCC     = riscv64-elf-gcc
OBJDUMP = riscv64-elf-objdump

CFLAGS 	= -march=rv32gc -mabi=ilp32 -std=gnu11 

LDFLAGS = -march=rv32gc -mabi=ilp32 -nostartfiles -lm -lc -lgloss --specs=semihost.specs -Wl,--wrap=strcmp -T arcv.ld

NSIM_OPTS = -p semihosting=1 -o.isa_family=rv32 -o.isa_ext=-smdbltrp.-ssdbltrp -e $(TESTNAME).elf --quiet

INTERACTIVE = 0

COMP_OPTS += -DINTERACTIVE=$(INTERACTIVE)

.PHONY: app run build dump debug clean

default: run

$(TESTNAME).elf: entry.S $(TESTNAME).c
	$(GCC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(TESTNAME).dump: $(TESTNAME).elf
	$(OBJDUMP) -D $^ > $@

app: $(TESTNAME).elf

run: app
ifeq ($(INTERACTIVE),0)
	nsim $(NSIM_OPTS)
else
	nsim $(NSIM_OPTS)
endif

build: app

dump: $(TESTNAME).dump

debug:
	nsim $(NSIM_OPTS) --debug=4 --trace --log-filter=-DSP,-APS,-MEM,-SMART >& nsim.trc

clean:
	rm -f $(TESTNAME).elf $(TESTNAME).dump nsim.trc
