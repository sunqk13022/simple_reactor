#include "timer.h"

namespace simple_reactor {


void Timer::Run() const {
  callback_();
}

Timestamp Timer::Expiration() const {
  return expiration_;
}

bool Timer::Repeat() const {
  return repeat_;
}

void Timer::Restart(Timestamp now) {
  now.AddSeconds(interval_);
  expiration_ = now;
}

} // namespace simple_reactor
