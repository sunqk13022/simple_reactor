#include "socket.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>

#include "inet_address.h"
#include "socket_fun.h"

namespace simple_reactor {

Socket::Socket(int fd)
: sockfd_(fd){
}

Socket::~Socket() {
  socket_fun::Close(sockfd_);
}

int  Socket::GetFd() const {
  return sockfd_;
}

void Socket::Bind(const InetAddress& addr) {
  socket_fun::Bind(sockfd_, addr.GetSockAddrInet());
}

void Socket::Listen() {
  socket_fun::Listen(sockfd_);
}

int  Socket::Accept(InetAddress* peeraddr) {
  struct sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  int connfd = socket_fun::Accept(sockfd_, &addr);
  if (connfd >=0) {
    peeraddr->SetSockAddrInet(addr);
  }
  return connfd;
}

void Socket::SetReuseAddr(bool on) {
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::ShutdownWrite() {
  socket_fun::ShutdownWrite(sockfd_);
}

} // namespace simple_reactor
