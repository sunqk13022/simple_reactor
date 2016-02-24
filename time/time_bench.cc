#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <stdint.h>

uint64_t GetTimeStampInUs() {
  struct timeval tv = {0, 0};
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

int main(int argc, char* argv[]) {
  int N = atoi(argv[1]);
  int which = atoi(argv[2]);
  const char* name = "unknown";
  uint64_t start = GetTimeStampInUs();
  if (which == 0) {
    name = "gettimeofday";
    struct timeval tv;
    for (int i = 0; i < N; ++i) {
      gettimeofday(&tv, NULL);
    }
  } else if (which ==1) {
    name = "time";
    for (int i = 0; i < N; ++i) {
      time(NULL);
    }
  } else if (which == 2) {
    name = "clock_gettime";
    for (int i = 0; i < N; ++i) {
      struct timespec ts;
      clock_gettime(CLOCK_REALTIME, &ts);
    }
  } else {
    name = "getppid";
    for (int i = 0; i < N; ++i) {
      getppid();
    }
  }
  uint64_t end = GetTimeStampInUs();
  //std::cout << (end-start) / 1000000 << " seconds \n";
  std::cout << "name=" << name <<"\n\t";
  std::cout << (end-start) << " us \n";
}
