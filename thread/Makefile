CXXFLAGS=-g -Wall -rdynamic -march=native
CXXFLAGS+=-O2

HEADERS=$(wildcard *.h)

TESTS = atomic_test \
				thread_test

all: $(TESTS)

$(TESTS) : $(HEADERS)
	g++ $(CXXFLAGS) -o $@ $(filter %.cc, $^) -lpthread

atomic_test: test/atomic_test.cc
thread_test: test/thread_test.cc thread.cc

clean:
	rm -f $(TESTS)
