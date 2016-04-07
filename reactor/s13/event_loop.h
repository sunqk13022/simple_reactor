#ifndef SIMPLE_REACTOR_S00_EVENT_LOOP_H_
#define SIMPLE_REACTOR_S00_EVENT_LOOP_H_

#include <thread/thread.h>
#include <thread/mutex.h>

#include <boost/scoped_ptr.hpp>
#include <vector>

#include "callback.h"
#include "timer_id.h"

namespace simple_reactor {

class Channel;
//class Poller;
class EPoller;
class TimerQueue;

class EventLoop {
 public:

  typedef boost::function<void ()> Functor;
  EventLoop();
  ~EventLoop();

  void Loop();
  void Quit();

  void AssertInloopthread();

  bool IsInLoopthread() const;

  void UpdateChannel(Channel* ch);
  void RemoveChannel(Channel* ch);

  TimerId RunAfter(double delay, const TimerCallback& cb);
  TimerId RunEvery(double interval, const TimerCallback& cb);

  void RunInLoop(const Functor& cb);
  void QueueInLoop(const Functor& cb);

  void Wakeup();
  void Cancel(TimerId timerId);

 private:
  void AbortNotInloopthread();

  void HandleRead();
  void DoPendingFunctors();

  bool                           loop_;
  bool                           quit_;
  bool                           call_pandingfunctors_;
  const pid_t                    threadid_;
  Timestamp                      poll_returntime_;
  boost::scoped_ptr<EPoller>      poller_;
  boost::scoped_ptr<TimerQueue>  timer_queue_;
  int                            wakefd_;
  boost::scoped_ptr<Channel>     wakeup_channel_;
  std::vector<Channel*>          active_channel_;
  Mutex                          mutex_;
  std::vector<Functor>           pending_functors_;
}; // class EventLoop

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_S00_EVENT_LOOP_H_
