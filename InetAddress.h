#pragma once
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

class InetAddress
{
private:
  sockaddr_in m_addr;

public:
  InetAddress(const std::string& ip, uint16_t port);
  InetAddress(const sockaddr_in addr);
  InetAddress() = default;
  ~InetAddress() = default;

  const char *ip() const;
  uint16_t port() const;
  const sockaddr* addr() const;
  void setaddr(sockaddr_in clientaddr);
};
