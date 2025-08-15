#include "Buffer.h"
#include <cstring>

size_t Buffer::size() const {
  return m_buf.size();
}

const char* Buffer::data() {
  return m_buf.data();
}

void Buffer::append(const char* data, size_t size) {
  m_buf.append(data, size);
}

void Buffer::appendwithsep(const char* data, size_t size) {
  if (m_sep == 0) {
    m_buf.append(data, size);
  } else if (m_sep == 1) { // 如果是以 消息头+消息体 的形式
    m_buf.append((char*)&size, 4); // 先创建四字节的消息头（内容为消息体的长度）
    m_buf.append(data, size);
  }
}

void Buffer::erase(size_t pos, size_t nn) {
  m_buf.erase(pos, nn);
}

bool Buffer::pickmessage(std::string &ss) {
  if (m_buf.size() == 0) return false;

  if (m_sep == 0) {
    ss = m_buf;
    m_buf.clear();
  } else if (m_sep == 1) {
    int len {};
    memcpy(&len, m_buf.data(), 4);

    if (m_buf.size() < len + 4) return false;
    ss = m_buf.substr(4, len);
    m_buf.erase(0, len+4);
  }

  return true;
}

void Buffer::clear() {
  m_buf.clear();
}
