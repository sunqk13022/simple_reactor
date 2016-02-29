#ifndef THREAD_MUTEX_H_
#define THREAD_MUTEX_H_

#include <pthread.h>

#include "thread.h"

namespace simple_reactor {

class Mutex {
 public:

  Mutex() : holder_(0) {
    pthread_mutex_init(&mutex_, NULL);
  }

  ~Mutex() {
    pthread_mutex_destroy(&mutex_);
  }

  void Lock() {
    pthread_mutex_lock(&mutex_);
    holder_ = CurrentThread::Tid();
  }

  void Unlock() {
    holder_ = 0;
    pthread_mutex_unlock(&mutex_);
  }

 private:

  pthread_mutex_t mutex_;
  pid_t           holder_;
}; // class Mutex

class MutexLock {
 public:

  explicit MutexLock(Mutex& mutex) : mutex_(mutex) {
    mutex_.Lock();
  }

  ~MutexLock() {
    mutex_.Unlock();
  }

 private:
  Mutex& mutex_;
}; // class MutexLock

#define MutexLock(x) error "Missing guard object name"

} // namespace simple_reactor


#endif // THREAD_MUTEX_H_
