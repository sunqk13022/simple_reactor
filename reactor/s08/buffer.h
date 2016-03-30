#ifndef SIMPLE_REACTOR_S07_BUFFER_H_
#define SIMPLE_REACTOR_S07_BUFFER_H_

#include <algorithm>
#include <string>
#include <vector>

namespace simple_reactor {

class Buffer {
 public:
  static const size_t kCheapPrepend = 8;
  static const size_t kInitialSize  = 1024;

  Buffer();
  ~Buffer();

  size_t  GetReadableBytes() const;
  size_t  GetWritableBytes() const;
  size_t  GetPrepenableBytes() const;

  const char* Peek() const;

  void Retrieve(size_t len);
  void RetrieveUntil(const char* end);
  void RetrieveAll();
  std::string RetrieveAsString();

  void Append(const char* data, size_t len);
  void Append(const void* data, size_t len);
  void Append(const std::string& str);

  void PrePend(const void* data, size_t len);

  void EnsureWritableBytes(size_t len);

  ssize_t ReadFd(int fd, int* errnum);

 private:
  char* Begin();
  const char* Begin() const;

  char* BeginWrite();

  void MakeSpace(size_t len);

 private:
  std::vector<char>   buffer_;
  size_t              read_index_;
  size_t              write_index_;
}; // class Buffer

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_S07_BUFFER_H_
