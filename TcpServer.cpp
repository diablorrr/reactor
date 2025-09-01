#include "TcpServer.h"
#include "ThreadPool.h"

TcpServer::TcpServer(const std::string& ip, const uint16_t port, int threadnum):
  m_threadnum(threadnum), m_mainloop(new EventLoop(true)), m_acceptor(m_mainloop.get(), ip, port), m_threadpool(m_threadnum, "IO")
{

}
