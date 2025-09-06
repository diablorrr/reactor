#pragma once
#include <functional>
#include <memory>
#include <atomic>
#include <sched.h>
#include <mutex>
#include <map>
#include <queue>
#include "Epoll.h"
#include "Channel.h"
#include "Connection.h"

class Connection;
class Channel;
class Epoll;
using spConnection = std::shared_ptr<Connection>;

class EventLoop {
private:
  int m_timetvl;                // 闹钟时间间隔
  int m_timeout;                // Connection对象超时时间
  bool m_mainloop;              // true 为主事件循环，false 为从事件循环

  std::unique_ptr<Epoll> m_ep;
  std::map<int, spConnection> m_conns;            // Connection的fd 和 对应的Connection对象
  std::queue<std::function<void()>> m_taskqueue;  // 不处理业务逻辑

  pid_t m_threadid;

  std::atomic_bool m_stop;
  std::mutex m_mutex;           // 任务队列同步的互斥锁
  std::mutex m_mmutex;          // 保护 m_conns 的互斥锁

  std::function<void(EventLoop*)> m_epolltimeoutcallback;  // epoll_wait() 超时的回调函数
  std::function<void(int)> m_timercallback; // 超时回调，删除超时的 Connection 对象

  int m_wakeupfd;
  std::unique_ptr<Channel> m_wakechannel;
  int m_timerfd;
  std::unique_ptr<Channel> m_timerchannel;


public:
  EventLoop(bool m_mainloop, int m_timetvl=30, int m_timeout=80);
  ~EventLoop();

  void run();
  void stop();

  bool isinloopthread(); // 判断当前线程是否为事件循环线程

  void updatechannel(Channel* ch);
  void removechannel(Channel* ch);

  void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);
  void settimercallback(std::function<void(int)> fn);

  void newconnection(spConnection conn);
  void queueinloop(std::function<void()> fn);

  void wakeup();

  void handlewakeup();
  void handletimer();
};
