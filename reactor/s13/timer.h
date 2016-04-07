#ifndef SIMPLE_REACTOR_REACTOR_S02_TIMER_H_
#define SIMPLE_REACTOR_REACTOR_S02_TIMER_H_

#include "time/timestamp.h"
#include "callback.h"
#include <thread/atomic.h>

namespace simple_reactor {

class Timer {
 public:

  Timer(const TimerCallback& cb, Timestamp when, double interval)
    : callback_(cb),
    expiration_(when),
    interval_(interval),
    repeat_(interval > 0),
    sequence_(num_created_.incrementAndGet())
  {
  }

  void Run() const;
  void Restart(Timestamp now);

  Timestamp Expiration() const;
  bool Repeat() const;

  int64_t GetSequence() const;

 private:

  const TimerCallback callback_;
  Timestamp           expiration_;
  const double        interval_;
  const bool          repeat_;

  const int64_t       sequence_;
  static AtomicInt64  num_created_;
}; // class Timer


} // namespace simple_reactor

#endif // SIMPLE_REACTOR_REACTOR_S02_TIMER_H_
