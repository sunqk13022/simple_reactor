#include "event_loop.h"

#include <poll.h>
#include <boost/bind.hpp>
#include <sys/eventfd.h>
#include <signal.h>

#include <logging/logging.h>

//#include "poller.h"
#include "epoller.h"
#include "channel.h"
#include "timer_queue.h"

namespace simple_reactor {

__thread EventLoop* loop_thread_ptr = 0;

static int createEventfd() {
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0) {
    LOG_FATAL << "Failed innnn eventfd";
    abort();
  }
  return evtfd;
}

class IgnoreSigPipe {
 public:
  IgnoreSigPipe() {
    ::signal(SIGPIPE, SIG_IGN);
  }
}; // class IgnoreSigPipe

IgnoreSigPipe init_obj;

EventLoop::EventLoop()
  : loop_(false),
    quit_(false),
    call_pandingfunctors_(false),
    threadid_(CurrentThread::Tid()),
    poller_(new EPoller(this)),
    timer_queue_(new TimerQueue(this)),
    wakefd_(createEventfd()),
    wakeup_channel_(new Channel(this, wakefd_))
{
  LOG_INFO << "EventLoop create " << this << " in thread " << threadid_;
  if (loop_thread_ptr) {
    LOG_FATAL << "Another EventLoop " << loop_thread_ptr
      << " exists in this thread " << threadid_;
  } else {
    loop_thread_ptr = this;
  }
  wakeup_channel_->SetReadCallback(
    boost::bind(&EventLoop::HandleRead, this));
  wakeup_channel_->EnableReading();
}

EventLoop::~EventLoop() {
  loop_thread_ptr = 0;
  ::close(wakefd_);
}

void EventLoop::Loop() {
  AssertInloopthread();
  loop_ = true;
  quit_ = false;
  while (!quit_) {
    active_channel_.clear();
    //::poll(NULL, 0, 5*1000);
    poll_returntime_ = poller_->Poll(10000, &active_channel_);
    for (std::vector<Channel*>::iterator it = active_channel_.begin();
         it != active_channel_.end(); ++it) {
      (*it)->HandleEvent(poll_returntime_);
    }
    DoPendingFunctors();
  }

  LOG_INFO << "EventLoop " << this << " stop looping";
  loop_ = false;
}

void EventLoop::Quit() {
  quit_ = true;
  if (!IsInLoopthread()) {
    Wakeup();
  }
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

void EventLoop::RemoveChannel(Channel* ch) {
  AssertInloopthread();
  poller_->RemoveChannel(ch);
}

TimerId EventLoop::RunAfter(double delay, const TimerCallback& cb) {
  Timestamp t(Timestamp::Now());
  t.AddSeconds(delay);
  return timer_queue_->AddTimer(cb, t, 0.0);
}

TimerId EventLoop::RunEvery(double interval, const TimerCallback& cb) {
  Timestamp t(Timestamp::Now());
  t.AddSeconds(interval);
  return timer_queue_->AddTimer(cb, t, interval);
}

void EventLoop::RunInLoop(const Functor& cb) {
  if (IsInLoopthread()) {
    cb();
  } else {
    QueueInLoop(cb);
  }
}

void EventLoop::QueueInLoop(const Functor& cb) {
  {
    MutexLock lock(mutex_);
    pending_functors_.push_back(cb);
  }
  if (!IsInLoopthread() || call_pandingfunctors_) {
    Wakeup();
  }
}

void EventLoop::Wakeup() {
  uint64_t one = 1;
  ssize_t n = ::write(wakefd_, &one, sizeof(one));
  if (n != sizeof(one)) {
    LOG_ERROR << "EventLoop::Wakeup() writes " << n << " bytes instead of 8";
  }
}

void EventLoop::Cancel(TimerId timerId) {
  timer_queue_->Cancel(timerId);
}

void EventLoop::HandleRead() {
  uint64_t one = 1;
  ssize_t n = ::read(wakefd_, &one, sizeof(one));
  if (n != sizeof(one)) {
    LOG_ERROR << "EventLoop::HandleRead() writes " << n << " bytes instead of 8";
  }
}

void EventLoop::DoPendingFunctors() {
  std::vector<Functor> functors;
  call_pandingfunctors_ = true;

  {
    MutexLock lock(mutex_);
    functors.swap(pending_functors_);
  }

  for (size_t i = 0; i < functors.size(); ++i) {
    functors[i]();
  }

  call_pandingfunctors_ = false;
}

} // namespace simple_reactor
