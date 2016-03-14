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

using namespace simple_reactor;

void newConnection(int sockfd, const InetAddress& peer_addr) {
  printf("newConnection(): accepted a new connection from %s\n",
         peer_addr.ToHostPort().c_str());
  ::write(sockfd, "How are you?\n", 13);
  socket_fun::Close(sockfd);
}

int main(int argc, char* argv[]) {
  printf("main():pid = %d, tid=%d \n", getpid(), CurrentThread::Tid());
  InetAddress listen_addr(12820);
  EventLoop loop;

  Acceptor acceptor(&loop, listen_addr);
  acceptor.SetNewConnectionCallback(newConnection);
  acceptor.Listen();

  loop.Loop();

  printf("exit main().\n");
}
