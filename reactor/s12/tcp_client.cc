#include "tcp_client.h"

#include <stdio.h>
#include <boost/bind.hpp>
#include <logging/logging.h>

#include "event_loop.h"
#include "socket_fun.h"

namespace simple_reactor {

namespace detail {
void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn) {
  loop->QueueInLoop(boost::bind(&TcpConnection::ConnectionDestroyed, conn));
}

} // namespace  detail

TcpClient::TcpClient(EventLoop* loop, const InetAddress& listen_addr)
  : loop_(loop),
  connector_(new Connector(loop, listen_addr)),
  retry_(false),
  connected_(true),
  next_conn_id_(1)
{
  connector_->SetNewConnectionCallback(
    boost::bind(&TcpClient::HandleNewConnection, this, _1));

  LOG_INFO << "TcpClient [" << this
    << "] - connector " << get_pointer(connector_);
}

TcpClient::~TcpClient() {
  LOG_INFO << "TcpClient discontruct [" << this
    << "] - connector " << get_pointer(connector_);

  TcpConnectionPtr conn;
  {
    MutexLock lock(mutex_);
    conn = connection_;
  }

  if (conn) {
    CloseCallback cb = boost::bind(&detail::removeConnection, loop_, _1);
    loop_->RunInLoop(
      boost::bind(&TcpConnection::SetCloseCallback, conn, cb));
  } else {
    connector_->Stop();
  }
}

void TcpClient::Connect() {
  LOG_INFO << "TcpClinet connect [" << this << "] - connection to "
    << connector_->GetServerAddress().ToHostPort();
  connected_ = true;
  connector_->Start();
}

void TcpClient::DisConnect() {
  connected_ = false;
  {
    MutexLock lock(mutex_);
    if (connection_) {
      connection_->Shutdown();
    }
  }
}

void TcpClient::Stop() {
  connected_ = false;
  connector_->Stop();
}

TcpConnectionPtr  TcpClient::GetConnection() const {
  MutexLock lock(mutex_);
  return connection_;
}

void TcpClient::EnableRetry() {
  retry_ = true;
}


void TcpClient::SetConnectionCallback(const ConnectionCallback& cb) {
  connection_callback_ = cb;
}

void TcpClient::SetMessageCallback(const MessageCallback& cb) {
  message_callback_ = cb;
}

void TcpClient::SetWriteCompleteCallback(const WriteCompleteCallback& cb) {
  write_complete_callback_ = cb;
}

void TcpClient::HandleNewConnection(int sockfd) {
  loop_->AssertInloopthread();
  InetAddress peerAddr(socket_fun::GetPeerAddr(sockfd));
  char buf[32];
  snprintf(buf, sizeof(buf), ":%s#%d", peerAddr.ToHostPort().c_str(), next_conn_id_);
  ++next_conn_id_;
  std::string conn_name = buf;

  InetAddress local_addr(socket_fun::GetLocalAddr(sockfd));
  TcpConnectionPtr conn(new TcpConnection(loop_,
                                          conn_name,
                                          sockfd,
                                          local_addr,
                                          peerAddr));
  conn->SetConnectionCallback(connection_callback_);
  conn->SetMessageCallback(message_callback_);
  conn->SetWriteCompleteCallback(write_complete_callback_);
  conn->SetCloseCallback(
    boost::bind(&TcpClient::RemoveConnection, this, _1));

  {
    MutexLock lock(mutex_);
    connection_ = conn;
  }

  conn->ConnectionEstablished();
}

void TcpClient::RemoveConnection(const TcpConnectionPtr& conn) {
  loop_->AssertInloopthread();
  {
    MutexLock lock(mutex_);
    connection_.reset();
  }

  loop_->QueueInLoop(
    boost::bind(&TcpConnection::ConnectionDestroyed, conn));
  if (retry_ && connected_) {
    LOG_INFO << "TcpClient connec [" << this << "] Reconnect to"
      << connector_->GetServerAddress().ToHostPort();
    connector_->ReStart();
  }
}

} // namespace simple_reactor
