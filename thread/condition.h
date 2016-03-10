#ifndef SIMPLE_REACTOR_THREAD_CONDITION_H_
#define SIMPLE_REACTOR_THREAD_CONDITION_H_

#include <pthread.h>
#include <errno.h>

#include "mutex.h"

namespace simple_reactor {

class Condition {
 public:

  explicit Condition(Mutex& mutex) : mutex_(mutex) {
    pthread_cond_init(&pcond_, NULL);
  }

  ~Condition() {
    pthread_cond_destroy(&pcond_);
  }

  void Wait() {
    pthread_cond_wait(&pcond_, mutex_.GetMutex());
  }

  void Notify() {
    pthread_cond_signal(&pcond_);
  }


 private:

  Mutex&           mutex_;
  pthread_cond_t   pcond_;
}; // class Condition

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_THREAD_CONDITION_H_
