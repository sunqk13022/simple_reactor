#define __STDC_LIMIT_MACROS
#include "timer_queue.h"

#include <sys/timerfd.h>
#include <boost/bind.hpp>

#include <logging/logging.h>

#include "event_loop.h"
#include "timer.h"

#include <stdint.h>


namespace simple_reactor {

int create_timerfd() {
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                 TFD_NONBLOCK | TFD_CLOEXEC);
  if (timerfd < 0) {
    LOG_FATAL << "Failed in timerfd_create";
  }
  return timerfd;
}

struct timespec time_from_now(Timestamp when) {
  int64_t ns = when.Ns() - Timestamp::Now().Ns();
  if (ns < 100) {
    ns = 100;
  }

  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(ns/1000000);
  ts.tv_nsec= static_cast<long>((ns%1000000) * 1000);

  return ts;
}

void reset_timerfd(int timerfd, Timestamp expiration) {
  struct itimerspec newValue;
  struct itimerspec oldValue;
  bzero(&newValue, sizeof(newValue));
  bzero(&oldValue, sizeof(oldValue));
  newValue.it_value = time_from_now(expiration);
  int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
  if (ret) {
    LOG_ERROR << "timerfd_settime()";
  }
}

void read_timerfd(int timerfd, Timestamp now) {
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
  LOG_INFO << "TimerQueue::HandleRead*( " << howmany << " at " << now.ToFormattedString();
  if (n != sizeof(howmany)) {
    LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
  }
}

TimerQueue::TimerQueue(EventLoop* loop)
  : loop_(loop),
    timerfd_(create_timerfd()),
    timerfd_channel_(loop, timerfd_),
    timers_()
{
  timerfd_channel_.SetReadCallback(boost::bind(&TimerQueue::HandleRead, this));
  timerfd_channel_.EnableReading();
}

TimerQueue::~TimerQueue() {
  ::close(timerfd_);
  for (std::set<Entry>::iterator it = timers_.begin(); it != timers_.end(); ++it) {
    delete it->second;
  }
}

void TimerQueue::AddTimer(const TimerCallback& cb, Timestamp when, double interval) {
  Timer* timer = new Timer(cb, when, interval);
  loop_->RunInLoop(
    boost::bind(&TimerQueue::AddTimerInLoop, this, timer));
}

void TimerQueue::AddTimerInLoop(Timer* timer) {
  loop_->AssertInloopthread();
  bool earlist_changed = Insert(timer);
  if (earlist_changed) {
    reset_timerfd(timerfd_, timer->Expiration());
  }
}

void TimerQueue::HandleRead() {
  loop_->AssertInloopthread();

  Timestamp now(Timestamp::Now());
  read_timerfd(timerfd_, now);

  std::vector<Entry> expired = GetExpired(now);

  for (std::vector<Entry>::iterator it = expired.begin();
       it != expired.end(); ++it) {
    it->second->Run();
  }

  Reset(expired, now);
}

bool TimerQueue::Insert(Timer* timer) {
  bool earlist_changed = false;
  Timestamp when = timer->Expiration();
  std::set<Entry>::iterator it = timers_.begin();
  if (it == timers_.end() || when < it->first) {
    earlist_changed = true;
  }
  timers_.insert(std::make_pair(when, timer));
  return earlist_changed;
}

std::vector<TimerQueue::Entry> TimerQueue::GetExpired(Timestamp now) {
  std::vector<Entry> expired;

  Entry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
  std::set<Entry>::iterator it = timers_.lower_bound(sentry);

  std::copy(timers_.begin(), it, back_inserter(expired));
  timers_.erase(timers_.begin(), it);

  return expired;
}

void TimerQueue::Reset(const std::vector<Entry>& expired, Timestamp now) {
  Timestamp nextExpire;

  for (std::vector<Entry>::const_iterator it = expired.begin();
       it != expired.end(); ++it) {
    if (it->second->Repeat()) {
      it->second->Restart(now);
      Insert(it->second);
    } else {
      delete it->second;
    }
  }

  if (!timers_.empty()) {
    nextExpire = timers_.begin()->second->Expiration();
  }
  if (nextExpire.Valid()) {
    reset_timerfd(timerfd_, nextExpire);
  }
}

} // namespace simple_reactor
