#include <stdio.h>
#include <iostream>

#include <thread/thread.h>
#include <logging/logging.h>

#include "event_loop.h"

using namespace simple_reactor;

EventLoop* g_loop;

void Run() {
  //printf("Run: pid = %d, tid=%d\n", getpid(), CurrentThread::Tid());
  g_loop->Loop();
}

int main(int argc, char* argv[]) {
  //printf("main: pid = %d, tid=%d\n", getpid(), CurrentThread::Tid());
  Logger::SetLogLevel(Logger::ERROR);
  EventLoop loop;
  g_loop = &loop;
  Thread thread(Run);
  thread.Start();
  thread.Join();

  //loop.Loop();
  //pthread_exit(NULL);
}
