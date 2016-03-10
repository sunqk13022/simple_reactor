#include "mutex.h"
#include "thread.h"

#include <set>
#include <stdio.h>

#include "../logging/logging.h"

using namespace simple_reactor;

class Request;

class Inventory {
 public:

  void add(Request* req) {
    MutexLock lock(mutex_);
    requests_.insert(req);
  }

  void remove(Request* req) {
    MutexLock lock(mutex_);
    requests_.erase(req);
  }

  void printAll() const;

 private:
  mutable Mutex mutex_;
  std::set<Request*> requests_;
};

Inventory g_inventory;

class Request {
 public:

  void process() {
    MutexLock lock(mutex_);
    g_inventory.add(this);
    LOG_INFO << "Leaving process";
  }

  ~Request() {
    MutexLock lock(mutex_);
    sleep(1);
    g_inventory.remove(this);
    LOG_INFO << "delete request";
  }

  void print() const {
    MutexLock lock(mutex_);
  }

 private:
  mutable Mutex mutex_;
};

void Inventory::printAll() const {
  MutexLock lock(mutex_);
  sleep(1);
  for (std::set<Request*>::const_iterator it = requests_.begin();
       it != requests_.end(); ++it) {
    (*it)->print();
  }
  LOG_INFO << "printAll Unlocked\n";
}

void threadFunc() {
  Request* req = new Request();
  req->process();
  delete req;
}

int main() {
  LOG_INFO << "BEGIN";
  Thread th(threadFunc);
  th.Start();
  usleep(500 * 1000);
  g_inventory.printAll();
  th.Join();
}
