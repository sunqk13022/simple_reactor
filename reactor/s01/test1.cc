#include <stdio.h>
#include <iostream>

#include <thread/thread.h>

#include "event_loop.h"

using namespace simple_reactor;

void Run() {
  printf("Run: pid = %d, tid=%d\n", getpid(), CurrentThread::Tid());
  EventLoop loop;
  loop.Loop();
}

int main(int argc, char* argv[]) {
  printf("main: pid = %d, tid=%d\n", getpid(), CurrentThread::Tid());
  EventLoop loop;
  Thread thread(Run);
  thread.Start();

  loop.Loop();
  pthread_exit(NULL);
}
