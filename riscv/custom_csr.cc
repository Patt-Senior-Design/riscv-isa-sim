#include "custom_csr.h"
#include <cstdint>
#include <cstdio>

static uint8_t muarttx;
static uint32_t mbfsroot;
static uint32_t mbfstarg;
static uint32_t mbfsqbase;
static uint32_t mbfsqsize;

bool is_custom_csr(int which) {
  #define DECLARE_CSR(name, num) if(which == num) {return true;}
  #include "encoding_custom.h"
  #undef DECLARE_CSR
  return false;
}

reg_t get_custom_csr(processor_t* p, int which, insn_t insn,
                     bool write, bool peek) {
  switch(which) {
  case CSR_MUARTSTAT:
    return MUARTSTAT_TXEMPTY | MUARTSTAT_RXEMPTY;
  case CSR_MUARTTX:
    return (reg_t) muarttx;
  case CSR_MBFSSTAT:
    return MBFSSTAT_DONE;
  case CSR_MBFSROOT:
    return (reg_t) mbfsroot;
  case CSR_MBFSTARG:
    return (reg_t) mbfstarg;
  case CSR_MBFSQBASE:
    return (reg_t) mbfsqbase;
  case CSR_MBFSQSIZE:
    return (reg_t) mbfsqsize;
  default:
    return 0;
  }
}

void set_custom_csr(processor_t* p, int which, reg_t val) {
  switch(which) {
  case CSR_MUARTTX:
    muarttx = (uint8_t) val;
    putchar(muarttx);
    break;
  case CSR_MBFSROOT:
    mbfsroot = (uint32_t) val;
    break;
  case CSR_MBFSTARG:
    mbfstarg = (uint32_t) val;
    break;
  case CSR_MBFSQBASE:
    mbfsqbase = (uint32_t) val;
    break;
  case CSR_MBFSQSIZE:
    mbfsqsize = (uint32_t) val;
    break;
  }
}
