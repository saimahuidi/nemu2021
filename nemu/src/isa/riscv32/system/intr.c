#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  cpu.mepc = epc;
  cpu.mcause = NO;
  return cpu.mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
