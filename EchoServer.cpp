#include "EchoServer.h"
#include "Connection.h"

EchoServer::EchoServer(const std::string& ip, const uint16_t port, int subthreadnum, int workthreadnum):
  m_tcpserver(ip, port, subthreadnum), m_threadpool(workthreadnum, "WORKS")
{
  m_tcpserver.setnewconnectioncb(std::bind(&EchoServer::HandleNewConnection, this, std::placeholders::_1));
  m_tcpserver.setcloseconnectioncb(std::bind(&EchoServer::HandleClose, this, std::placeholders::_1));
  m_tcpserver.seterrorconnectioncb(std::bind(&EchoServer::HandleError, this, std::placeholders::_1));
  m_tcpserver.setonmessagecb(std::bind(&EchoServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
  m_tcpserver.setsendcompletecb(std::bind(&EchoServer::HandleSendComplete, this, std::placeholders::_1));
}


EchoServer::~EchoServer() {

}


void EchoServer::Start() {
  m_tcpserver.start();
}


void EchoServer::Stop() {
  m_threadpool.stop();
  printf("工作线程已停止。\n");

  m_tcpserver.stop();
}


void EchoServer::HandleNewConnection(spConnection conn) {
  printf ("%s new connection(fd=%d,ip=%s,port=%d) ok.\n",Timestamp::now().tostring().c_str(),conn->fd(),conn->ip().c_str(),conn->port());
}


void EchoServer::HandleClose(spConnection conn)
{
    printf ("%s connection closed(fd=%d,ip=%s,port=%d) ok.\n",Timestamp::now().tostring().c_str(),conn->fd(),conn->ip().c_str(),conn->port());
}


void EchoServer::HandleError(spConnection conn)
{
    // 根据业务的需求，在这里可以增加其它的代码。
}


void EchoServer::HandleMessage(spConnection conn, std::string& message) {
  if (m_threadpool.size() == 0) {
    OnMessage(conn, message);
  } else {
    m_threadpool.addtask(std::bind(&EchoServer::OnMessage, this, conn, message));
  }
}


void EchoServer::OnMessage(spConnection conn, std::string& message) {
  message = "reply:" + message;
  conn->send(message.data(), message.size());
}


void EchoServer::HandleSendComplete(spConnection conn) {

}
