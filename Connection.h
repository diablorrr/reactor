#pragma once
#include <functional>
#include <memory>
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Buffer.h"
#include "Timestamp.h"

class Connection;

using spConnection = std::shared_ptr<Connection>;

class Connection : public std::enable_shared_from_this<Connection> {
private:
  EventLoop* m_loop;
  std::unique_ptr<Socket> m_clientsock;
  std::unique_ptr<Channel> m_clientchannel;
  Buffer m_inputbuffer;
  Buffer m_outputbuffer;
  std::atomic_bool m_disconnect;

  std::function<void(spConnection)> m_closecallback;
  std::function<void(spConnection)> m_errorcallback;
  std::function<void(spConnection, std::string&)> m_onmessagecallback;
  std::function<void(spConnection)> m_sendcompletecallback;
  Timestamp m_lasttime;

public:
  Connection(EventLoop* loop, std::unique_ptr<Socket> clientsock);
  ~Connection();

  int fd() const;
  std::string ip() const;
  uint16_t port() const;

  void onmessage();
  void closecallback();
  void errorcallback();
  void writecallback();

  void setclosecallback(std::function<void(spConnection)> fn);
  void seterrorcallback(std::function<void(spConnection)> fn);
  void setonmessagecallback(std::function<void(spConnection, std::string&)> fn);
  void setsendcompletecallback(std::function<void(spConnection)> fn);

  void send(const char* data, size_t size);
  void sendinloop(const char* data, size_t size);

  bool timeout(time_t now, int val);
};
