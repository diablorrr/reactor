#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <string>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <unistd.h>
#include <sys/syscall.h>

class ThreadPool {
private:
  std::vector<std::thread> m_threads;
  std::queue<std::function<void()>> m_taskqueue;
  const std::string m_threadtype;  // IO、WORKS
  std::atomic<bool> m_stop;
  std::mutex m_mutex;
  std::condition_variable m_condition;

public:
  ThreadPool(size_t threadnum, const std::string& m_threadtype);
  ~ThreadPool();

  size_t size() const;

  void addtask(std::function<void()> task);
  void stop();
};
