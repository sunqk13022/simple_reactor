#include "socket_fun.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>

#include <logging/logging.h>

namespace simple_reactor {
namespace socket_fun {

uint64_t HostToNetwork64(uint64_t num) {
  return htobe64(num);
}

uint64_t NetworkToHost64(uint64_t num) {
  return be64toh(num);
}

uint32_t HostToNetwork32(uint32_t num) {
  return htonl(num);
}

uint32_t NetworkToHost32(uint32_t num) {
  return ntohl(num);
}

uint16_t HostToNetwork16(uint16_t num) {
  return htons(num);
}

uint16_t NetworkToHost16(uint16_t num) {
  return ntohs(num);
}

struct sockaddr* sockaddr_cast(struct sockaddr_in* addr) {
  return (struct sockaddr*)addr;
}
const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr) {
  return (const struct sockaddr*)addr;
}

int  CreateSocketNonblocking() {
  int sockfd = ::socket(AF_INET,
                        SOCK_STREAM | SOCK_NONBLOCK|SOCK_CLOEXEC,
                        IPPROTO_TCP);
  if (sockfd < 0) {
    LOG_FATAL << "CreateSocketNonblocking ERROR";
  }
  return sockfd;
}

void Bind(int sockfd, const struct sockaddr_in& addr) {
  int ret = ::bind(sockfd, sockaddr_cast(&addr), sizeof(addr));
  if (ret < 0) {
    LOG_FATAL << "Bind Error!";
  }
}

void Listen(int sockfd) {
  int ret = ::listen(sockfd, SOMAXCONN);
  if (ret < 0) {
    LOG_FATAL << "Listen Error!";
  }
}

int  Accept(int sockfd, struct sockaddr_in* addr) {
  socklen_t addrlen = sizeof(*addr);
  int connfd = ::accept4(sockfd, sockaddr_cast(addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
  if (connfd < 0) {
    LOG_FATAL << "Accept Error!";
  }
  return connfd;
}

void Close(int sockfd) {
  ::close(sockfd);
}

void ShutdownWrite(int sockfd) {
  if (::shutdown(sockfd, SHUT_WR) < 0) {
    LOG_FATAL << "socket_fun::ShutdownWrite";
  }
}

void ParseSockaddrFromHostPort(const char* ip, uint16_t port, struct sockaddr_in* addr) {
  addr->sin_family = AF_INET;
  addr->sin_port = HostToNetwork16(port);
  if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
    LOG_FATAL << "ParseSockaddrFromHostPort Error!";
  }
}

void SockaddrToString(const struct sockaddr_in& addr, std::string* host) {
  char buf[INET_ADDRSTRLEN] = "INVALID";
  ::inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof(buf));
  uint16_t port = NetworkToHost16(addr.sin_port);

  char out_buf[32] = {0};
  snprintf(out_buf, sizeof(out_buf), "%s:%u", buf, port);
  host->assign(out_buf);
}

struct sockaddr_in GetLocalAddr(int sockfd) {
  struct sockaddr_in localaddr;
  bzero(&localaddr, sizeof(localaddr));
  socklen_t addrlen = sizeof(localaddr);
  if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0) {
    LOG_FATAL << "socket_fun::GetLocalAddr Error!";
  }
  return localaddr;
}

int GetSocketError(int sockfd) {
  int optval;
  socklen_t optlen = sizeof(optval);

  if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
    return errno;
  } else {
    return optval;
  }
}

} // namespace socket_fun
} // namespace simple_reactor
