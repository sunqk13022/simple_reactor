CXXFLAGS=-g -Wall -O0 -I../
#CXXFLAGS=-g -Wall -O2 -I../
LDFLAGS= -DBOOST_TEST_DYN_LINK -lboost_unit_test_framework -lpthread  -L.
CC=g++

BINARIES=log_stream_test \
         log_stream_bench \
         logging_test
LIBNAME=libreactor_log.a

all: $(LIBNAME) $(BINARIES)



LIB_SRC := log_stream.cc logging.cc
LIB_OBJ := $(patsubst %.cc, %.o, $(LIB_SRC))
$(LIBNAME): $(LIB_OBJ)
	ar rcs $@ $^

$(LIB_OBJ): %.o: %.cc
	$(CC) -c $(CXXFLAGS) $< -o $@

$(BINARIES):
	$(CC) $(CXXFLAGS) -o $@  $^ $(LDFLAGS)

log_stream_test: log_stream_test.cc $(LIBNAME)
log_stream_bench: log_stream_bench.cc $(LIBNAME) ../time/timestamp.cc
logging_test: logging_test.cc $(LIBNAME) ../time/timestamp.cc

clean:
	rm -rf $(LIB_OBJ) $(BINARIES) $(LIBNAME) *.o
