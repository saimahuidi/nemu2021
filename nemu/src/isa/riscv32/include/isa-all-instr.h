#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) \
f(lui) f(lb) f(lbu) f(lh) f(lhu) f(lw) \
f(sw) f(sh) f(sb) \
f(mul) f(mulh) f(mulhsu) f(mulhu) f(div) f(divu) f(rem) f(remu)  \
f(inv) f(nemu_trap) f(auipc) f(jal) f(jalr) \
f(addi) f(add) f(sub) f(xor) f(or) f(and) f(xori) f(andi) f(ori) \
 f(beq) f(bne) f(blt) f(bge) f(bltu) f(bgeu) \
f(slt) f(sltu) f(slti) f(sltiu) \
f(srai) f(sll) f(slli)  f(srli) f(srl) f(sra) 

def_all_EXEC_ID();
