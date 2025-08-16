#include "Epoll.h"
#include "Channel.h"
#include <cstdlib>
#include <sys/epoll.h>
#include <vector>

Epoll::Epoll() {
  if((m_epollfd = epoll_create(1)) == -1) {
    printf("epoll_create() failed(%d).\n", errno);
    exit(-1);
  }
}


Epoll::~Epoll() {
  close(m_epollfd);
}


void Epoll::updatechannel(Channel* ch) {
  epoll_event ev;
  ev.data.ptr = ch;
  ev.events = ch->events();

  if (ch->inpoll()) {
    if (epoll_ctl(m_epollfd, EPOLL_CTL_MOD, ch->fd(), &ev) == -1) {
      perror("epoll_ctl() failed.\n");
      exit(-1);
    }
  } else {
    if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, ch->fd(), &ev) == -1) {
      perror("epoll_ctl() failed.\n");
      exit(-1);
    }
    ch->setinepoll(true);
  }
}


void Epoll::removechannel(Channel* ch) {
  if (epoll_ctl(m_epollfd, EPOLL_CTL_DEL, ch->fd(), 0) == -1) {
    perror("epoll_ctl() failed.\n");
    exit(-1);
  }
}


std::vector<Channel*> Epoll::loop(int timeout) {
  std::vector<Channel*> channels;

  // bzero(m_events, sizeof(m_events));
  int fds = epoll_wait(m_epollfd, m_events, MaxEvents, timeout);

  if (fds < 0) {
    perror("epoll_wait() failed.\n");
    exit(-1);
  }

  if (fds == 0) {
    return channels;
  }

  for (int i = 0; i < fds; ++i) {
    Channel* ch = (Channel*)m_events[i].data.ptr;
    ch->setrevents(m_events[i].events);
    channels.push_back(ch);
  }

  return channels;
}
