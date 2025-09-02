#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <map>
#include "EventLoop.h"
#include "Acceptor.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "Connection.h"

class TcpServer {
private:
  std::unique_ptr<EventLoop> m_mainloop;
  std::vector<std::unique_ptr<EventLoop>> m_subloops;
  Acceptor m_acceptor;
  std::map<int, spConnection> m_conns;

  ThreadPool m_threadpool;
  int m_threadnum;
  std::mutex m_mmutex;

  std::function<void(spConnection)> m_newconnectioncb;
  std::function<void(spConnection)> m_closeconnectioncb;
  std::function<void(spConnection)> m_errorconnectioncb;
  std::function<void(spConnection, std::string& message)> m_onmessagecb;
  std::function<void(spConnection)> m_sendcompletecb;
  std::function<void(EventLoop*)> m_timeoutcb;

public:
  TcpServer(const std::string& ip, const uint16_t port, int threadnum=3);
  ~TcpServer();

  void start();
  void stop();

  void newconnection(std::unique_ptr<Socket> clientsock);
  void closeconnection(spConnection conn);
  void errorconnection(spConnection conn);
  void onmessage(spConnection conn, std::string& message);
  void sendcomplete(spConnection conn);
  void epolltimeout(EventLoop* loop);

  void setnewconnectioncb(std::function<void(spConnection)> fn);
  void setcloseconnectioncb(std::function<void(spConnection)> fn);
  void seterrorconnectioncb(std::function<void(spConnection)> fn);
  void setonmessagecb(std::function<void(spConnection, std::string& message)> fn);
  void setsendcompletecb(std::function<void(spConnection)> fn);
  void settimeoutcb(std::function<void(EventLoop*)> fn);

  void removeconn(int fd);
};
