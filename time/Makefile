CXXFLAGS=-g -Wall -O0
LDFLAGS = -lpthread -lrt -L. -lreactor_time
BINARIES=time_bench \
         timestamp_test

CC=g++
LIBNAME=libreactor_time.a

all: $(LIBNAME) $(BINARIES)

SRC := $(wildcard *.cc)
OBJ:=$(SRC:%.cc=%.o)

$(BINARIES):
	$(CC) $(CXXFLAGS) $(LDFLAGS) $^ -o $@


$(OBJ): %.o: %.cc
	$(CC) -c $(CXXFLAGS) $< -o $@


time_bench: time_bench.o
timestamp_test: timestamp_test.o $(LIBNAME)

LIB_SRC := timestamp.cc
LIB_OBJ := $(patsubst %.cc, %.o, $(LIB_SRC))

$(LIBNAME): $(LIB_SRC)
	$(CC) $(CXXFLAGS) -c $^
	ar rcs $@ $(LIB_OBJ)
	cp $(LIBNAME) ../

clean:
	rm -rf $(OBJ) $(BINARIES) $(LIBNAME) ../$(LIBNAME)
