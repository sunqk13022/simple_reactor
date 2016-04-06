#ifndef SIMPLE_REACTOR_S10_EVENTLOOPTHREAD_POOL_H_
#define SIMPLE_REACTOR_S10_EVENTLOOPTHREAD_POOL_H_

#include <thread/thread.h>
#include <thread/mutex.h>
#include <thread/condition.h>

#include <vector>
#include <boost/function.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace simple_reactor {

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool {
 public:

  explicit EventLoopThreadPool(EventLoop* baseLoop);
  ~EventLoopThreadPool();

  void SetThreadNum(int num);
  void Start();
  EventLoop* GetNextLoop();

 private:

  EventLoop*   base_loop_;
  bool         started_;
  int          num_threads_;
  int          next_;
  boost::ptr_vector<EventLoopThread> threads_;
  std::vector<EventLoop*> loops_;
}; // class EventLoopThreadPool

}

#endif // SIMPLE_REACTOR_S10_EVENTLOOPTHREAD_POOL_H_
