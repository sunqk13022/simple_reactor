#include "timer.h"

namespace simple_reactor {

AtomicInt64 Timer::num_created_;

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

int64_t Timer::GetSequence() const {
  return sequence_;
}

} // namespace simple_reactor
