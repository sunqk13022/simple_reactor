#include "buffer.h"

#include <errno.h>
#include <memory.h>
#include <sys/uio.h>

namespace simple_reactor {

Buffer::Buffer()
: buffer_(kCheapPrepend + kInitialSize),
  read_index_(kCheapPrepend),
  write_index_(kCheapPrepend)
{
}

Buffer::~Buffer() {
}

size_t  Buffer::GetReadableBytes() const {
  return write_index_ - read_index_;
}

size_t  Buffer::GetWritableBytes() const {
  return buffer_.size() - write_index_;
}

size_t  Buffer::GetPrepenableBytes() const {
  return read_index_;
}

char* Buffer::Begin() {
  return &*buffer_.begin();
}

const char* Buffer::Begin() const {
  return &*buffer_.begin();
}

char* Buffer::BeginWrite() {
  return Begin() + write_index_;
}

void Buffer::MakeSpace(size_t len) {
  if (GetWritableBytes() + GetPrepenableBytes() < len + kCheapPrepend) {
    buffer_.resize(write_index_ + len);
  } else {
    size_t readable = GetReadableBytes();
    std::copy(Begin() + read_index_,
              Begin() + write_index_,
              Begin() + kCheapPrepend);
    read_index_ = kCheapPrepend;
    write_index_ = read_index_ + readable;
  }
}

const char* Buffer::Peek() const {
  return Begin() + read_index_;
}

void Buffer::Retrieve(size_t len) {
  read_index_ += len;
}

void Buffer::RetrieveUntil(const char* end) {
  Retrieve(end - Peek());
}

void Buffer::RetrieveAll() {
  read_index_ = kCheapPrepend;
  write_index_ = kCheapPrepend;
}

std::string Buffer::RetrieveAsString() {
  std::string str(Peek(), GetReadableBytes());
  RetrieveAll();
  return str;
}

void Buffer::Append(const char* data, size_t len) {
  EnsureWritableBytes(len);
  std::copy(data, data + len, BeginWrite());
}

void Buffer::Append(const void* data, size_t len) {
  Append(static_cast<const char*>(data), len);
}

void Buffer::Append(const std::string& str) {
  Append(str.c_str(), str.length());
}

void Buffer::EnsureWritableBytes(size_t len) {
  if (GetWritableBytes() < len) {
    MakeSpace(len);
  }
}

void Buffer::PrePend(const void* data, size_t len) {
  read_index_ -= len;
  const char *d = static_cast<const char*>(data);
  std::copy(d, d + len, Begin() + read_index_);
}

ssize_t Buffer::ReadFd(int fd, int* errnum) {
  char extrabuf[65536];
  struct iovec vec[2];
  const size_t writable = GetWritableBytes();
  vec[0].iov_base = BeginWrite();
  vec[0].iov_len = writable;
  vec[1].iov_base = extrabuf;
  vec[1].iov_len = sizeof(extrabuf);

  const ssize_t n = readv(fd, vec, 2);
  if (n < 0) {
    *errnum = errno;
  } else if ((size_t)(n) <= writable) {
    write_index_ += n;
  } else {
    write_index_ = buffer_.size();
    Append(extrabuf, n - writable);
  }
  return n;
}

} // namespace simple_reactor
