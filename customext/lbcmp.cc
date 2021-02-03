#include "extension.h"
#include "mmu.h"
#include <cstring>

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
} xrs1_addr;

static reg_t custom_lbcmp(processor_t* p, insn_t insn, reg_t pc) {
    uint8_t byte = RS2;
    reg_t base = RS1;

    reg_t result = 0;
    for(int i = 0; i < 4; i++) {
        uint64_t dword = MMU.load_uint64(base + (i * 8), true);
        for(int j = 0; j < 8; j++) {
            result = (result >> 1) | (((dword & 0xff) == byte) ? 0x80000000 : 0);
            dword >>= 8;
        }
    }
    WRITE_RD(result);

    return pc + 4;
}

class lbcmp_t : public extension_t {
  public:
    const char* name() {return "lbcmp";}

    lbcmp_t() {}

    // lbcmp:  0000000 xxxxx xxxxx x11 xxxxx 0001011
    //   mask: 1111111 00000 00000 011 00000 1111111 (fe00307f)
    //   val:  0000000 00000 00000 011 00000 0001011 (0000300b)
    std::vector<insn_desc_t> get_instructions() {
        std::vector<insn_desc_t> insns;
        insns.push_back((insn_desc_t){0x0000300b, 0xfe00307f, custom_lbcmp, nullptr});
        return insns;
    }

    std::vector<disasm_insn_t*> get_disasms() {
        std::vector<disasm_insn_t*> insns;
        insns.push_back(new disasm_insn_t("lbcmp", 0x0000300b, 0xfe00307f, {&xrd,&xrs2,&xrs1_addr}));
        return insns;
    }
};

REGISTER_EXTENSION(lbcmp, [](){return new lbcmp_t;})
