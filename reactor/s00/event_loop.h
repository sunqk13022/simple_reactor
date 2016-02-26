#ifndef SIMPLE_REACTOR_S00_EVENT_LOOP_H_
#define SIMPLE_REACTOR_S00_EVENT_LOOP_H_

#include <thread/thread.h>

namespace simple_reactor {

class EventLoop {
 public:
  EventLoop();
  ~EventLoop();

  void Loop();
  void AssertInloopthread();

  bool IsInLoopthread() const;

 private:
  void AbortNotInloopthread();

  bool        loop_;
  const pid_t threadid_;
}; // class EventLoop

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_S00_EVENT_LOOP_H_
