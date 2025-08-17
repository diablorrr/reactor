#include "EventLoop.h"

void EventLoop::run() {
  while (m_stop == false) {
    std::vector<Channel*> channels = m_ep->loop(10*1000);

    if (channels.size() == 0) {
      // TODO 超时回调
    } else {
      for (auto& ch : channels) {
        ch->handleevent();
      }
    }
  }
}


void EventLoop::stop() {
  m_stop = true;
  // TODO
}


void EventLoop::updatechannel(Channel* ch) {
  m_ep->updatechannel(ch);
}


void EventLoop::removechannel(Channel* ch) {
  m_ep->removechannel(ch);
}
