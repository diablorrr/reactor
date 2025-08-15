#pragma once
#include <cstddef>
#include <cstdint>
#include <string>

class Buffer
{
private:
  std::string m_buf;
  const uint16_t m_sep;

public:
  Buffer(uint16_t sep=1): m_sep{sep} {}; // 0 没有分隔符；1 四字节报头
  ~Buffer() = default;

  size_t size() const;
  const char* data();

  void append(const char* data, size_t size);
  void appendwithsep(const char* data, size_t size);
  void erase(size_t pos, size_t nn);
  bool pickmessage(std::string &ss);
  void clear();


};
