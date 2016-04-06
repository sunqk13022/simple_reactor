#include <stdio.h>
#include <iostream>
#include <utility>
#include <unistd.h>

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
#include "tcp_client.h"
#include "connector.h"

using namespace simple_reactor;

std::string message = "Hello\n";

void onConnection(const TcpConnectionPtr& conn) {
  if (conn->GetConnected()) {
    printf("onConnection(): tid=%d new connection [%s] from %s\n",
           CurrentThread::Tid(),
           conn->GetName().c_str(),
           conn->GetPeerAddr().ToHostPort().c_str());
    conn->Send(message);
  } else {
    printf("onConnection(): tid=%d new connection [%s] is down\n",
           CurrentThread::Tid(),
           conn->GetName().c_str());
  }
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp receiveTime) {
  printf("onMessage(): tid=%d received %zd bytes from connection [%s] at [%s]\n",
         CurrentThread::Tid(),
         buf->GetReadableBytes(),
         conn->GetName().c_str(),\
         receiveTime.ToFormattedString().c_str());
  //conn->Send(buf->RetrieveAsString());
  printf("onMessage():[%s]\n", buf->RetrieveAsString().c_str());
  //buf->RetrieveAll();
}


int main(int argc, char* argv[]) {
  printf("main():pid = %d, tid=%d \n", getpid(), CurrentThread::Tid());


  InetAddress listen_addr("127.0.0.1", 12820);
  EventLoop loop;
  TcpClient client(&loop, listen_addr);

  client.SetConnectionCallback(onConnection);
  client.SetMessageCallback(onMessage);
  client.EnableRetry();
  client.Connect();

  loop.Loop();

  printf("exit main().\n");
}
