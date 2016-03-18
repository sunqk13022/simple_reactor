#ifndef SIMPLE_REACTOR_S04_INET_ADDRESS_H_
#define SIMPLE_REACTOR_S04_INET_ADDRESS_H_

#include <string>
#include <netinet/in.h>

namespace simple_reactor {

class InetAddress {
 public:

  explicit InetAddress(uint16_t port);

  InetAddress(const std::string& ip, uint16_t port);

  InetAddress(const struct sockaddr_in& addr);

  std::string ToHostPort() const;

  const struct sockaddr_in& GetSockAddrInet() const;
  void SetSockAddrInet(const struct sockaddr_in& addr);

 private:
  struct sockaddr_in  addr_;
}; // class InetAddress

} // namespace simple_reactor

#endif // SIMPLE_REACTOR_S04_INET_ADDRESS_H_
