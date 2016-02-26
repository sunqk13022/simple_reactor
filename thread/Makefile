CXXFLAGS=-g -Wall
CXXFLAGS+=-O0 -I../
LDFLAGS = -pthread -lrt
CC=g++

LIBNAME=libreactor_thread.a
HEADERS=$(wildcard *.h)

SRC := $(wildcard *.cc)
OBJ:=$(SRC:%.cc=%.o)


TESTS = atomic_test \
        thread_test \
        race_condition_test

all: $(LIBNAME) $(TESTS)

$(TESTS) : $(HEADERS)
	$(CC) $(CXXFLAGS) $(LDFLAGS) $^ -o $@
#	g++ $(CXXFLAGS) $(LDFLAGS) -o $@ $(filter %.cc, $^)

atomic_test: test/atomic_test.cc
thread_test: test/thread_test.cc $(LIBNAME)
race_condition_test: race_condition_test.cc $(LIBNAME) ../time/timestamp.cc ../logging/log_stream.cc ../logging/logging.cc

$(OBJ): %.o: %.cc
	$(CC) -c $(CXXFLAGS) $< -o $@

LIB_SRC := thread.cc
LIB_OBJ := $(patsubst %.cc, %.o, $(LIB_SRC))

$(LIBNAME): $(LIB_SRC)
	$(CC) $(CXXFLAGS) -c $^
	ar rcs $@ $(LIB_OBJ)
#	cp $(LIBNAME) ../


clean:
	rm -f $(TESTS) $(LIBNAME) ../$(LIBNAME) $(OBJ)

.PHONY: clean
