#include <isa.h>
#include <stdio.h>
#define IRQ_TIMER 0x80000007  // for riscv32

word_t isa_query_intr() {
  if (cpu.INTR && cpu.MIE) {
    cpu.INTR = false;
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  // printf("intr\n NO = %0x\n",NO);
  cpu.mepc = epc;
  cpu.mcause = NO;
  cpu.MPIE = cpu.MIE;
  cpu.MIE  = 0;
  return cpu.mtvec;
}

