#include "Connection.h"
#include "Timestamp.h"
#include <cerrno>

Connection::Connection(EventLoop* loop, std::unique_ptr<Socket> clientsock):
  m_loop(loop), m_clientsock(std::move(clientsock)), m_disconnect(false), m_clientchannel(new Channel(m_loop, m_clientsock->fd()))
{
  m_clientchannel->setreadcallback(std::bind(&Connection::onmessage, this));
  m_clientchannel->setclosecallback(std::bind(&Connection::closecallback, this));
  m_clientchannel->seterrorcallback(std::bind(&Connection::errorcallback, this));
  m_clientchannel->setwritecallback(std::bind(&Connection::writecallback, this));
  m_clientchannel->useet();
  m_clientchannel->enablereading();
}


Connection::~Connection() {

}


int Connection::fd() const {
  return m_clientsock->fd();
}


std::string Connection::ip() const {
  return m_clientsock->ip();
}


uint16_t Connection::port() const {
  return m_clientsock->port();
}


void Connection::onmessage() {
  char buffer[1024];
  while (true) {
    ssize_t nread = read(fd(), buffer, sizeof(buffer));
    if (nread > 0) {
      m_inputbuffer.append(buffer, nread);
    } else if (nread == -1 && errno == EINTR) {
      continue;
    } else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {

      std::string message;

      while (true) {
        if (m_inputbuffer.pickmessage(message) == false) break;

        m_lasttime = Timestamp::now();

        m_onmessagecallback(shared_from_this(), message);
      }
      break;
    } else if (nread == 0) {
      closecallback();
      break;
    }
  }
}


void Connection::closecallback() {
  m_disconnect = true;
  m_clientchannel->remove();
  m_closecallback(shared_from_this());
}


void Connection::errorcallback() {
  m_disconnect = true;
  m_clientchannel->remove();
  m_errorcallback(shared_from_this());
}


void Connection::writecallback() {
  int writen = ::send(fd(), m_outputbuffer.data(), m_outputbuffer.size(), 0);
  if (writen > 0)
    m_outputbuffer.erase(0, writen);

  if (m_outputbuffer.size() == 0) {
    m_clientchannel->disablewriting();
    m_sendcompletecallback(shared_from_this());
  }
}


void Connection::setclosecallback(std::function<void(spConnection)> fn) {
  m_closecallback = fn;
}


void Connection::seterrorcallback(std::function<void(spConnection)> fn) {
  m_errorcallback = fn;
}


void Connection::setonmessagecallback(std::function<void(spConnection, std::string&)> fn) {
  m_onmessagecallback = fn;
}


void Connection::setsendcompletecallback(std::function<void(spConnection)> fn) {
  m_sendcompletecallback = fn;
}


bool Connection::timeout(time_t now, int val) {
  return now - m_lasttime.toint() > val;
}

// 发送数据，不管在任何线程，都是调用这个函数发送数据
void Connection::send(const char* data, size_t size) {
  if (m_disconnect == true) {
    printf("客户端连接已断开，send()直接返回。\n");
    return;
  }

  if (m_loop->isinloopthread()) { // 若是 IO线程（事件循环线程）
    sendinloop(data, size);
  } else {
    m_loop->queueinloop(std::bind(&Connection::sendinloop, this, data, size));
  }
}


void Connection::sendinloop(const char* data, size_t size) {
  m_outputbuffer.appendwithsep(data, size);
  m_clientchannel->enablewriting();
}
