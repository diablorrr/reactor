#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadnum, const std::string& threadtype):m_stop(false), m_threadtype(threadtype){
  for (int i = 0; i < threadnum; ++i) {
    m_threads.emplace_back([this](){
      printf("create %s thread(%d).\n", m_threadtype.c_str(), syscall(SYS_gettid));
      while(m_stop == false) {
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lock(this->m_mutex);

          m_condition.wait(lock, [this]{
            return this->m_stop == true || this->m_taskqueue.empty() == false;
          });

          if (this->m_stop == true && this->m_taskqueue.empty() == true) return;

          task = std::move(this->m_taskqueue.front());
          this->m_taskqueue.pop();
        }
        task();
      }
    });
  }
}


size_t ThreadPool::size() const{
  return m_threads.size();
}


void ThreadPool::stop() {
  if (m_stop) return;
  m_stop = true;
  m_condition.notify_all();

  for (auto& thread:m_threads) {
    thread.join();
  }
}


ThreadPool::~ThreadPool() {
  stop();
}


void ThreadPool::addtask(std::function<void()> task) {
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_taskqueue.push(task);
  }

  m_condition.notify_one();
}


