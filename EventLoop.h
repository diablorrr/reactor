#pragma once
#include <memory>
#include <atomic>
#include "Epoll.h"

class EventLoop {
private:
  std::unique_ptr<Epoll> m_ep;
  std::atomic_bool m_stop;
  // TODO 不使用bool，用枚举类优化，明确语义
  bool m_mainloop;              // true 为主事件循环，false 为从事件循环

public:
  EventLoop(bool m_mainloop);

  void run();
  void stop();

  void updatechannel(Channel* ch);
  void removechannel(Channel* ch);
};
