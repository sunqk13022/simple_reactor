#ifndef SIMPLE_REACTOR_REACTOR_S02_TIMERQUEUE_H_
#define SIMPLE_REACTOR_REACTOR_S02_TIMERQUEUE_H_

#include <set>
#include <vector>

#include <time/timestamp.h>
#include <thread/mutex.h>

#include "callback.h"
#include "channel.h"
#include "timer_id.h"

namespace simple_reactor {

class EventLoop;
class Timer;

class TimerQueue {
 public:

  TimerQueue(EventLoop* loop);
  ~TimerQueue();

  TimerId AddTimer(const TimerCallback& cb, Timestamp when, double interval);
  void Cancel(TimerId timerId);

 private:
  typedef std::pair<Timestamp, Timer*> Entry;
  typedef std::set<Entry>              TimerList;
  typedef std::pair<Timer*, int64_t>   ActiveTimer;
  typedef std::set<ActiveTimer>        ActiveTimerSet;

  void HandleRead();

  std::vector<Entry> GetExpired(Timestamp now);

  void Reset(const std::vector<Entry>& expired, Timestamp now);

  bool Insert(Timer* timer);

  void AddTimerInLoop(Timer* timer);
  void CancelInLoop(TimerId timerId);

 private:

  EventLoop*         loop_;
  const int          timerfd_;
  Channel            timerfd_channel_;
  std::set<Entry>    timers_;

  // for cancel
  bool               calling_expired_timers_;
  ActiveTimerSet     active_timers_;
  ActiveTimerSet     canceling_timers_;
}; // class TimerQueue

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_REACTOR_S02_TIMERQUEUE_H_
