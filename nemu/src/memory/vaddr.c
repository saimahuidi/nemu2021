#include "common.h"
#include "debug.h"
#include <assert.h>
#include <isa.h>
#include <memory/paddr.h>
#include <stdio.h>

word_t vaddr_ifetch(vaddr_t addr, int len) {
  int result = isa_mmu_check(addr, len, MEM_TYPE_IFETCH);
  Assert(result <= MMU_TRANSLATE, "MMU ifetch check wrong\n");
  if (result == MMU_TRANSLATE) {
    paddr_t paddr = isa_mmu_translate(addr, len, MEM_TYPE_IFETCH);
    return paddr_read(paddr, len);
  }
  return paddr_read(addr, len);
}

word_t vaddr_read(vaddr_t addr, int len) {
  int result = isa_mmu_check(addr, len, MEM_TYPE_READ);
  Assert(result <= MMU_TRANSLATE, "MMU read check wrong\n");
  if (result == MMU_TRANSLATE) {
    paddr_t paddr = isa_mmu_translate(addr, len, MEM_TYPE_READ);
    return paddr_read(paddr, len);
  }
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
  int result = isa_mmu_check(addr, len, MEM_TYPE_WRITE);
  Assert(result <= MMU_TRANSLATE, "MMU write check wrong\n");
  if (result == MMU_TRANSLATE) {
    paddr_t paddr = isa_mmu_translate(addr, len, MEM_TYPE_WRITE);
    paddr_write(paddr, len, data);
    return;
  }
  paddr_write(addr, len, data);
}
