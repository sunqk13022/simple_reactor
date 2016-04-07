#ifndef SIMPLE_REACTOR_REACTOR_S12_TCP_CLIENT_H_
#define SIMPLE_REACTOR_REACTOR_S12_TCP_CLIENT_H_

#include <map>
#include <boost/scoped_ptr.hpp>

#include <thread/mutex.h>

#include "callback.h"
#include "tcp_connection.h"
#include "connector.h"

namespace simple_reactor {


class TcpClient {
 public:
  TcpClient(EventLoop* loop, const InetAddress& listen_addr);

  ~TcpClient();

  void Connect();
  void DisConnect();
  void Stop();

  TcpConnectionPtr  GetConnection() const;

  void EnableRetry();


  void SetConnectionCallback(const ConnectionCallback& cb);
  void SetMessageCallback(const MessageCallback& cb);
  void SetWriteCompleteCallback(const WriteCompleteCallback& cb);

 private:

  void HandleNewConnection(int sockfd);
  void RemoveConnection(const TcpConnectionPtr& conn);

  EventLoop*                   loop_;
  ConnectorPtr                 connector_;
  ConnectionCallback           connection_callback_;
  MessageCallback              message_callback_;
  WriteCompleteCallback        write_complete_callback_;
  bool                         retry_;
  bool                         connected_;
  int                          next_conn_id_;
  mutable Mutex                mutex_;
  TcpConnectionPtr             connection_;
}; // class TcpClient

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_REACTOR_S12_TCP_CLIENT_H_
