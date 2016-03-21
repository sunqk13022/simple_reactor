#include "event_loop_thread.h"

#include <boost/bind.hpp>
#include "event_loop.h"

namespace simple_reactor {

EventLoopThread::EventLoopThread()
  : loop_(NULL),
  existing_(false),
  thread_(boost::bind(&EventLoopThread::ThreadFunc, this)),
  mutex_(),
  condition_(mutex_)
{
}

EventLoopThread::~EventLoopThread() {
  existing_ = true;
  loop_->Quit();
  thread_.Join();
}

EventLoop* EventLoopThread::StartLoop() {
  thread_.Start();

  {
    MutexLock lock(mutex_);
    while (loop_ == NULL) {
      condition_.Wait();
    }
  }
  return loop_;
}

void EventLoopThread::ThreadFunc() {
  EventLoop loop;

  {
    MutexLock lock(mutex_);
    loop_ = &loop;
    condition_.Notify();
  }
  loop.Loop();
}

} // namespace simple_reactor
