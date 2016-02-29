CXXFLAGS=-g -Wall -O2 -pthread -I../../
LDFLAGS= -lpthread -lsimreactor -L.
CC=g++

BINARIES=test1 \
         test2 \
         test3
LIBNAME=libsimreactor.a

all: $(BINARIES)

$(BINARIES):$(LIBNAME)
	$(CC) $(CXXFLAGS) -o $@  $(filter %.cc,$^) $(LDFLAGS)

test1: test1.cc
test2: test2.cc
test3: test3.cc

BASE_SRC :=../../thread/thread.cc \
          ../../time/timestamp.cc \
          ../../logging/log_stream.cc \
          ../../logging/logging.cc

LIB_SRC :=event_loop.cc \
          channel.cc \
          poller.cc \
          channel.cc


SUM_SRC = $(notdir $(LIB_SRC) $(BASE_SRC))
LIB_OBJ = $(patsubst %.cc,%.o,$(SUM_SRC))
$(LIBNAME): $(LIB_SRC) $(BASE_SRC)
	$(CC)  $(CXXFLAGS) -c $^
	ar rcs $@ $(LIB_OBJ)


clean:
	rm -rf $(LIB_OBJ) $(BINARIES) $(LIBNAME) *.o *.dSYM *.txt
