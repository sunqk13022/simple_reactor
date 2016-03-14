#ifndef SIMPLE_REACTOR_REACTOR_S04_ACCEPTOR_H_
#define SIMPLE_REACTOR_REACTOR_S04_ACCEPTOR_H_

#include <boost/function.hpp>

#include "channel.h"
#include "socket.h"

namespace simple_reactor {

class EventLoop;
class InetAddress;

class Acceptor {
 public:
  typedef boost::function<void (int sockfd,
                                const InetAddress&)> NewConnectionCallback;
  Acceptor(EventLoop* loop, const InetAddress& listenAddr);

  void SetNewConnectionCallback(const NewConnectionCallback& cb);

  bool IsListenning() const;
  void Listen();

 private:
  void HandleRead();

  EventLoop*              loop_;
  Socket                  accept_socket_;
  Channel                 accept_channel_;
  NewConnectionCallback   new_connection_callback_;
  bool                    listenning_;
}; // class Acceptor

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_REACTOR_S04_ACCEPTOR_H_
