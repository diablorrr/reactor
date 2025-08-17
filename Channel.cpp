#include "Channel.h"
#include <sys/epoll.h>

Channel::Channel(EventLoop* loop, int fd):
  m_loop(loop), m_fd(fd) { }


Channel::~Channel() {}


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


void Channel::enablereading() {
  m_events |= EPOLLIN;
  m_loop->updatechannel(this);
}


void Channel::disablereading() {
  m_events &= ~EPOLLIN;
  m_loop->updatechannel(this);
}


void Channel::enablewriting() {
  m_events |= EPOLLOUT;
  m_loop->updatechannel(this);
}


void Channel::disablewriting() {
  m_events &= ~EPOLLOUT;
  m_loop->updatechannel(this);
}


void Channel::disableall() {
  m_events = 0;
  m_loop->updatechannel(this);
}


void Channel::setreadcallback(std::function<void()> fn) {
  m_readcallback = fn;
}


void Channel::setwritecallback(std::function<void()> fn) {
  m_closecallback = fn;
}


void Channel::setclosecallback(std::function<void()> fn) {
  m_closecallback = fn;
}


void Channel::seterrorcallback(std::function<void()> fn) {
  m_errorcallback = fn;
}


void Channel::handleevent() {
  if (m_revents & EPOLLRDHUP) {
    m_closecallback();
  } else if (m_revents & (EPOLLIN|EPOLLPRI)) {
    m_readcallback();
  } else if (m_revents & EPOLLOUT) {
    m_writecallback();
  } else {
    m_errorcallback();
  }
}
