CXXFLAGS=-g -Wall -O2 -I../../
LDFLAGS= -lpthread -lsimreactor -L.
CC=g++

BINARIES=test1
LIBNAME=libsimreactor.a

all: $(BINARIES)

$(BINARIES):$(LIBNAME)
	$(CC) $(CXXFLAGS) -o $@  $(filter %.cc,$^) $(LDFLAGS)

test1: test1.cc

BASE_SRC :=../../thread/thread.cc \
          ../../logging/log_stream.cc \
          ../../logging/logging.cc

LIB_SRC :=
SUM_SRC = $(notdir $(LIB_SRC) $(BASE_SRC))
LIB_OBJ = $(patsubst %.cc,%.o,$(SUM_SRC))
$(LIBNAME): $(LIB_SRC) $(BASE_SRC)
	$(CC)  $(CXXFLAGS) -c $^
	ar rcs $@ $(LIB_OBJ)


clean:
	rm -rf $(LIB_OBJ) $(BINARIES) $(LIBNAME) *.o *.dSYM *.txt
