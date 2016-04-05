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
int g_flag = 0;

void printTid() {
  printf("pid = %d,tid = %d\n", getpid(), CurrentThread::Tid());
  printf("now %s\n", Timestamp::Now().ToFormattedString().c_str());
}

void run4() {
  printf("run4():pid = %d, flag=%d \n", getpid(), g_flag);
  g_loop->Quit();
}

void run3() {
  printf("run3():pid = %d, flag=%d \n", getpid(), g_flag);
  g_loop->RunAfter(3, run4);
  g_flag = 3;
}

void run2() {
  printf("run2():pid = %d, flag=%d \n", getpid(), g_flag);
  g_loop->QueueInLoop(run3);
}

void run1() {
  g_flag = 1;
  printf("run1():pid = %d, flag=%d \n", getpid(), g_flag);
  g_loop->RunInLoop(run2);
  g_flag = 2;
}

int main(int argc, char* argv[]) {
  printf("main():pid = %d, flag=%d \n", getpid(), g_flag);
  EventLoop loop;
  g_loop = &loop;

  loop.RunAfter(1.5, run1);
  loop.Loop();

  sleep(1);
}
