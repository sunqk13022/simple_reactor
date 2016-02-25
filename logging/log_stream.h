#ifndef SIMPLE_REACTOR_LOGSTREAM_H_
#define SIMPLE_REACTOR_LOGSTREAM_H_

#include <assert.h>
#include <string.h>
#include <string>

namespace simple_reactor {

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template<int SIZE>
class FixedBuffer {
 public:

  FixedBuffer() : cur_(data_) {}
  ~FixedBuffer() {}

  void Append(const char* buf, int len) {
    if (Avail() > len) {
      memcpy(cur_, buf, len);
      cur_ += len;
    }
  }
  void Reset() { cur_ = data_; }
  char* Current() { return cur_; }
  void  AddLength(size_t len) { cur_ += len; }

  int Avail() const { return static_cast<int>(End() - cur_); }
  int Length() const { return static_cast<int>(cur_ - data_); }

  std::string ToString() const { return std::string(data_, Length()); }

 private:

  const char* End() const { return data_ + sizeof(data_); }

  char  data_[SIZE];
  char* cur_;
}; // class FixedBuffer

class LogStream {
 public:
  typedef FixedBuffer<kSmallBuffer> Buffer;

  LogStream();
  ~LogStream();

  LogStream& operator << (bool v);
  LogStream& operator << (short v);
  LogStream& operator << (unsigned short v);
  LogStream& operator << (int v);
  LogStream& operator << (unsigned int v);
  LogStream& operator << (long v);
  LogStream& operator << (unsigned long v);
  LogStream& operator << (const void* v);
  LogStream& operator << (float v);
  LogStream& operator << (double v);
  LogStream& operator << (char v);
  LogStream& operator << (const char* v);
  LogStream& operator << (const std::string& v);

  void ResetBuffer();
  const Buffer& GetBuffer() const {
    return buffer_;
  }

 private:
  template<typename T>
  void formatInteger(T);

  Buffer buffer_;
  static const int kMaxNumSize = 32;

 private:
  LogStream(const LogStream& obj);
  LogStream& operator = (const LogStream& obj);
}; // class LogStream

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_LOGSTREAM_H_
