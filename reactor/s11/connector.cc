#include "connector.h"

#include <boost/bind.hpp>
#include <stdio.h>
#include <errno.h>

#include <logging/logging.h>

#include "channel.h"
#include "event_loop.h"
#include "socket.h"
#include "socket_fun.h"

namespace simple_reactor {

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
  : loop_(loop),
  server_addr_(serverAddr),
  connected_(false),
  state_(kDisConnected),
  retry_dlay_ms_(kInitRetryDelayMs)
{
  LOG_DEBUG << "Connector::Connector [" << this << "]";
}

Connector::~Connector() {
  LOG_DEBUG << "Connector::~Connector [" << this << "]";
  loop_->Cancel(timer_id_);
}

void Connector::SetNewConnectionCallback(const NewConnectionCallback& cb) {
  new_connection_callback_ = cb;
}

void Connector::Start() {
  connected_ = true;
  loop_->RunInLoop(
    boost::bind(&Connector::StartInLoop, this));
}

void Connector::ReStart() {
  loop_->AssertInloopthread();
  SetState(kDisConnected);

  retry_dlay_ms_ = kInitRetryDelayMs;
  connected_ = true;
  StartInLoop();
}

void Connector::Stop() {
  connected_ = false;
  loop_->Cancel(timer_id_);
}

const InetAddress& Connector::GetServerAddress() const {
  return server_addr_;
}

void Connector::SetState(States s) {
  state_ = s;
}

void Connector::StartInLoop() {
  loop_->AssertInloopthread();
  if (connected_) {
    Connect();
  } else {
    LOG_DEBUG << "Do not Connect";
  }
}

void Connector::Connect() {
  int sockfd = socket_fun::CreateSocketNonblocking();
  int ret = socket_fun::Connect(sockfd, server_addr_.GetSockAddrInet());
  int saved_errno = (ret == 0) ? 0 : errno;
  switch (saved_errno) {
  case 0:
  case EINPROGRESS:
  case EINTR:
  case EISCONN:
    Connecting(sockfd);
    break;
  case EAGAIN:
  case EADDRINUSE:
  case EADDRNOTAVAIL:
  case ECONNREFUSED:
  case ENETUNREACH:
    Retry(sockfd);
    break;
  case EACCES:
  case EPERM:
  case EAFNOSUPPORT:
  case EALREADY:
  case EBADF:
  case EFAULT:
  case ENOTSOCK:
    LOG_ERROR << "Connect error in Connector::StartInLoop " << saved_errno;
    socket_fun::Close(sockfd);
    break;
  default:
    LOG_ERROR << "Unexpected error in Connector::StartInLoop " << saved_errno;
    socket_fun::Close(sockfd);
    break;
  }
}

void Connector::Connecting(int sockfd) {
  SetState(kConnecting);
  channel_.reset(new Channel(loop_, sockfd));
  channel_->SetWriteCallback(
    boost::bind(&Connector::HandleWrite, this));
  channel_->SetErrorCallback(
    boost::bind(&Connector::HandleError, this));

  channel_->EnableWriting();
}

void Connector::HandleWrite() {
  if (state_ != kConnecting) {
    return;
  }
  int sockfd = RemoveAndResetCancel();
  int err = socket_fun::GetSocketError(sockfd);
  if (err) {
    LOG_WARN << "Connector::HandleWrite - SO_ERROR = " << err;
    Retry(sockfd);
  } else {
    SetState(kConnected);
    if (connected_) {
      new_connection_callback_(sockfd);
    } else {
      socket_fun::Close(sockfd);
    }
  }
}

void Connector::HandleError() {
  int sockfd = RemoveAndResetCancel();
  int err = socket_fun::GetSocketError(sockfd);
  LOG_ERROR << "ERR=" << err;
  Retry(sockfd);
}

void Connector::Retry(int sockfd) {
  socket_fun::Close(sockfd);
  SetState(kDisConnected);
  if (connected_) {
    LOG_INFO << "Connector::Retry - Retry Connecting to "
      << server_addr_.ToHostPort() << " in "
      << retry_dlay_ms_ << " milliseconds. ";
    timer_id_ = loop_->RunAfter(retry_dlay_ms_/1000.0,
                                boost::bind(&Connector::StartInLoop, this));
    retry_dlay_ms_ = std::min(retry_dlay_ms_*2, kMaxRetryDelayMs);
  }
}

int  Connector::RemoveAndResetCancel() {
  channel_->DisableAll();
  loop_->RemoveChannel(get_pointer(channel_));
  int sockfd = channel_->Fd();
  loop_->QueueInLoop(
    boost::bind(&Connector::ResetChannel, this));
  return sockfd;
}

void Connector::ResetChannel() {
  channel_.reset();
}

} // namespace simple_reactor
