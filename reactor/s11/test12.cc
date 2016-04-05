#include <stdio.h>
#include <iostream>

#include <sys/timerfd.h>

#include <boost/bind.hpp>

#include <thread/thread.h>
#include <logging/logging.h>

#include "event_loop.h"
#include "event_loop_thread.h"
#include "channel.h"
#include "acceptor.h"
#include "socket_fun.h"
#include "inet_address.h"
#include "tcp_server.h"
#include "connector.h"

using namespace simple_reactor;

EventLoop* g_loop;

void connectCallback(int sockfd) {
  printf("connected.\n");
  g_loop->Quit();
}

int main(int argc, char* argv[]) {
  printf("main():pid = %d, tid=%d \n", getpid(), CurrentThread::Tid());


  InetAddress listen_addr("127.0.0.1", 12820);
  EventLoop loop;
  g_loop = &loop;

  ConnectorPtr connector(new Connector(&loop, listen_addr));
  connector->SetNewConnectionCallback(connectCallback);
  connector->Start();

  loop.Loop();

  printf("exit main().\n");
}
