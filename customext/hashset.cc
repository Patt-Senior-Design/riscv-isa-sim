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
} xrs1;


/* LBCMP Extension */
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


/* PFD Extension */
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

/* PCR Extension */
static reg_t custom_pcr(processor_t* p, insn_t insn, reg_t pc) {
    reg_t base_val = (RS1 & ~RS2);
    reg_t result = RS1;
    reg_t temp;
    for (int i = 31; i >= 0; i--) {
      temp = (1 << i);
      if (base_val & temp)
        result = RS1 ^ temp;
    }

    WRITE_RD(result);

    return pc + 4;
}



class hashset_t : public extension_t {
  public:
    const char* name() {return "hashset";}

    hashset_t() {}
    

    // lbcmp:  0000000 xxxxx xxxxx x11 xxxxx 0001011
    //   mask: 1111111 00000 00000 011 00000 1111111 (fe00307f)
    //   val:  0000000 00000 00000 011 00000 0001011 (0000300b)
    //
    // pfd:  0000000 xxxxx xxxxx xx0 xxxxx 0101011
    //   mask: 1111111 00000 00000 001 00000 1111111 (fe00107f)
    //   val:  0000000 00000 00000 000 00000 0101011 (0000002b)

    // pcr:  0000000 xxxxx xxxxx xx1 xxxxx 0101011
    //   mask: 1111111 00000 00000 001 00000 1111111 (fe00107f)
    //   val:  0000000 00000 00000 001 00000 0101011 (0000102b)
    std::vector<insn_desc_t> get_instructions() {
        std::vector<insn_desc_t> insns;
        insns.push_back((insn_desc_t){0x0000002b, 0xfe00107f, custom_pfd, nullptr});
        insns.push_back((insn_desc_t){0x0000102b, 0xfe00107f, custom_pcr, nullptr});
        insns.push_back((insn_desc_t){0x0000300b, 0xfe00307f, custom_lbcmp, nullptr});
        return insns;
    }

    std::vector<disasm_insn_t*> get_disasms() {
        std::vector<disasm_insn_t*> insns;
        insns.push_back(new disasm_insn_t("pfd", 0x0000002b, 0xfe00107f, {&xrd,&xrs2,&xrs1}));
        insns.push_back(new disasm_insn_t("pcr", 0x0000102b, 0xfe00107f, {&xrd,&xrs2,&xrs1}));
        insns.push_back(new disasm_insn_t("lbcmp", 0x0000300b, 0xfe00307f, {&xrd,&xrs2,&xrs1}));
        return insns;
    }
};


REGISTER_EXTENSION(hashset, [](){return new hashset_t;})
