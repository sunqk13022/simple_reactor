#ifndef SIMPLE_REACTOR_REACTOR_S11_CONNECTOR_H_
#define SIMPLE_REACTOR_REACTOR_S11_CONNECTOR_H_

#include "callback.h"
#include "inet_address.h"
#include "timer_id.h"

#include <boost/any.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>


namespace simple_reactor {

class Channel;
class EventLoop;

class Connector {
 public:
  typedef boost::function<void (int sockfd)> NewConnectionCallback;

  Connector(EventLoop* loop, const InetAddress& serverAddr);
  ~Connector();

  void SetNewConnectionCallback(const NewConnectionCallback& cb);

  void Start();
  void ReStart();
  void Stop();

  const InetAddress& GetServerAddress() const;

 private:
  enum States { kDisConnected, kConnecting, kConnected };
  static const int kMaxRetryDelayMs = 30 * 1000;
  static const int kInitRetryDelayMs = 500;

  void SetState(States s);
  void StartInLoop();
  void Connect();
  void Connecting(int sockfd);
  void HandleWrite();
  void HandleError();
  void Retry(int sockfd);
  int  RemoveAndResetCancel();
  void ResetChannel();

 private:
  EventLoop*                  loop_;
  InetAddress                 server_addr_;
  bool                        connected_;
  States                      state_;
  boost::scoped_ptr<Channel>  channel_;
  NewConnectionCallback       new_connection_callback_;
  int                         retry_dlay_ms_;
  TimerId                     timer_id_;
}; // class Connector

typedef boost::shared_ptr<Connector> ConnectorPtr;

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_REACTOR_S11_CONNECTOR_H_
