#include "timestamp.h"

#include <stdio.h>
#include <iostream>
#include <vector>

using namespace simple_reactor;

void benchmark();

int main(int argc, char* argv[]) {
  Timestamp now(Timestamp::Now());
  std::cout << "timestamp test:now= " << now.ToFormattedString() << "\n";

  now = Timestamp::Now();
  for (int i = 0; i < 1000; ++i) {
    Timestamp temp(Timestamp::Now());
  }
  Timestamp end(Timestamp::Now());
  std::cout << "\t 1000 now use time=" << end.Ns() - now.Ns() << "\n";
  now = Timestamp::Now();
  for (int i = 0; i < 1000; ++i) {
    Timestamp temp(Timestamp::Now());
    std::string str_temp = temp.ToString();
  }
  end = Timestamp::Now();
  std::cout << "\t 1000 tostring now use time=" << end.Ns() - now.Ns() << "\n";
  now = Timestamp::Now();
  for (int i = 0; i < 1000; ++i) {
    Timestamp temp(Timestamp::Now());
    std::string str_temp = temp.ToFormattedString();
  }
  end = Timestamp::Now();
  std::cout << "\t 1000 toformat now use time=" << end.Ns() - now.Ns() << "\n";
  std::cout << "benchmark test:now= " << now.ToFormattedString() << "\n";
  benchmark();
}

void benchmark() {
  const int kNum = 1000 * 1000;
  std::vector<Timestamp> stamps;
  stamps.reserve(kNum);

  for (int i = 0; i < kNum; ++i) {
    stamps.push_back(Timestamp::Now());
  }
  std::cout << "first=" << stamps.front().ToString()
            << "\tlast=" << stamps.back().ToString()
            << "\tDiff="<< stamps.back().Ns() - stamps.front().Ns() << "\n";

  int incre[100] = {0};
  int64_t start = stamps.front().Ns();
  for (int i = 1; i < kNum; ++i) {
    int64_t next = stamps[i].Ns();
    int inc = next - start;
    start = next;
    if (inc < 0) {
      std::cout << "error..inc=" << inc << "\n";
    } else if (inc < 100) {
      ++incre[inc];
    } else {
      std::cout << "big gap.inc=" << inc << "\n";
    }
  }

  for (int i = 0; i < 100; ++i) {
    if (incre[i] > 0) {
      std::cout << "\t" << i << ":\t" << incre[i] << "\n";
    }
  }
}
