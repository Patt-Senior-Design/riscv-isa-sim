#include "extension.h"
#include <cstring>
#include <cmath>

// based on riscv-isa-sim/disasm/disasm.cc
struct : public arg_t {
    std::string to_string(insn_t insn) const {
        return xpr_name[insn.rd()];
    }
} xrd;

struct : public arg_t {
    std::string to_string(insn_t insn) const {
        return xpr_name[insn.rs2()];
    }
} xrs2;

struct : public arg_t {
    std::string to_string(insn_t insn) const {
        return std::string("(") + xpr_name[insn.rs1()] + ')';
    }
} xrs1;

static reg_t custom_pfd(processor_t* p, insn_t insn, reg_t pc) {
    reg_t base_val = (RS1 & ~RS2);
    reg_t result = 0;
    if (base_val) {
      for (int i = 31; i >= 0; i--) {
        if (base_val & (1 << i)) 
          result = i;
      }
    }
    else result = 1 << 31;

    WRITE_RD(result);

    return pc + 4;
}

class pfd_t : public extension_t {
  public:
    const char* name() {return "pfd";}

    pfd_t() {}

    // pfd:  0000000 xxxxx xxxxx xx0 xxxxx 0101011
    //   mask: 1111111 00000 00000 001 00000 1111111 (fe00107f)
    //   val:  0000000 00000 00000 000 00000 0101011 (0000002b)
    std::vector<insn_desc_t> get_instructions() {
        std::vector<insn_desc_t> insns;
        insns.push_back((insn_desc_t){0x0000002b, 0xfe00107f, custom_pfd, nullptr});
        return insns;
    }

    std::vector<disasm_insn_t*> get_disasms() {
        std::vector<disasm_insn_t*> insns;
        insns.push_back(new disasm_insn_t("pfd", 0x0000002b, 0xfe00107f, {&xrd,&xrs2,&xrs1}));
        return insns;
    }
};

REGISTER_EXTENSION(pfd, [](){return new pfd_t;})
