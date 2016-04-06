#ifndef SIMPLE_REACTOR_S04_SOCKET_H_
#define SIMPLE_REACTOR_S04_SOCKET_H_

namespace simple_reactor {

class InetAddress;

class Socket {
 public:
  explicit Socket(int fd);
  ~Socket();

  int  GetFd() const;

  void Bind(const InetAddress& addr);
  void Listen();
  int  Accept(InetAddress* addr);
  void SetReuseAddr(bool on);
  void ShutdownWrite();
  void SetTcpNoDelay(bool on);

 private:
  const int sockfd_;
}; // class Socket

} // simple_reactor

#endif // SIMPLE_REACTOR_S04_SOCKET_H_
