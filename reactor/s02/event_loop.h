#ifndef SIMPLE_REACTOR_S00_EVENT_LOOP_H_
#define SIMPLE_REACTOR_S00_EVENT_LOOP_H_

#include <thread/thread.h>

#include <boost/scoped_ptr.hpp>
#include <vector>

#include "callback.h"

namespace simple_reactor {

class Channel;
class Poller;
class TimerQueue;

class EventLoop {
 public:
  EventLoop();
  ~EventLoop();

  void Loop();
  void Quit();

  void AssertInloopthread();

  bool IsInLoopthread() const;

  void UpdateChannel(Channel* ch);

  void RunAfter(double delay, const TimerCallback& cb);
  void RunEvery(double interval, const TimerCallback& cb);

 private:
  void AbortNotInloopthread();

  bool                           loop_;
  bool                           quit_;
  const pid_t                    threadid_;
  boost::scoped_ptr<Poller>      poller_;
  boost::scoped_ptr<TimerQueue>  timer_queue_;
  std::vector<Channel*>          active_channel_;
}; // class EventLoop

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_S00_EVENT_LOOP_H_
