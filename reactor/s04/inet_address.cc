#include "inet_address.h"

#include <strings.h>
#include <netinet/in.h>

#include "socket_fun.h"

namespace simple_reactor {

static const in_addr_t kInaddrAny = INADDR_ANY;

InetAddress::InetAddress(uint16_t port) {
  bzero(&addr_, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = socket_fun::HostToNetwork32(kInaddrAny);
  addr_.sin_port = socket_fun::HostToNetwork16(port);
}

InetAddress::InetAddress(const std::string& ip, uint16_t port) {
  bzero(&addr_, sizeof(addr_));
  socket_fun::ParseSockaddrFromHostPort(ip.c_str(), port, &addr_);
}

InetAddress::InetAddress(const struct sockaddr_in& addr)
: addr_(addr) {
}

std::string InetAddress::ToHostPort() const {
  std::string str_ret;
  socket_fun::SockaddrToString(addr_, &str_ret);
  return str_ret;
}

const struct sockaddr_in& InetAddress::GetSockAddrInet() const {
  return addr_;
}

void InetAddress::SetSockAddrInet(const struct sockaddr_in& addr) {
  addr_ = addr;
}


} // namespace simple_reactor
