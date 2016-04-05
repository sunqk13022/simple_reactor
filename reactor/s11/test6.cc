#include <stdio.h>
#include <iostream>

#include <sys/timerfd.h>

#include <boost/bind.hpp>

#include <thread/thread.h>
#include <logging/logging.h>

#include "event_loop.h"
#include "event_loop_thread.h"
#include "channel.h"

using namespace simple_reactor;

void runInThread() {
  printf("runInThread():pid = %d, tid=%d \n", getpid(), CurrentThread::Tid());
}

int main(int argc, char* argv[]) {
  printf("main():pid = %d, tid=%d \n", getpid(), CurrentThread::Tid());
  EventLoopThread loopThread;
  EventLoop* loop = loopThread.StartLoop();

  loop->RunInLoop(runInThread);
  sleep(1);
  printf("main():pid = %d, tid=%d \n", getpid(), CurrentThread::Tid());
  loop->RunAfter(2, runInThread);
  sleep(3);
  printf("main():pid = %d, tid=%d \n", getpid(), CurrentThread::Tid());
  loop->Quit();

  printf("exit main().\n");
}
