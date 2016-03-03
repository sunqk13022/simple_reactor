#ifndef SIMPLE_REACTOR_REACTOR_S02_TIMERQUEUE_H_
#define SIMPLE_REACTOR_REACTOR_S02_TIMERQUEUE_H_

#include <set>
#include <vector>

#include <time/timestamp.h>
#include <thread/mutex.h>

#include "callback.h"
#include "channel.h"

namespace simple_reactor {

class EventLoop;
class Timer;

class TimerQueue {
 public:

  TimerQueue(EventLoop* loop);
  ~TimerQueue();

  void AddTimer(const TimerCallback& cb, Timestamp when, double interval);

 private:
  typedef std::pair<Timestamp, Timer*> Entry;

  void HandleRead();

  std::vector<Entry> GetExpired(Timestamp now);

  void Reset(const std::vector<Entry>& expired, Timestamp now);

  bool Insert(Timer* timer);

 private:

  EventLoop*         loop_;
  const int          timerfd_;
  Channel            timerfd_channel_;
  std::set<Entry>    timers_;
}; // class TimerQueue

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_REACTOR_S02_TIMERQUEUE_H_
