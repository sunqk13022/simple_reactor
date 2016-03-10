#include <stdio.h>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "thread.h"
#include "mutex.h"

using namespace simple_reactor;

class Foo {
 public:
  void doit() const;
};

Mutex mutex;
boost::shared_ptr<std::vector<Foo> > g_foos;

void post(const Foo& f) {
  printf("post\n");
  MutexLock lock(mutex);
  if (!g_foos.unique()) {
    g_foos.reset(new std::vector<Foo>(*g_foos));
    printf("copy the whole list\n");
  }

  g_foos->push_back(f);
}

void traverse() {
  boost::shared_ptr<std::vector<Foo> > foos;
  {
    MutexLock lock(mutex);
    foos = g_foos;
  }

  for (std::vector<Foo>::const_iterator it = foos->begin();
       it != foos->end(); ++it) {
    it->doit();
  }
}

void Foo::doit() const {
  Foo f;
  post(f);
  sleep(1);
}

int main(int argc, char** argv) {
  g_foos.reset(new std::vector<Foo>());
  Foo f;
  post(f);
  traverse();

  printf("g_foos size=%d\n", g_foos->size());
  return 0;
}
