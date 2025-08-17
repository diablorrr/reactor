#include "TcpServer.h"

TcpServer::TcpServer(const std::string& ip, const uint16_t port, int threadnum):
  m_threadnum(threadnum), m_mainloop(new EventLoop(true)),
{

}
