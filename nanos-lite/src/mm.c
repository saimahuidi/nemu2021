#include "am.h"
#include "klib-macros.h"
#include "proc.h"
#include <memory.h>
#include <stdint.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *ret = pf;
  pf += nr_page * 4096;
  memset(ret, 0, nr_page * 4096);
  return ret;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  int an = ROUNDUP(n, 4096);
  return new_page(an / 4096);
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  for (; current->max_brk < brk; current->max_brk += PGSIZE) {
    map(&current->as, (void *)current->max_brk, new_page(1), 0);
  }
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
