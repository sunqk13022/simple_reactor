#ifndef SIMPLE_REACTOR_REACTOR_S05_TCP_CONNECTION_H_
#define SIMPLE_REACTOR_REACTOR_S05_TCP_CONNECTION_H_

#include "callback.h"
#include "inet_address.h"

#include <boost/any.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "buffer.h"

namespace simple_reactor {

class Channel;
class EventLoop;
class Socket;

class TcpConnection: public boost::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(EventLoop* loop,
                const std::string& name,
                int sockfd,
                const InetAddress& localaddr,
                const InetAddress& peeraddr);
  ~TcpConnection();

  EventLoop* GetLoop() const;
  const std::string& GetName() const;
  const InetAddress& GetLocalAddr();
  const InetAddress& GetPeerAddr();
  bool  GetConnected() const { return state_ == kConnected; }

  void Send(const std::string& message);
  void Shutdown();

  void SetConnectionCallback(const ConnectionCallback& cb);
  void SetMessageCallback(const MessageCallback& cb);
  void SetCloseCallback(const CloseCallback& cb);

  void ConnectionEstablished();
  void ConnectionDestroyed();

 private:
  enum StateE { kConnecting, kConnected, kDisConnecting, kDisConnected};

  void SetState(StateE s) { state_ = s; }

  void HandleRead(Timestamp receiveTime);
  void HandleWrite();
  void HandleClose();
  void HandleError();
  void SendInLoop(const std::string& message);
  void ShutdownInLoop();

  EventLoop*                 loop_;
  std::string                name_;
  StateE                     state_;
  boost::scoped_ptr<Socket>  socket_;
  boost::scoped_ptr<Channel> channel_;
  InetAddress                local_addr_;
  InetAddress                peer_addr_;
  ConnectionCallback         connection_callback_;
  MessageCallback            message_callback_;
  CloseCallback              close_callback_;
  Buffer                     input_buffer_;
  Buffer                     output_buffer_;
}; // class TcpConnection

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_REACTOR_S05_TCP_CONNECTION_H_
