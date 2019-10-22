PROG=pmu.so
OBJS=pmu.o

ifdef GCC
  CXX=$(GCC)/bin/g++
else
  CXX=g++
endif

CXXFLAGS=-O2 -m32
LDFLAGS=-m32 -shared

INC=-I$(NSIM_HOME)/inc

CXXFLAGS:=$(CXXFLAGS) -fPIC
LDFLAGS:=$(LDFLAGS) -fPIC -rdynamic

all: $(PROG)

%.o: %.cpp
	$(Verb) $(CXX) $(CXXFLAGS) $(INC) -o $@ -c $<

$(PROG): $(OBJS)
	$(Verb) $(CXX) $(LDFLAGS) -o $@ $<

clean:
	$(Verb) $(RM) $(OBJS) $(PROG)

.PHONY: clean
