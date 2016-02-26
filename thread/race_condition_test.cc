#include <stdio.h>
#include <iostream>
#include <boost/bind.hpp>

#include "thread.h"
#include <time/timestamp.h>
#include <logging/logging.h>

using namespace simple_reactor;

Timestamp g_stamp;

void Fuc(int x) {
  //printf("pid=%d, tid=%d\n", ::getpid(), simple_reactor::CurrentThread::Tid());
  g_stamp = Timestamp::Now();
  LOG_INFO << "id=" << x<< ",time=" << g_stamp.ToFormattedString();
  for (int i = 0; i < 10; i++) {
    if (x == 0) {
      //std::cout << "threadid=" << CurrentThread::Tid() << ",time=" << g_stamp.ToFormattedString() << "\n";
      LOG_INFO << x << "==========================time=" << g_stamp.ToFormattedString() << "";
    }
  }
}

int main(int argc, char* argv[]) {
  printf("pid=%d, tid=%d\n", ::getpid(), simple_reactor::CurrentThread::Tid());
  Thread* td[10];
  for (int i = 0; i < 10; ++i) {
    td[i] = new Thread(boost::bind(Fuc,i));
    td[i]->Start();
  }
  for (int i = 0; i < 10; ++i) {
    td[i]->Join();
    delete td[i];
  }
}
