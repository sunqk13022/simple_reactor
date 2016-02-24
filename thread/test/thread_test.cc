#include "../thread.h"

#include <string>
#include <boost/bind.hpp>
#include <stdio.h>

void threadFunc() {
  printf("pid=%d, tid=%d\n", ::getpid(), simple_reactor::CurrentThread::Tid());
}

void threadFunc2(int x) {
  printf("pid=%d, tid=%d, x=%d\n", ::getpid(), simple_reactor::CurrentThread::Tid(), x);
}

void threadFunc3() {
  printf("pid=%d, tid=%d\n", ::getpid(), simple_reactor::CurrentThread::Tid());
  sleep(1);
}

class Foo {
 public:
  explicit Foo(double x) :x_(x) {}

  void memberFunc() {
    printf("pid=%d, tid=%d, x=%f\n", ::getpid(), simple_reactor::CurrentThread::Tid(), x_);
  }

  void memberFunc2(const std::string& text) {
    printf("pid=%d, tid=%d, x=%f, text=%s\n", ::getpid(), simple_reactor::CurrentThread::Tid(), x_, text.c_str());
  }

 private:
  double x_;
}; // class Foo

int main() {
  printf("pid=%d, tid=%d\n", ::getpid(), simple_reactor::CurrentThread::Tid());

  simple_reactor::Thread t1(threadFunc);
  t1.Start();
  t1.Join();

  simple_reactor::Thread t2(boost::bind(threadFunc2, 42), "thread for free func with arg");
  t2.Start();
  t2.Join();

  Foo foo(82.8);
  simple_reactor::Thread t3(boost::bind(&Foo::memberFunc, &foo));
  t3.Start();
  t3.Join();

  simple_reactor::Thread t4(boost::bind(&Foo::memberFunc2, &foo, std::string("yiyiyi")));
  t4.Start();
  t4.Join();

  {
    simple_reactor::Thread t5(threadFunc3);
    t5.Start();
  }

  sleep(2);

  printf("number of create threads %d\n", simple_reactor::Thread::NumCreated());
}
