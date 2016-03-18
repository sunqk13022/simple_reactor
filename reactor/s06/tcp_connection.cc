#include "tcp_connection.h"

#include <boost/bind.hpp>
#include <stdio.h>
#include <errno.h>

#include <logging/logging.h>

#include "channel.h"
#include "event_loop.h"
#include "socket.h"

namespace simple_reactor {

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& name,
                             int sockfd,
                             const InetAddress& localaddr,
                             const InetAddress& peeraddr)
: loop_(loop),
  name_(name),
  state_(kConnecting),
  socket_(new Socket(sockfd)),
  channel_(new Channel(loop, sockfd)),
  local_addr_(localaddr),
  peer_addr_(peeraddr)
{
  LOG_DEBUG << "TcpConnection::TcpConnection[" << name_ << "] at " << this << " fd=" << sockfd;
  channel_->SetReadCallback(
    boost::bind(&TcpConnection::HandleRead, this));
}

TcpConnection::~TcpConnection() {
  LOG_DEBUG << "TcpConnection::~TcpConnection[" << name_ << "] at " << this << " fd=" << channel_->Fd();
}

EventLoop* TcpConnection::GetLoop() const {
  return loop_;
}

const std::string& TcpConnection::GetName() const {
  return name_;
}

const InetAddress& TcpConnection::GetLocalAddr() {
  return local_addr_;
}

const InetAddress& TcpConnection::GetPeerAddr() {
  return peer_addr_;
}

void TcpConnection::SetConnectionCallback(const ConnectionCallback& cb) {
  connection_callback_ = cb;
}

void TcpConnection::SetMessageCallback(const MessageCallback& cb) {
  message_callback_ = cb;
}

void TcpConnection::ConnectionEstablished() {
  loop_->AssertInloopthread();
  SetState(kConnected);
  channel_->EnableReading();

  connection_callback_(shared_from_this());
}

void TcpConnection::HandleRead() {
  char buf[65536];
  ssize_t n = ::read(channel_->Fd(), buf, sizeof(buf));
  message_callback_(shared_from_this(), buf, n);
}

} // namespace simple_reactor
