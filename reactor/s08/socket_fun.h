#ifndef SIMPLE_REACTOR_S04_SOCK_FUN_H_
#define SIMPLE_REACTOR_S04_SOCK_FUN_H_

#include <arpa/inet.h>
#include <endian.h>
#include <string>

namespace simple_reactor {
namespace socket_fun {

uint64_t HostToNetwork64(uint64_t num);
uint64_t NetworkToHost64(uint64_t num);

uint32_t HostToNetwork32(uint32_t num);
uint32_t NetworkToHost32(uint32_t num);

uint16_t HostToNetwork16(uint16_t num);
uint16_t NetworkToHost16(uint16_t num);


int  CreateSocketNonblocking();

void Bind(int sockfd, const struct sockaddr_in& addr);

void Listen(int sockfd);

int  Accept(int sockfd, struct sockaddr_in* addr);

void Close(int sockfd);

void ShutdownWrite(int sockfd);

void ParseSockaddrFromHostPort(const char* ip, uint16_t port, struct sockaddr_in* addr);
void SockaddrToString(const struct sockaddr_in& addr, std::string* host);

struct sockaddr_in GetLocalAddr(int sockfd);

int GetSocketError(int sockfd);

} // namespace socket_fun
} // namespace simple_reactor

#endif // SIMPLE_REACTOR_S04_SOCK_FUN_H_
