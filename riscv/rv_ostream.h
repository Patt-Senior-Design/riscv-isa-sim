#ifndef _RISCV_OSTREAM_H
#define _RISCV_OSTREAM_H

#include <ostream>
#include <string>

class rv_ostream_t
{
public:
  rv_ostream_t(std::ostream& ostream)
    : ostream(ostream) {}

  void printf(const char* fmt, ...);

  rv_ostream_t& operator<<(const char* str) {ostream << str; return *this;}
  rv_ostream_t& operator<<(const std::string& str) {ostream << str; return *this;}

private:
  std::ostream& ostream;
};

#endif
