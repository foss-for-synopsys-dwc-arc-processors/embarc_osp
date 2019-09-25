COMPONENTS = mc bmain

.PHONY: default clean 

export NSIM_MULTICORE=1
SHELL=/bin/bash

BOARD := nsim
CUR_CORE := archs
Verb := @
MDB := mdb

default: exe

exe: 
	for f in $(COMPONENTS); do make -C $$f BOARD=$(BOARD) CUR_CORE=$(CUR_CORE) ; done

clean:
	for f in $(COMPONENTS); do make -C $$f BOARD=$(BOARD) CUR_CORE=$(CUR_CORE) clean ; done


define configure_mdb_core
  $(Verb) $(MDB) $(CONNECT_OPT)$(2) -pset=$(1) -psetname=$(CPU_PREFIX)$(1) $(MDB_CORE_OPT) $(TARGET)
endef

NSIM_CONNECT_OPT=-nsim -DLL=$(TOOLSDIR)arccli,shmem=1234
MDB_OPT=-OK -nosplash -prelaunch="${NSIM_HOME}/bin/arcsrv${EXE} -DLL=${NSIM_HOME}/lib/libsim -shmem=1234" -prelaunchwd= -prelaunchdelay=2000

sim: exe
	$(Verb) $(MDB) $(NSIM_CONNECT_OPT) -pset=1 -psetname=mc @mc/obj_nsim_10/mw_archs/embARC_generated/mdb.arg mc/obj_nsim_10/mw_archs/mc_mw_archs.elf
	#$(Verb) $(MDB) $(NSIM_CONNECT_OPT) -pset=2 -psetname=bmain @bmain/obj_nsim_10/mw_archs/embARC_generated/mdb.arg bmain/obj_nsim_10/mw_archs/bmain_mw_archs.elf
	$(MDB) $(MDB_OPT) -multifiles=bmain
