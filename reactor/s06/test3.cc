#include <stdio.h>
#include <iostream>

#include <sys/timerfd.h>

#include <thread/thread.h>
#include <logging/logging.h>

#include "event_loop.h"
#include "channel.h"

using namespace simple_reactor;

EventLoop* g_loop;

void timeout() {
  //printf("Run: pid = %d, tid=%d\n", getpid(), CurrentThread::Tid());
  printf("Timeout!\n");
  g_loop->Quit();
}

int main(int argc, char* argv[]) {
  //printf("main: pid = %d, tid=%d\n", getpid(), CurrentThread::Tid());
  //Logger::SetLogLevel(Logger::ERROR);
  EventLoop loop;
  g_loop = &loop;

  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  Channel ch(g_loop, timerfd);
  ch.SetReadCallback(timeout);
  ch.EnableReading();

  struct itimerspec howlong;
  bzero(&howlong, sizeof(howlong));
  howlong.it_value.tv_sec = 2;
  ::timerfd_settime(timerfd, 0, &howlong, NULL);

  loop.Loop();
  ::close(timerfd);
}
