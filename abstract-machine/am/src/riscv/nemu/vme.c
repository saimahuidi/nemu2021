#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);
  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  // printf("pdir = %p\n", c->pdir);
  
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

typedef struct _PTEnode {
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
}PTEnode;

void map(AddrSpace *as, void *va, void *pa, int prot) {
  // judge whether the entry in the basic table is vaild
  
  uint32_t VPN1 = (uint32_t)va >> 22;
  PTEnode *primaryEntry = (PTEnode *)as->ptr;
  // printf("as = %p\n", as);

  // if (as->ptr == NULL) {
  //   printf("as.pgsize = %d\n", as->pgsize);
  //   printf("as.start %d end %d \n", as->area.start, as->area.end);
  // }
  // printf("pte = %p\n" ,primaryEntry);
  if (!primaryEntry[VPN1].V) {
    // set the entry
    uintptr_t PPN = (uint32_t)pgalloc_usr(PGSIZE);
    primaryEntry[VPN1].PPN10_19 = (PPN >> 12) & 0x3ff;
    primaryEntry[VPN1].PPN20_31 = (PPN >> 22);
    primaryEntry[VPN1].R   = 0;
    primaryEntry[VPN1].W   = 0;
    primaryEntry[VPN1].X   = 0;
    primaryEntry[VPN1].D   = 0;
    primaryEntry[VPN1].A   = 0;
    primaryEntry[VPN1].U   = 0;
    primaryEntry[VPN1].V   = 1;
  }

  PTEnode *secondEntry = (PTEnode *)(primaryEntry[VPN1].PPN10_19 << 12 | primaryEntry[VPN1].PPN20_31 << 22);
  uint32_t VPN2 = ((uint32_t)va & 0x3ff000) >> 12;
  // set the leave entry
  secondEntry[VPN2].PPN10_19 = (uint32_t)pa >> 12 & 0x3ff;
  secondEntry[VPN2].PPN20_31 = (uint32_t)pa >> 22;
  secondEntry[VPN2].R   = 1;
  secondEntry[VPN2].W   = 1;
  secondEntry[VPN2].X   = 1;
  secondEntry[VPN2].D   = 0;
  secondEntry[VPN2].A   = 0;
  secondEntry[VPN2].U   = 0;
  secondEntry[VPN2].V   = 1;
  secondEntry[VPN2].G   = 0;
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *context = (Context *)(kstack.end - sizeof(Context));
  for (int i = 0; i < 32; i++) {
    context->gpr[i] = 0;
  } 
  context->gpr[2] = (uintptr_t)kstack.end;
  context->mstatus = 0x1800;
  context->mepc    = (uintptr_t)entry;
  context->mcause  = 8;
  context->pdir    = as->ptr;
  return context;
}
