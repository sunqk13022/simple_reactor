#ifndef SIMPLE_REACTOR_REACTOR_S11_TIMER_ID_H_
#define SIMPLE_REACTOR_REACTOR_S11_TIMER_ID_H_


namespace simple_reactor {

class Timer;

class TimerId {
 public:
  TimerId(Timer* timer = NULL, int64_t seq = 0)
    : timer_(timer),
      seq_(seq)
  {
  }

  friend class TimerQueue;

 private:
  Timer*   timer_;
  int64_t  seq_;
}; // class TimerId


} // namespace simple_reactor

#endif // SIMPLE_REACTOR_REACTOR_S11_TIMER_ID_H_
