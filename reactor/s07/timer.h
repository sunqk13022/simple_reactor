#ifndef SIMPLE_REACTOR_REACTOR_S02_TIMER_H_
#define SIMPLE_REACTOR_REACTOR_S02_TIMER_H_

#include "time/timestamp.h"
#include "callback.h"

namespace simple_reactor {

class Timer {
 public:

  Timer(const TimerCallback& cb, Timestamp when, double interval)
    : callback_(cb),
    expiration_(when),
    interval_(interval),
    repeat_(interval > 0)
  {
  }

  void Run() const;
  void Restart(Timestamp now);

  Timestamp Expiration() const;
  bool Repeat() const;

 private:

  const TimerCallback callback_;
  Timestamp           expiration_;
  const double        interval_;
  const bool          repeat_;
}; // class Timer


} // namespace simple_reactor

#endif // SIMPLE_REACTOR_REACTOR_S02_TIMER_H_
