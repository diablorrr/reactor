#pragma once
#include <ctime>
#include <string>
#include <iostream>

class Timestamp {
private:
  time_t m_secsinceepoch;

public:
  Timestamp();
  Timestamp(int64_t secsinceepoch);

  static Timestamp now();

  time_t toint() const;
  std::string tostring() const;
};
