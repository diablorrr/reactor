#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"
#include "ThreadPool.h"
#include <cstdint>

class EchoServer {
private:
  TcpServer m_tcpserver;
  ThreadPool m_threadpool;

public:
  EchoServer(const std::string& ip, const uint16_t port, int subthreadnum=3, int workthreadnum=5);
  ~EchoServer();

  void Start();
  void Stop();

  void HandleNewConnection(spConnection conn);
  void HandleClose(spConnection conn);
  void HandleError(spConnection conn);
  void HandleMessage(spConnection conn, std::string& message);
  void HandleSendComplete(spConnection conn);

  void OnMessage(spConnection conn, std::string& message);
};
