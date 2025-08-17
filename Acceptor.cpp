#include "Acceptor.h"

Acceptor::Acceptor(EventLoop* loop, const std::string& ip, const uint16_t port):
  m_loop(loop), m_servsock(createnoblocking()), m_acceptchannel(m_loop, m_servsock.fd())
{
  InetAddress servaddr(ip, port);
  m_servsock.setreuseaddr(true);
  m_servsock.settcpnodelay(true);
  m_servsock.setreuseport(true);
  m_servsock.setkeepalive(true);
  m_servsock.bind(servaddr);
  m_servsock.listen();

  m_acceptchannel.setreadcallback(std::bind(&Acceptor::newconnection, this));
  m_acceptchannel.enablereading();
}


Acceptor::~Acceptor() {}


void Acceptor::newconnection() {
  InetAddress clientaddr;

  std::unique_ptr<Socket> clientsock(new Socket(m_servsock.accept(clientaddr)));
  clientsock->setipport(clientaddr.ip(), clientaddr.port());

  m_newconnectioncb(std::move(clientsock));
}


void Acceptor::setnewconnection(std::function<void(std::unique_ptr<Socket>)> fn) {
  m_newconnectioncb = fn;
}
