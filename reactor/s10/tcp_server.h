#ifndef SIMPLE_REACTOR_REACTOR_S05_TCP_SERVER_H_
#define SIMPLE_REACTOR_REACTOR_S05_TCP_SERVER_H_

#include <map>
#include <boost/scoped_ptr.hpp>

#include "callback.h"
#include "tcp_connection.h"

namespace simple_reactor {

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer {
 public:
  TcpServer(EventLoop* loop, const InetAddress& listen_addr);

  ~TcpServer();

  void Start();
  void SetThreadNum(int num);

  void SetConnectionCallback(const ConnectionCallback& cb);
  void SetMessageCallback(const MessageCallback& cb);
  void SetWriteCompleteCallback(const WriteCompleteCallback& cb);

 private:

  void HandleNewConnection(int sockfd, const InetAddress& peer_addr);
  void RemoveConnection(const TcpConnectionPtr& conn);
  void RemoveConnectionInLoop(const TcpConnectionPtr& conn);

  typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

  EventLoop*                   loop_;
  const std::string            name_;
  boost::scoped_ptr<Acceptor>  acceptor_;
  boost::scoped_ptr<EventLoopThreadPool>  thread_pool_;
  ConnectionCallback           connection_callback_;
  MessageCallback              message_callback_;
  WriteCompleteCallback        write_complete_callback_;
  bool                         started_;
  int                          next_conn_id_;
  ConnectionMap                connections_;
}; // class TcpServer

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_REACTOR_S05_TCP_SERVER_H_
