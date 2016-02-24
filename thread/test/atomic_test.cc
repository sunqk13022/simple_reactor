#include <stdio.h>
#include <assert.h>

#include "../atomic.h"

int main() {
  simple_reactor::AtomicInt32 a1;
  assert(a1.get() == 0);
  assert(a1.getAndAdd(1) == 0);
  assert(a1.get() == 1);
}
