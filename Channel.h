#pragma once
#include <cstdint>

class Channel {
private:
  int m_fd = -1;
  uint32_t m_events = 0;
  uint32_t m_revents = 0;
  bool m_inepoll = false;

public:
  int fd() const;
  uint32_t events() const;
  uint32_t revents() const;
  bool inpoll() const;
  void setinepoll(bool inepoll);
  void setrevents(uint32_t ev);
};
