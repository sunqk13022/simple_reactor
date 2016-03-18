#include "acceptor.h"

#include <boost/bind.hpp>
#include <logging/logging.h>

#include "event_loop.h"
#include "inet_address.h"
#include "socket_fun.h"

namespace simple_reactor {

Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr)
  : loop_(loop),
    accept_socket_(socket_fun::CreateSocketNonblocking()),
    accept_channel_(loop, accept_socket_.GetFd()),
    listenning_(false)
{
  accept_socket_.SetReuseAddr(true);
  accept_socket_.Bind(listenAddr);
  accept_channel_.SetReadCallback(
    boost::bind(&Acceptor::HandleRead, this));
}

void Acceptor::SetNewConnectionCallback(const NewConnectionCallback& cb) {
  new_connection_callback_ = cb;
}

bool Acceptor::IsListenning() const {
  return listenning_;
}

void Acceptor::Listen() {
  loop_->AssertInloopthread();
  listenning_ = true;
  accept_socket_.Listen();
  accept_channel_.EnableReading();
}

void Acceptor::HandleRead() {
  loop_->AssertInloopthread();
  InetAddress peerAddr(0);

  int connfd = accept_socket_.Accept(&peerAddr);
  if (connfd >= 0) {
    if (new_connection_callback_) {
      new_connection_callback_(connfd, peerAddr);
    } else {
      socket_fun::Close(connfd);
    }
  }
}

} // namespace simple_reactor
