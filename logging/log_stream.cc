#include "log_stream.h"

#include <algorithm>
#include <stdint.h>
#include <stdio.h>

namespace simple_reactor {

template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargeBuffer>;

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;
const char digitsHex[] = "0123456789abcdef";

template<typename T>
size_t convert(char buf[], T value) {
  T i = value;
  char* p = buf;

  do {
    int lsd = i % 10;
    i = i / 10;
    *p = zero[lsd];
    ++p;
  } while ( i != 0);

  if ( value < 0) {
    (*p) = '-';
    ++p;
  }
  *p = '\0';
  std::reverse(buf, p);
  return p - buf;
}

size_t convertHex(char buf[], uintptr_t value) {
  uintptr_t i = value;
  char* p = buf;

  do {
    int lsd = i % 16;
    i = i / 16;
    *p = digitsHex[lsd];
    ++p;
  } while (i != 0);

  *p = '\0';
  std::reverse(buf, p);
  return p - buf;
}

LogStream::LogStream() {
}

LogStream::~LogStream() {
}

template<typename T>
void LogStream::formatInteger(T v) {
  if (buffer_.Avail() > kMaxNumSize) {
    size_t len = convert(buffer_.Current(), v);
    buffer_.AddLength(len);
  }
}

LogStream& LogStream::operator << (bool v) {
  buffer_.Append(v? "1" : "0", 1);
  return *this;
}

LogStream& LogStream::operator << (short v) {
  *this << static_cast<int>(v);
  return *this;
}

LogStream& LogStream::operator << (unsigned short v) {
  *this << static_cast<unsigned int>(v);
  return *this;
}


LogStream& LogStream::operator << (int v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator << (unsigned int v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator << (long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator << (unsigned long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator << (const void* v) {
  if (buffer_.Avail() > kMaxNumSize) {
    uintptr_t vp = reinterpret_cast<uintptr_t>(v);
    char* buf = buffer_.Current();
    buf[0] = '0';
    buf[1] = 'x';
    size_t len = convertHex(buf + 2, vp);
    buffer_.AddLength(len + 2);
  }
  return *this;
}

LogStream& LogStream::operator << (float v) {
  *this << static_cast<double>(v);
  return *this;
}

LogStream& LogStream::operator << (double v) {
  if (buffer_.Avail() > kMaxNumSize) {
    int len = snprintf(buffer_.Current(), kMaxNumSize, "%.12g", v);
    buffer_.AddLength(len);
  }
  return *this;
}

LogStream& LogStream::operator << (char v) {
  buffer_.Append(&v, 1);
  return *this;
}

LogStream& LogStream::operator << (const char* v) {
  buffer_.Append(v, strlen(v));
  return *this;
}

LogStream& LogStream::operator << (const std::string& v) {
  buffer_.Append(v.c_str(), v.size());
  return *this;
}

void LogStream::ResetBuffer() {
  buffer_.Reset();
}

} // namespace simple_reactor
