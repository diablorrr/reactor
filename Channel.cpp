#include "Channel.h"


int Channel::fd() const {
  return m_fd;
}


uint32_t Channel::events() const {
  return m_events;
}


uint32_t Channel::revents() const {
  return m_revents;
}


bool Channel::inpoll() const {
  return m_inepoll;
}


void Channel::setinepoll(bool inepoll) {
  m_inepoll = inepoll;
}


void Channel::setrevents(uint32_t ev) {
  m_revents = ev;
}
