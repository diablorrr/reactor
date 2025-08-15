#include "Epoll.h"
#include <sys/epoll.h>

Epoll::Epoll() {
  if((m_epollfd = epoll_create(1)) == -1) {
    printf("epoll_create() failed(%d).\n", errno);
    exit(-1);
  }
}

Epoll::~Epoll() {
  close(m_epollfd);
}
