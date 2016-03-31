#include "tcp_server.h"

#include <stdio.h>
#include <boost/bind.hpp>
#include <logging/logging.h>

#include "acceptor.h"
#include "event_loop.h"
#include "socket_fun.h"


namespace simple_reactor {

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listen_addr)
: loop_(loop),
  name_(listen_addr.ToHostPort()),
  acceptor_(new Acceptor(loop, listen_addr)),
  started_(false),
  next_conn_id_(1)
{
  acceptor_->SetNewConnectionCallback(
    boost::bind(&TcpServer::HandleNewConnection, this, _1, _2));
}

TcpServer::~TcpServer() {
}

void TcpServer::Start() {
  if (!started_) {
    started_ = true;
  }

  if (!acceptor_->IsListenning()) {
    loop_->RunInLoop(
      boost::bind(&Acceptor::Listen, get_pointer(acceptor_)));
  }
}

void TcpServer::SetConnectionCallback(const ConnectionCallback& cb) {
  connection_callback_ = cb;
}

void TcpServer::SetMessageCallback(const MessageCallback& cb) {
  message_callback_ = cb;
}

void TcpServer::SetWriteCompleteCallback(const WriteCompleteCallback& cb) {
  write_complete_callback_ = cb;
}

void TcpServer::HandleNewConnection(int sockfd, const InetAddress& peer_addr) {
  loop_->AssertInloopthread();
  char buf[32];
  snprintf(buf, sizeof(buf), "#%d", next_conn_id_);
  ++next_conn_id_;

  std::string conn_name = name_ + buf;

  LOG_INFO << "TcpServer::HandleNewConnection [" << name_
    << "] - New Connection [" << conn_name
    << "] from " << peer_addr.ToHostPort();
  InetAddress local_addr(socket_fun::GetLocalAddr(sockfd));

  TcpConnectionPtr conn(new TcpConnection(loop_, conn_name, sockfd, local_addr, peer_addr));
  connections_[conn_name] = conn;
  conn->SetConnectionCallback(connection_callback_);
  conn->SetMessageCallback(message_callback_);
  conn->SetCloseCallback(
    boost::bind(&TcpServer::RemoveConnection, this, _1));
  conn->ConnectionEstablished();
}

void TcpServer::RemoveConnection(const TcpConnectionPtr& conn) {
  loop_->AssertInloopthread();
  LOG_INFO << "TcpServer::RemoveConnection [" << name_
    << "] - connection " << conn->GetName();
  connections_.erase(conn->GetName());
  loop_->QueueInLoop(
    boost::bind(&TcpConnection::ConnectionDestroyed, conn));
}

} // namespace simple_reactor
