#include "EventLoop.h"
#include <cstdint>
#include <cstring>
#include <ctime>
#include <functional>
#include <mutex>
#include <sys/time.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <sys/syscall.h>

int createtimerfd(int sec=30) {
  int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC|TFD_NONBLOCK);
  struct itimerspec timeout;
  memset(&timeout, 0, sizeof(itimerspec));
  timeout.it_value.tv_sec = sec;
  timeout.it_value.tv_nsec = 0;
  timerfd_settime(tfd, 0, &timeout, 0);
  return tfd;
}


EventLoop::EventLoop(bool mainloop, int timetvl, int timeout):
  m_ep(new Epoll), m_mainloop(mainloop), m_timetvl(timetvl), m_timeout(timeout),
  m_stop(false), m_wakeupfd(eventfd(0, EFD_NONBLOCK)), m_wakechannel(new Channel(this, m_wakeupfd)),
  m_timerfd(createtimerfd(m_timeout)), m_timerchannel(new Channel(this, m_timerfd))
{
  m_wakechannel->setreadcallback(std::bind(&EventLoop::handlewakeup,this));
  m_wakechannel->enablereading();

  m_timerchannel->setreadcallback(std::bind(&EventLoop::handletimer, this));
  m_timerchannel->enablereading();
}


EventLoop::~EventLoop() {

}


void EventLoop::run() {
  m_threadid = syscall(SYS_gettid);

  while (m_stop == false) {

    std::vector<Channel*> channels = m_ep->loop(10*1000);

    if (channels.size() == 0) {
      m_epolltimeoutcallback(this);
    } else {
      for (auto& ch : channels) {
        ch->handleevent();
      }
    }
  }
}


// 唤醒事件循环线程
void EventLoop::wakeup() {
  uint64_t val = 1;
  write(m_wakeupfd, &val, sizeof(val));
}


void EventLoop::stop() {
  m_stop = true;
  wakeup();
}


void EventLoop::updatechannel(Channel* ch) {
  m_ep->updatechannel(ch);
}


void EventLoop::removechannel(Channel* ch) {
  m_ep->removechannel(ch);
}


void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)> fn) {
  m_epolltimeoutcallback = fn;
}


// 将被设置为 TcpServer::removeconn()
void EventLoop::settimercallback(std::function<void(int)> fn) {
  m_timercallback = fn;
}


bool EventLoop::isinloopthread() {
  return m_threadid == syscall(SYS_gettid);
}


// 将任务添加到队列
void EventLoop::queueinloop(std::function<void()> fn) {
  {
    std::lock_guard<std::mutex> gd(m_mutex);
    m_taskqueue.push(fn);
  }

  wakeup();
}


void EventLoop::handlewakeup() {
  uint64_t val;
  read(m_wakeupfd, &val, sizeof(val)); // 从 eventfd中读数据，若不读，则读n事件会一直触发

  std::function<void()> fn;

  std::lock_guard<std::mutex> gd(m_mutex);

  while (m_taskqueue.size() > 0) {
    fn = std::move(m_taskqueue.front());
    m_taskqueue.pop();
    fn();
  }
}


// 闹钟响时执行的函数
void EventLoop::handletimer() {
  // 重新计时
  struct itimerspec timeout;
  memset(&timeout, 0, sizeof(itimerspec));
  timeout.it_value.tv_sec = m_timetvl;
  timeout.it_value.tv_nsec = 0;
  timerfd_settime(m_timerfd, 0, &timeout, 0);

  if (m_mainloop) {

  } else {
    time_t now = time(0);
    for (auto aa:m_conns) {
      if (aa.second->timeout(now, m_timeout)) {
        {
          std::lock_guard<std::mutex> gd(m_mmutex);
          m_conns.erase(aa.first);
        }
        m_timercallback(aa.first);   // 从 TcpServer 的 map 中删除超时的 conn
      }
    }
  }
}


void EventLoop::newconnection(spConnection conn) {
  std::lock_guard<std::mutex> gd(m_mmutex);
  m_conns[conn->fd()] = conn;
}
