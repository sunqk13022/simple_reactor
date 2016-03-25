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

using namespace simple_reactor;

void onConnection(const TcpConnectionPtr& conn) {
  if (conn->GetConnected()) {
    printf("onConnection(): new connection [%s] from %s\n",
           conn->GetName().c_str(),
           conn->GetPeerAddr().ToHostPort().c_str());
  } else {
    printf("onConnection(): connection [%s] is down\n",
           conn->GetName().c_str());
  }
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime) {
  printf("onMessage(): received %zd bytes from connection [%s] at [%s]\n",
         buf->GetReadableBytes(),
         conn->GetName().c_str(),\
         receiveTime.ToFormattedString().c_str());
}

int main(int argc, char* argv[]) {
  printf("main():pid = %d, tid=%d \n", getpid(), CurrentThread::Tid());
  InetAddress listen_addr(12820);
  EventLoop loop;

  TcpServer server(&loop, listen_addr);
  server.SetConnectionCallback(onConnection);
  server.SetMessageCallback(onMessage);

  server.Start();

  loop.Loop();

  printf("exit main().\n");
}
