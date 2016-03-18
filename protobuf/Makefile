CXXFLAGS=-g -Wall -O0 -pthread
LDFLAGS=-lprotobuf -lz -lpthread

BINARIES=codec_test descriptor_test

all: $(BINARIES)

codec_test:query.pb.h query.pb.cc codec_test.cc codec.h

descriptor_test:query.pb.h query.pb.cc descriptor_test.cc codec.h

$(BINARIES):
	g++ $(CXXFLAGS) $(filter %.cc, $^) -o $@ $(LDFLAGS)

query.pb.h query.pb.cc:query.proto
	protoc --cpp_out . $<

clean:
	rm -f query.pb.*
	rm -f $(BINARIES)
