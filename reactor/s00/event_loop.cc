#include "event_loop.h"

#include <poll.h>

#include <logging/logging.h>

namespace simple_reactor {

__thread EventLoop* loop_thread_ptr = 0;

EventLoop::EventLoop()
  : loop_(false),
    threadid_(CurrentThread::Tid())
{
  LOG_INFO << "EventLoop create " << this << " in thread " << threadid_;
  if (loop_thread_ptr) {
    LOG_FATAL << "Another EventLoop " << loop_thread_ptr
      << " exists in this thread " << threadid_;
  } else {
    loop_thread_ptr = this;
  }
}

EventLoop::~EventLoop() {
  loop_thread_ptr = 0;
}

void EventLoop::Loop() {
  AssertInloopthread();
  loop_ = true;
  ::poll(NULL, 0, 5*1000);

  LOG_INFO << "EventLoop " << this << " stop looping";
  loop_ = false;
}

bool EventLoop::IsInLoopthread() const {
  return threadid_ == CurrentThread::Tid();
}

void EventLoop::AssertInloopthread() {
  if (!IsInLoopthread()) {
    AbortNotInloopthread();
  }
}

void EventLoop::AbortNotInloopthread() {
  LOG_FATAL << "EventLoop " << this << " was create in thread:" << threadid_
    << ", current threadid:" << CurrentThread::Tid();
}

} // namespace simple_reactor
