#include "TcpServer.h"
#include "EventLoop.h"
#include "Socket.h"
#include "ThreadPool.h"
#include <mutex>

TcpServer::TcpServer(const std::string& ip, const uint16_t port, int threadnum)
  :m_threadnum(threadnum), m_mainloop(new EventLoop(true)),
   m_acceptor(m_mainloop.get(), ip, port), m_threadpool(m_threadnum, "IO")
{
  m_mainloop->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));
  m_acceptor.setnewconnection(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));

  for (int i = 0; i < m_threadnum; ++i) {
    m_subloops.emplace_back(new EventLoop(false, 5, 10));
    m_subloops[i]->setepolltimeoutcallback(std::bind(&TcpServer::epolltimeout, this, std::placeholders::_1));
    m_subloops[i]->settimercallback(std::bind(&TcpServer::removeconn, this, std::placeholders::_1));
    m_threadpool.addtask(std::bind(&EventLoop::run, m_subloops[i].get()));
  }
}


TcpServer::~TcpServer() {

}


void TcpServer::start() {
  m_mainloop->run();
}


void TcpServer::stop() {
  m_mainloop->stop();
  printf("主事件循环已停止。\n");

  for (int i = 0; i < m_threadnum; ++i) {
    m_subloops[i]->stop();
  }
  printf("从事件循环已停止。\n");

  m_threadpool.stop();
  printf("IO线程池停止。\n");
}


void TcpServer::newconnection(std::unique_ptr<Socket> clientsock) {
  spConnection conn(new Connection(m_subloops[clientsock->fd() % m_threadnum].get(), std::move(clientsock)));
  conn->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1));
  conn->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));
  conn->setonmessagecallback(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
  conn->setsendcompletecallback(std::bind(&TcpServer::sendcomplete, this, std::placeholders::_1));

  {
    std::lock_guard<std::mutex> gd(m_mmutex);
    m_conns[conn->fd()] = conn;
  }

  m_subloops[conn->fd() % m_threadnum]->newconnection(conn);

  if (m_newconnectioncb) {
    m_newconnectioncb(conn); // 回调 EchoServer::HandleNewConnection()
  }
}


void TcpServer::closeconnection(spConnection conn) {
  if (m_closeconnectioncb) {
    m_closeconnectioncb(conn); // 回调 EchoServer::HandleClose();
  }

  {
    std::lock_guard<std::mutex> gd(m_mmutex);
    m_conns.erase(conn->fd());
  }
}


void TcpServer::errorconnection(spConnection conn) {
  if (m_errorconnectioncb) {
    m_errorconnectioncb(conn); // 回调 EchoServer::HandleError()
  }

  {
    std::lock_guard<std::mutex> gd(m_mmutex);
    m_conns.erase(conn->fd());
  }
}


void TcpServer::onmessage(spConnection conn, std::string& message) {
  if (m_onmessagecb) {
    m_onmessagecb(conn, message);
  }
}


void TcpServer::sendcomplete(spConnection conn) {
  if (m_sendcompletecb) {
    m_sendcompletecb(conn);
  }
}


void TcpServer::epolltimeout(EventLoop* loop) {
  if (m_timeoutcb) {
    m_timeoutcb(loop);
  }
}


void TcpServer::setnewconnectioncb(std::function<void(spConnection)> fn)
{
    m_newconnectioncb=fn;
}


void TcpServer::setcloseconnectioncb(std::function<void(spConnection)> fn)
{
    m_closeconnectioncb=fn;
}


void TcpServer::seterrorconnectioncb(std::function<void(spConnection)> fn)
{
    m_errorconnectioncb=fn;
}


void TcpServer::setonmessagecb(std::function<void(spConnection,std::string &message)> fn)
{
    m_onmessagecb=fn;
}


void TcpServer::setsendcompletecb(std::function<void(spConnection)> fn)
{
    m_sendcompletecb=fn;
}


void TcpServer::settimeoutcb(std::function<void(EventLoop*)> fn)
{
    m_timeoutcb=fn;
}


void TcpServer::removeconn(int fd) {
  {
    std::lock_guard<std::mutex> gd(m_mmutex);
    m_conns.erase(fd);
  }
}
