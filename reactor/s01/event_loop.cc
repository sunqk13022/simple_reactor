#include "event_loop.h"

#include <poll.h>

#include <logging/logging.h>

#include "poller.h"
#include "channel.h"

namespace simple_reactor {

__thread EventLoop* loop_thread_ptr = 0;

EventLoop::EventLoop()
  : loop_(false),
    quit_(false),
    threadid_(CurrentThread::Tid()),
    poller_(new Poller(this))
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
  quit_ = false;
  while (!quit_) {
    active_channel_.clear();
    //::poll(NULL, 0, 5*1000);
    poller_->Poll(10000, &active_channel_);
    for (std::vector<Channel*>::iterator it = active_channel_.begin();
         it != active_channel_.end(); ++it) {
      (*it)->HandleEvent();
    }
  }

  LOG_INFO << "EventLoop " << this << " stop looping";
  loop_ = false;
}

void EventLoop::Quit() {
  quit_ = true;
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

void EventLoop::UpdateChannel(Channel* ch) {
  AssertInloopthread();
  poller_->UpdateChannel(ch);
}

} // namespace simple_reactor
