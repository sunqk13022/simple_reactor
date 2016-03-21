#include <stdio.h>
#include <iostream>

#include <sys/timerfd.h>

#include <boost/bind.hpp>

#include <thread/thread.h>
#include <logging/logging.h>

#include "event_loop.h"
#include "channel.h"

using namespace simple_reactor;

EventLoop* g_loop;
int cnt = 0;

void printTid() {
  printf("pid = %d,tid = %d\n", getpid(), CurrentThread::Tid());
  printf("now %s\n", Timestamp::Now().ToFormattedString().c_str());
}

void print(const char* msg) {
  printf("msg %s %s\n", Timestamp::Now().ToFormattedString().c_str(), msg);
  if (++cnt == 20) {
    g_loop->Quit();
  }
}

int main(int argc, char* argv[]) {
  printTid();
  EventLoop loop;
  g_loop = &loop;

  print("main");
  loop.RunAfter(1, boost::bind(print, "once1"));
  loop.RunAfter(1.5, boost::bind(print, "once1.5"));
  loop.RunAfter(2.5, boost::bind(print, "once2.5"));
  loop.RunAfter(3.5, boost::bind(print, "once3.5"));

  loop.RunEvery(2, boost::bind(print, "every2"));
  loop.RunEvery(3, boost::bind(print, "every3"));

  loop.Loop();

  print("main loop exists");
  sleep(1);
}
