#pragma once
#include <cstdint>
#include <functional>
#include "EventLoop.h"

class EventLoop;

class Channel {
private:
  int m_fd = -1;
  EventLoop* m_loop;
  bool m_inepoll = false;
  uint32_t m_events = 0;
  uint32_t m_revents = 0;
  std::function<void()> m_readcallback;
  std::function<void()> m_writecallback;
  std::function<void()> m_closecallback;
  std::function<void()> m_errorcallback;

public:
  Channel(EventLoop* loop, int fd);
  ~Channel();

  int fd() const;
  uint32_t events() const;
  uint32_t revents() const;
  bool inpoll() const;
  void setinepoll(bool inepoll);
  void setrevents(uint32_t ev);
  void enablereading();
  void disablereading();
  void enablewriting();
  void disablewriting();
  void disableall();

  void handleevent();

  void setreadcallback(std::function<void()> fn);
  void setwritecallback(std::function<void()> fn);
  void setclosecallback(std::function<void()> fn);
  void seterrorcallback(std::function<void()> fn);
};
