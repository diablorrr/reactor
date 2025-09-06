#include "InetAddress.h"

InetAddress::InetAddress() {

}

InetAddress::~InetAddress() {

}

// 要发送数据，转为网络序
InetAddress::InetAddress(const std::string& ip, uint16_t port) {
  m_addr.sin_family = AF_INET;
  m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
  m_addr.sin_port = htons(port);
}

InetAddress::InetAddress(const sockaddr_in addr) : m_addr{addr} { }

// 取来用时，转为主机序
const char *InetAddress::ip() const {
  return inet_ntoa(m_addr.sin_addr);
}

uint16_t InetAddress::port() const {
  return ntohs(m_addr.sin_port);
}

const sockaddr* InetAddress::addr() const {
  return (sockaddr*)&m_addr;
}

void InetAddress::setaddr(sockaddr_in clientaddr) {
  m_addr = clientaddr;
}
