#include <am.h>
#include "arch/riscv32-nemu.h"
#include <fs.h>
#include <stdint.h>
#include <time.h>
#include <proc.h>
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  switch (a[0]) {
    case SYS_yield:
      yield();
    case SYS_exit:
      naive_uload(NULL, "/bin/nterm");
      break;
    case SYS_brk:
      c->GPRx = 0;
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
      naive_uload(NULL, (void *)c->GPR2);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

