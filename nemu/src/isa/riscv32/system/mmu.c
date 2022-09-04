#include "common.h"
#include "debug.h"
#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#include <stdio.h>

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  // compute the VPN
  uint32_t VPN1 = (uint32_t)vaddr >> 22;
  uint32_t VPN2 = ((uint32_t)vaddr & 0x3ff000) >> 12;

  paddr_t primaryTable = cpu.satp << 12;
  PTEnode primaryEntry;
  primaryEntry.val = paddr_read(primaryTable + 4 * VPN1, 4);
  // printf("primaryTable = %x vaddr = %x pc = %x\n", primaryTable, vaddr, cpu.pc);
  if (!primaryEntry.V) {
    printf("primaryTable = %x vaddr = %x pc = %x\n", primaryTable, vaddr, cpu.pc);
  }
  Assert(primaryEntry.V, "primary entry is not vaild\n");

  paddr_t secondTable = primaryEntry.PPN10_19 << 12 | primaryEntry.PPN20_31 << 22;
  PTEnode secondEntry;
  secondEntry.val = paddr_read(secondTable + 4 * VPN2, 4);
  Assert(secondEntry.V, "second entry is not vaild\n");

  paddr_t paddrBase = secondEntry.PPN10_19 << 12 | secondEntry.PPN20_31 << 22;
  return paddrBase + (vaddr & 0xfff);
}

int isa_mmu_check(vaddr_t vaddr, int len, int type) {
  if (cpu.satp >> 31) {
    return MMU_TRANSLATE;
  } else {
    return MMU_DIRECT;
  }
}
