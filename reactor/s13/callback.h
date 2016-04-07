#ifndef SIMPLE_REACTOR_REACTOR_SO2_CALLBACK_H_
#define SIMPLE_REACTOR_REACTOR_SO2_CALLBACK_H_

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "time/timestamp.h"

namespace simple_reactor {

typedef boost::function<void ()> TimerCallback;

class TcpConnection;
class Buffer;
typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef boost::function<void (const TcpConnectionPtr&,
                              Buffer* buf,
                              Timestamp)> MessageCallback;

typedef boost::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef boost::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_REACTOR_SO2_CALLBACK_H_
