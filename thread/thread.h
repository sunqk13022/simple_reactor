#ifndef THREAD_THREAD_H_
#define THREAD_THREAD_H_

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <pthread.h>

#include "atomic.h"

namespace thread {

class Thread {
 public:
  typedef boost::function<void ()> ThreadFunc;

  explicit Thread(const ThreadFunc& f, const std::string& name = "");
  ~Thread();

  void Start();
  void Join();

  bool Started() const { }


 private:
  bool       started_;
  bool       joined_;
  pthread_t  pthreadId_;
  boost::shared_ptr<pid_t> tid_;
  ThreadFunc func_;
  std::string name_;

  static AtomicInt32 num_created_;

 private:
  Thread(const Thread& obj);
  Thread& operator = (const Thread& obj);
}; // class Thread

} // namespace thread

#endif // THREAD_THREAD_H_
