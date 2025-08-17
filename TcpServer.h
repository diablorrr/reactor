#pragma once
#include <memory>
#include <vector>
#include "EventLoop.h"
#include "Acceptor.h"
#include "ThreadPool.h"

class TcpServer {
private:
  std::unique_ptr<EventLoop> m_mainloop;
  std::vector<std::unique_ptr<EventLoop>> m_subloops;
  Acceptor m_acceptor;
  int m_threadnum;
  ThreadPool m_threadpool;

public:
  TcpServer(const std::string& ip, const uint16_t port, int threadnum=3);
  ~TcpServer();
};
