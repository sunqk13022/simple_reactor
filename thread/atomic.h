#ifndef ATOMIC_H
#define ATOMIC_H

#include <boost/noncopyable.hpp>
#include <stdint.h>

namespace simple_reactor {

template<typename T>
class AtomicIntegerT {
 public:
  AtomicIntegerT() : value_(0) {
  }

  T get() const {
    return __sync_val_compare_and_swap(const_cast<volatile T*>(&value_), 0, 0);
  }

  T getAndAdd(T x) {
    return __sync_fetch_and_add(&value_, x);
  }

  T addAndGet(T x) {
    return getAndAdd(x) + x;
  }

  T incrementAndGet() {
    return addAndGet(1);
  }

  void add(T x) {
    getAndAdd(x);
  }

  void increment() {
    incrementAndGet();
  }

  void decrement() {
    getAndAdd(-1);
  }

  T getAndSet(T newvalue) {
    return __sync_lock_test_and_set(&value_, newvalue);
  }

 private:
  volatile T value_;
}; // class AtomicIntegerT

typedef AtomicIntegerT<int32_t> AtomicInt32;
typedef AtomicIntegerT<int64_t> AtomicInt64;

} // namespace simple_reactor

#endif // ATOMIC_H
