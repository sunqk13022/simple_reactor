#include "thread.h"

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <boost/weak_ptr.hpp>

namespace thread {

__thread pid_t t_cachedTid = 0;

pid_t gettid() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}

namespace CurrentThread {

__thread const char* t_threadName = "unknown";

pid_t Tid() {
  if (t_cachedTid == 0) {
    t_cachedTid = gettid();
  }
  return t_cachedTid;
}

const char* Name() {
  return t_threadName;
}

bool IsMainThread() {
  return Tid() == ::getpid();
}
} // namespace CurrentThread

void afterFork() {
  t_cachedTid = gettid();
  CurrentThread::t_threadName = "main";
}

class ThreadNameInitializer {
 public:
  ThreadNameInitializer() {
    CurrentThread::t_threadName = "main";
    pthread_atfork(NULL, NULL, &afterFork);
  }
}; // class ThreadNameInitializer

ThreadNameInitializer init;

struct ThreadData {
  typedef Thread::ThreadFunc ThreadFunc;
  ThreadFunc               func_;
  std::string              name_;
  boost::weak_ptr<pid_t>   wk_tid_;

  ThreadData(const ThreadFunc& f, const std::string& name, const boost::shared_ptr<pid_t>& tid)
    : func_(f),
      name_(name),
      wk_tid_(tid)
  {
  }

  void RunInThread() {
    pid_t tid = CurrentThread::Tid();
    boost::shared_ptr<pid_t> ptid = wk_tid_.lock();
    if (ptid) {
      *ptid = tid;
      ptid.reset();
    }

    CurrentThread::t_threadName = name_.c_str();
    func_();
    CurrentThread::t_threadName = "finished";
  }
}; // struct ThreadData

void* startThread(void* obj) {
  ThreadData* data = static_cast<ThreadData*>(obj);
  data->RunInThread();
  delete data;
  return NULL;
}

AtomicInt32 Thread::num_created_;

Thread::Thread(const ThreadFunc& f, const std::string& name)
  : started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(new pid_t(0)),
    func_(f),
    name_(name)
{
  num_created_.increment();
}

Thread::~Thread() {
  if (started_ && !joined_) {
    pthread_detach(pthreadId_);
  }
}

void Thread::Start() {
  assert(!started_);
  started_ = true;

  ThreadData* data = new ThreadData(func_, name_, tid_);
  if (pthread_create(&pthreadId_, NULL, &startThread, data)) {
    started_ = false;
    delete data;
    abort();
  }
}
void Thread::Join() {
  assert(started_);
  assert(!joined_);
  joined_ = true;
  pthread_join(pthreadId_, NULL);
}

} // namespace thread
