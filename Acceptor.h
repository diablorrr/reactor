#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <functional>
#include <utility>
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "InetAddress.h"

class Acceptor {
private:
  EventLoop* m_loop;
  Socket m_servsock;
  Channel m_acceptchannel;
  std::function<void(std::unique_ptr<Socket>)> m_newconnectioncb;

public:
  Acceptor(EventLoop* loop, const std::string& ip, const uint16_t port);
  ~Acceptor();

  void newconnection();
  void setnewconnection(std::function<void(std::unique_ptr<Socket>)> fn);
};
