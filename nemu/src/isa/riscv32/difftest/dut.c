#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

#define NXPR 32

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  for (int i = 0; i < NXPR; i++) {
    if (ref_r->gpr[i]._32 != gpr(i)) {
      return false;
    }
  }
  return ref_r->pc == pc;
}

void isa_difftest_attach() {
}
