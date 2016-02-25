#include "log_stream.h"

#include <sstream>
#include <iostream>
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "time/timestamp.h"

using namespace simple_reactor;

const int N = 1000 * 1000;

template<typename T>
void benchPrintf(const char* fmt) {
  Timestamp start = Timestamp::Now();
  char buf[32];
  for (int i = 0; i < N; ++i) {
    snprintf(buf, sizeof(buf), fmt, (T)(i));
  }
  Timestamp end = Timestamp::Now();
  printf("benchPrintf %lld\n", end.Ns() - start.Ns());
}

template<typename T>
void benchStringStream() {
  Timestamp start = Timestamp::Now();
  std::ostringstream os;
  for (int i = 0; i < N; ++i) {
    os << (T)(i);
    os.seekp(0, std::ios_base::beg);
  }
  Timestamp end = Timestamp::Now();
  printf("benchStringStream %lld\n", end.Ns() - start.Ns());
}

template<typename T>
void benchLogstream() {
  Timestamp start = Timestamp::Now();
  LogStream os;
  for (int i = 0; i < N; ++i) {
    os << (T)(i);
    os.ResetBuffer();
  }
  Timestamp end = Timestamp::Now();
  printf("benchLogstream %lld\n", end.Ns() - start.Ns());
}

int main(int argc, char* argv[]) {
  std::cout << Timestamp::Now().ToFormattedString() << "\n";
  std::cout << "======================int test\n";
  benchPrintf<int>("%d");
  benchStringStream<int>();
  benchLogstream<int>();
  std::cout << "\n=====================double test\n";
  benchPrintf<double>("%.12g");
  benchStringStream<double>();
  benchLogstream<double>();
  std::cout << "\n=====================int64 test\n";
  benchPrintf<long>("%d" PRId64);
  benchStringStream<long>();
  benchLogstream<long>();
  std::cout << "\n=====================void* test\n";
  benchPrintf<void*>("%p");
  benchStringStream<void*>();
  benchLogstream<void*>();
}
