#include "event_loop_thread_pool.h"

#include "event_loop.h"
#include "event_loop_thread.h"

#include <boost/bind.hpp>

namespace simple_reactor {

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)
  : base_loop_(baseLoop),
  started_(false),
  num_threads_(0),
  next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool() {
}

void EventLoopThreadPool::SetThreadNum(int num) {
  num_threads_ = num;
}

void EventLoopThreadPool::Start() {
  base_loop_->AssertInloopthread();

  started_ = true;
  for (int i = 0; i < num_threads_; ++i) {
    EventLoopThread* t = new EventLoopThread();
    threads_.push_back(t);

    loops_.push_back(t->StartLoop());
  }
}

EventLoop* EventLoopThreadPool::GetNextLoop() {
  base_loop_->AssertInloopthread();

  EventLoop* loop = base_loop_;
  if (!loops_.empty()) {
    loop = loops_[next_];
    ++next_;

    if ((size_t)(next_) >= loops_.size()) {
      next_ = 0;
    }
  }

  return loop;
}

} // namespace simple_reactor
