#include "Socket.h"
#include "InetAddress.h"
#include <cstdint>
#include <sys/socket.h>

Socket::Socket(int fd):m_fd(fd){}

Socket::~Socket() {
  ::close(m_fd);
}

int Socket::fd() const {
  return m_fd;
}

std::string Socket::ip() const {
  return m_ip;
}

uint16_t Socket::port() const {
  return m_port;
}

void Socket::setipport(const std::string& ip, uint16_t port) {
  m_ip = ip;
  m_port = port;
}

void Socket::bind(const InetAddress& servaddr) {
  if (::bind(m_fd, servaddr.addr(), sizeof(sockaddr)) < 0) {
    perror("bind() failed");
    close(m_fd);
    exit(-1);
  }

  setipport(servaddr.ip(), servaddr.port());
}

void Socket::listen(int nn) {
  if (::listen(m_fd, nn) != 0) {
    perror("listen() failed");
    close(m_fd);
    exit(-1);
  }
}

int Socket::accept(InetAddress& clientaddr) {
  sockaddr_in peeraddr;
  socklen_t len = sizeof(peeraddr);
  int clientfd = accept4(m_fd, (sockaddr*)&peeraddr, &len, SOCK_NONBLOCK);

  clientaddr.setaddr(peeraddr);

  return clientfd;
}

void Socket::settcpnodelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

// 地址重用
void Socket::setreuseaddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

// 端口重用
void Socket::setreuseport(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(m_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

// 心跳
void Socket::setkeepalive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}
