#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 8:
        if (c->GPR1 == -1) {
          ev.event = EVENT_YIELD; break;}
        ev.event = EVENT_SYSCALL; break;
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }
  __am_switch(c);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *context = (Context *)(kstack.end + sizeof(Context));
  for (int i = 0; i < 32; i++) {
    context->gpr[i] = 0;
  } 
  context->gpr[2] = (uintptr_t)kstack.end;
  context->gpr[10] = (uintptr_t)arg;
  context->mstatus = 0x1800;
  context->mepc    = (uintptr_t)entry;
  context->mcause  = 8;
  context->pdir    = NULL;
  return context;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
