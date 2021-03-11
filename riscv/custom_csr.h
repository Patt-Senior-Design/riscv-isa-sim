#include "processor.h"

#ifndef _RISCV_CUSTOMCSR_H
#define _RISCV_CUSTOMCSR_H

bool is_custom_csr(int which);
reg_t get_custom_csr(processor_t* p, int which, insn_t insn,
                     bool write, bool peek);
void set_custom_csr(processor_t* p, int which, reg_t val);

#endif
