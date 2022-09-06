#ifndef ARCH_H__
#define ARCH_H__

struct Context {
  uintptr_t gpr[32]; 
  uintptr_t mcause; 
  union {
    struct {
      uintptr_t bits0_2   : 3;
      uintptr_t MIE       : 1;
      uintptr_t bits4_6   : 3;
      uintptr_t MPIE      : 1;
      uintptr_t bits8_31  : 24;
    };
    uintptr_t mstatus;
  };
  uintptr_t mepc;
  uintptr_t mscratch;
  void *pdir;
};

#define GPR1 gpr[17] // a7
#define GPR2 gpr[10] // a0
#define GPR3 gpr[11] // a1
#define GPR4 gpr[12] // a2
#define GPRx gpr[10] // a0

enum {
  SYS_exit,
  SYS_yield,
  SYS_open,
  SYS_read,
  SYS_write,
  SYS_kill,
  SYS_getpid,
  SYS_close,
  SYS_lseek,
  SYS_brk,
  SYS_fstat,
  SYS_time,
  SYS_signal,
  SYS_execve,
  SYS_fork,
  SYS_link,
  SYS_unlink,
  SYS_wait,
  SYS_times,
  SYS_gettimeofday
};

#endif
