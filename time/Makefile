CXXFLAGS=-g -Wall -O0
LDFLAGS = -lpthread -lrt
BINARIES=time_bench
CC=g++

all: $(BINARIES)

$(BINARIES):
	$(CC) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

CPP_SRC:=$(shell find ./ -name *.cc)
OBJ:=$(CPP_SRC:%.cc=%.o)

$(OBJ): %.o: %.cc
	$(CC) -c $(CXXFLAGS) $< -o $@


time_bench: time_bench.o

clean:
	rm -rf $(OBJ) $(BINARIES)
