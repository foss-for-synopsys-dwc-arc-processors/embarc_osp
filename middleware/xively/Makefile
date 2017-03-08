REV := $(shell git rev-parse --short --no-symbolic HEAD)

all:
	$(MAKE) -C src examples
docs:
	doxygen
clean:
	-rm -rf doc/html
	$(MAKE) -C src $@

ci_msp430:
	$(MAKE) -C src clean
	$(MAKE) -C src libxively \
		CC=msp430-elf-gcc AR=msp430-elf-ar \
		XI_DEBUG_ASSERT=0 XI_DEBUG_OUTPUT=0 \
		XI_IO_LAYER=dummy

ci_avr:
	$(MAKE) -C src clean
	$(MAKE) -C src libxively \
		CC=avr-gcc AR=avr-ar \
		XI_DEBUG_ASSERT=0 XI_DEBUG_OUTPUT=0 \
		XI_IO_LAYER=dummy

MBED_HEAD ?= HEAD
MBED_TEMP ?= mbed_mercurial
MBED_USER ?= xively
MBED_REPO ?= libxively-develop

MBED_URL := https://$(MBED_USER)@mbed.org/users/$(MBED_USER)/code/$(MBED_REPO)/

MBED_REV := $(shell git rev-parse --short --no-symbolic $(MBED_HEAD))

MBED_MSG := "Updated from git revision $(MBED_REV)"

update_mbed_repo:
	hg clone $(MBED_URL) $(MBED_TEMP)
	-rm -rf $(MBED_TEMP)/src
	git archive $(MBED_REV) \
		"src/libxively/*.[ch]" \
		"src/libxively/io/mbed/" \
		| tar x -C $(MBED_TEMP)
	echo "#define XI_VERSION \"0.1.x-$(MBED_REV)\"" \
		> $(MBED_TEMP)/src/libxively/xi_version.h
	hg commit --repository $(MBED_TEMP) --addremove \
		--user $(MBED_USER) --message $(MBED_MSG)
	-hg push --repository $(MBED_TEMP)
	-rm -rf $(MBED_TEMP)

update_docs_branch:
	-rm -rf doc/html
	doxygen && cd doc/html \
		&& git init \
		&& git remote add github git@github.com:xively/libxively \
		&& git add . \
		&& git commit -m "[docs] Regerated documentation for $(REV)" \
		&& git push github master:gh-pages -f
