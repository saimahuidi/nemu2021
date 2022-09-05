#include <common.h>
#include <syscall.h>
#include <proc.h>

void do_syscall(Context *c);

extern PCB pcb[];
static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD:
      c = schedule(c);
      break;

    case EVENT_SYSCALL:
      do_syscall(c);
      break;

    default: panic("Unhandled event ID = %d", e.event);
  }
  // printf("context = %p\n", c);
  // printf("max_brk = %p\n", pcb[1].max_brk);
  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
