#include <am.h>
#include "arch/riscv32-nemu.h"
#include "debug.h"
#include <fs.h>
#include <stdint.h>
#include <time.h>
#include <proc.h>

#define MAX_NR_PROC 4

extern PCB pcb[MAX_NR_PROC];

int mm_brk(uintptr_t brk);
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  switch (a[0]) {
    case SYS_yield:
      yield();
    case SYS_exit:
      panic("not implement\n");
      break;
    case SYS_brk:
      c->GPRx = mm_brk(c->GPR2);
      break;
    case SYS_write:
      c->GPRx = fs_write(c->GPR2, (void *)c->GPR3, c->GPR4);
      break;
    case SYS_read: 
      c->GPRx = fs_read(c->GPR2, (void *)c->GPR3, c->GPR4);
      break;
    case SYS_open: 
      c->GPRx = fs_open((char *)c->GPR2, c->GPR3, c->GPR4);
      break;
    case SYS_close: 
      c->GPRx = fs_close(c->GPR2);
      break;
    case SYS_lseek: 
      c->GPRx = fs_lseek(c->GPR2, c->GPR3, c->GPR4);
      break;
    case SYS_gettimeofday: 
      c->GPRx = gettimeofday((void *)c->GPR2, (void *)c->GPR3);
      break;
    case SYS_execve: 
      // c->GPRx = context_uload(&pcb[num++], (void *)c->GPR2, (void *)c->GPR3, (void *)c->GPR4);
      panic("not implement\n");
      
      yield();
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

