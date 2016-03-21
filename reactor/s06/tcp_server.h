#ifndef SIMPLE_REACTOR_REACTOR_S05_TCP_SERVER_H_
#define SIMPLE_REACTOR_REACTOR_S05_TCP_SERVER_H_

#include <map>
#include <boost/scoped_ptr.hpp>

#include "callback.h"
#include "tcp_connection.h"

namespace simple_reactor {

class Acceptor;
class EventLoop;

class TcpServer {
 public:
  TcpServer(EventLoop* loop, const InetAddress& listen_addr);

  ~TcpServer();

  void Start();

  void SetConnectionCallback(const ConnectionCallback& cb);
  void SetMessageCallback(const MessageCallback& cb);

 private:

  void HandleNewConnection(int sockfd, const InetAddress& peer_addr);
  void RemoveConnection(const TcpConnectionPtr& conn);

  typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

  EventLoop*                   loop_;
  const std::string            name_;
  boost::scoped_ptr<Acceptor>  acceptor_;
  ConnectionCallback           connection_callback_;
  MessageCallback              message_callback_;
  bool                         started_;
  int                          next_conn_id_;
  ConnectionMap                connections_;
}; // class TcpServer

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_REACTOR_S05_TCP_SERVER_H_
