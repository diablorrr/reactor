#pragma once
#include <sys/epoll.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

class Epoll
{
private:
  int m_epollfd = -1;
  static constexpr int MaxEvents=100;
  epoll_event m_events[MaxEvents];

public:
  Epoll();
  ~Epoll();


};
