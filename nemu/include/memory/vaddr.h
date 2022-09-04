#ifndef __MEMORY_VADDR_H__
#define __MEMORY_VADDR_H__

#include <common.h>

word_t vaddr_ifetch(vaddr_t addr, int len);
word_t vaddr_read(vaddr_t addr, int len);
void vaddr_write(vaddr_t addr, int len, word_t data);

#define PAGE_SHIFT        12
#define PAGE_SIZE         (1ul << PAGE_SHIFT)
#define PAGE_MASK         (PAGE_SIZE - 1)

typedef struct _PTEnode {
  union {
    struct {
      uint32_t V        : 1;
      uint32_t R        : 1;
      uint32_t W        : 1;
      uint32_t X        : 1;
      uint32_t U        : 1;
      uint32_t G        : 1;
      uint32_t A        : 1;
      uint32_t D        : 1;
      uint32_t RSW      : 2;
      uint32_t PPN10_19 : 10;
      uint32_t PPN20_31 : 12;
    };
    uint32_t val;
  };
}PTEnode;

#endif
