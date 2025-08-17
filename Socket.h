#pragma once
#include <string>
#include <unistd.h>
#include <netinet/tcp.h>
#include "InetAddress.h"

int createnoblocking();

class Socket
{
private:
  int m_fd;
  std::string m_ip;
  uint16_t m_port;

public:
  Socket(int fd);
  ~Socket();

  int fd() const;
  std::string ip() const;
  uint16_t port() const;
  void setipport(const std::string& ip, uint16_t port);

  void setreuseaddr(bool on);
  void setreuseport(bool on);
  void settcpnodelay(bool on);
  void setkeepalive(bool on);
  void bind(const InetAddress& servaddr);
  void listen(int nn=128);
  int accept(InetAddress& clientaddr);
};
