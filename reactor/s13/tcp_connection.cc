#include "tcp_connection.h"

#include <boost/bind.hpp>
#include <stdio.h>
#include <errno.h>

#include <logging/logging.h>

#include "channel.h"
#include "event_loop.h"
#include "socket.h"
#include "socket_fun.h"

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
    boost::bind(&TcpConnection::HandleRead, this, _1));
  channel_->SetWriteCallback(
    boost::bind(&TcpConnection::HandleWrite, this));
  channel_->SetCloseCallback(
    boost::bind(&TcpConnection::HandleClose, this));
  channel_->SetErrorCallback(
    boost::bind(&TcpConnection::HandleError, this));
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

void TcpConnection::SetCloseCallback(const CloseCallback& cb) {
  close_callback_ = cb;
}

void TcpConnection::ConnectionEstablished() {
  loop_->AssertInloopthread();
  SetState(kConnected);
  channel_->EnableReading();

  connection_callback_(shared_from_this());
}

void TcpConnection::ConnectionDestroyed() {
  loop_->AssertInloopthread();
  SetState(kDisConnected);
  channel_->DisableAll();
  connection_callback_(shared_from_this());

  loop_->RemoveChannel(get_pointer(channel_));
}

void TcpConnection::HandleRead(Timestamp receiveTime) {
  int errnum = 0;
  ssize_t n = input_buffer_.ReadFd(channel_->Fd(), &errnum);
  if (n > 0) {
    message_callback_(shared_from_this(), &input_buffer_, receiveTime);
  } else if (n == 0) {
    HandleClose();
  } else {
    LOG_ERROR << "TcpConnection::HandleRead";
    HandleError();
  }
}

void TcpConnection::HandleWrite() {
  loop_->AssertInloopthread();
  if (channel_->IsWriting()) {
    ssize_t n = ::write(channel_->Fd(),
                        output_buffer_.Peek(),
                        output_buffer_.GetReadableBytes());
    if (n > 0) {
      output_buffer_.Retrieve(n);
      if (output_buffer_.GetReadableBytes() == 0) {
        channel_->DisableWriting();
        if (write_complete_callback_) {
          loop_->QueueInLoop(
            boost::bind(write_complete_callback_, shared_from_this()));
        }

        if (state_ == kDisConnecting) {
          ShutdownInLoop();
        }
      }
    } else {
      LOG_FATAL << "TcpConnection::HandleWrite";
      abort();
    }
  }
}

void TcpConnection::HandleClose() {
  loop_->AssertInloopthread();
  if (state_ == kCloseing) {
    return;
  }
  SetState(kCloseing);
  LOG_INFO << "TcpConnection::HandleClose state = " << state_;
  channel_->DisableAll();
  close_callback_(shared_from_this());
}

void TcpConnection::HandleError() {
  int err = socket_fun::GetSocketError(channel_->Fd());
  LOG_ERROR << "TcpConnection::HandleError [" << name_
    << "] - SO_ERROR = " << err;
}

void TcpConnection::Send(const std::string& message) {
  if (state_ == kConnected) {
    if (loop_->IsInLoopthread()) {
      SendInLoop(message);
    } else {
      loop_->RunInLoop(
        boost::bind(&TcpConnection::SendInLoop, this, message));
    }
  }
}

void TcpConnection::Shutdown() {
  if (state_ == kConnected) {
    SetState(kDisConnecting);
    loop_->RunInLoop(
      boost::bind(&TcpConnection::ShutdownInLoop, this));
  }
}


void TcpConnection::SendInLoop(const std::string& message) {
  loop_->AssertInloopthread();
  ssize_t nwrote = 0;

  if (!channel_->IsWriting() && output_buffer_.GetReadableBytes() == 0) {
    nwrote = ::write(channel_->Fd(), message.c_str(), message.size());
    if (nwrote >= 0) {
      if ((size_t)nwrote < message.size()) {
        LOG_ERROR << "Going to write more data";
      } else if (write_complete_callback_) {
        loop_->QueueInLoop(
          boost::bind(write_complete_callback_, shared_from_this()));
      }
    } else {
      nwrote = 0;
      if (errno != EWOULDBLOCK) {
        LOG_FATAL << "TcpConnection::SendInLoop error!";
      }
    }
  }

  if ((size_t)(nwrote) < message.size()) {
    output_buffer_.Append(message.c_str() + nwrote, message.size() - nwrote);
    if (!channel_->IsWriting()) {
      channel_->EnableWriting();
    }
  }
}

void TcpConnection::ShutdownInLoop() {
  loop_->AssertInloopthread();
  if (!channel_->IsWriting()) {
    socket_->ShutdownWrite();
  }
}
void TcpConnection::SetTcpNoDelay(bool on) {
  socket_->SetTcpNoDelay(on);
}

void TcpConnection::SetWriteCompleteCallback(const WriteCompleteCallback& cb) {
  write_complete_callback_ = cb;
}

} // namespace simple_reactor
