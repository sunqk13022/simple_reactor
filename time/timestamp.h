#ifndef SIMPLE_REACTOR_TIMESTAMP_H_
#define SIMPLE_REACTOR_TIMESTAMP_H_

#include <stdint.h>
#include <string>

namespace simple_reactor {

class Timestamp {
 public:
  Timestamp();
  explicit Timestamp(int64_t t);

  std::string ToString() const;
  std::string ToFormattedString() const;

  bool Valid() const { return ns_ > 0; }
  int64_t Ns() const { return ns_; }

  static Timestamp Now();

 private:
  int64_t  ns_;
}; // class Timestamp

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_TIMESTAMP_H_
