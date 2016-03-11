#ifndef SIMPLE_REACTOR_S03_EVENTLOOPTHREAD_H_
#define SIMPLE_REACTOR_S03_EVENTLOOPTHREAD_H_

#include <thread/thread.h>
#include <thread/mutex.h>
#include <thread/condition.h>

namespace simple_reactor {

class EventLoop;

class EventLoopThread {
 public:

  EventLoopThread();
  ~EventLoopThread();

  EventLoop* StartLoop();

 private:

  void ThreadFunc();

  EventLoop*   loop_;
  bool         existing_;
  Thread       thread_;
  Mutex        mutex_;
  Condition    condition_;
}; // class EventLoopThread

}

#endif // SIMPLE_REACTOR_S03_EVENTLOOPTHREAD_H_
