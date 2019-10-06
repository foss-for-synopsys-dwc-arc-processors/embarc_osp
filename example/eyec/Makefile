COMPONENTS = mc bmain bootloader

export NSIM_MULTICORE=1
SHELL=/bin/bash
ROOT=.
export LINKER_SCRIPT_FILE=$(abspath $(ROOT)/common/linker_template_mw.ld)
export TCF=$(abspath $(ROOT)/common/archs38_c0.tcf)
BOARD := nsim
CUR_CORE := archs
Verb := @
MDB := mdb

ifeq ($(USE_CL), 1)
	MDB_CL := -cl
else
	MDB_CL := 
endif
.PHONY: default clean sim run
default: exe

exe: 
	for f in $(COMPONENTS); do $(MAKE) -C $$f LINKER_SCRIPT_FILE=$(LINKER_SCRIPT_FILE) BOARD=$(BOARD) CUR_CORE=$(CUR_CORE) ; done

clean:
	for f in $(COMPONENTS); do $(MAKE) -C $$f BOARD=$(BOARD) CUR_CORE=$(CUR_CORE) clean ; done

print-%: 
	@for f in $(COMPONENTS); do echo $$f; $(MAKE) -C $$f BOARD=$(BOARD) CUR_CORE=$(CUR_CORE) print-$* ; done

define configure_mdb_core
  $(Verb) $(MDB) $(CONNECT_OPT)$(2) -pset=$(1) -psetname=$(CPU_PREFIX)$(1) $(MDB_CORE_OPT) $(TARGET)
endef

NSIM_CONNECT_OPT=-nsim -DLL=$(TOOLSDIR)arccli,shmem=1234
MDB_OPT=-OK -nosplash -prelaunch="${NSIM_HOME}/bin/arcsrv${EXE} -DLL=${NSIM_HOME}/lib/libsim -shmem=1234" -prelaunchwd= -prelaunchdelay=2000

sim: exe
	/bin/rm -rf $(ROOT)/.sc.project
	$(Verb) $(MDB) $(NSIM_CONNECT_OPT) -pset=1 -psetname=bmain @bmain/obj_nsim_10/mw_archs/embARC_generated/mdb.arg bmain/obj_nsim_10/mw_archs/bmain_mw_archs.elf
	$(Verb) $(MDB) $(NSIM_CONNECT_OPT) -pset=2 -psetname=mc @mc/obj_nsim_10/mw_archs/embARC_generated/mdb.arg mc/obj_nsim_10/mw_archs/mc_mw_archs.elf
	$(MDB) $(MDB_OPT) $(MDB_CL) -multifiles=bmain,mc

bl: exe
	/bin/rm -rf $(ROOT)/.sc.project
	$(Verb) $(MDB) -nogoifmain $(NSIM_CONNECT_OPT) -pset=1 -psetname=core0 @bootloader/obj_nsim_10/mw_archs/embARC_generated/mdb.arg bootloader/obj_nsim_/mw_archs/bootloader_mw_archs.elf
	$(Verb) $(MDB) -nogoifmain $(NSIM_CONNECT_OPT) -pset=2 -psetname=core1 @bootloader/obj_nsim_10/mw_archs/embARC_generated/mdb.arg bootloader/obj_nsim_/mw_archs/bootloader_mw_archs.elf
	$(MDB) $(MDB_OPT) $(MDB_CL) -multifiles=core0,core1

run: MDB_CL=-cl 
run: sim
