#include "am.h"
#include "memory.h"
#include <proc.h>
#include <stdint.h>
#include <string.h>

#define MAX_NR_PROC 4

PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static int num = 0;
static PCB pcb_boot = {};
static int next = 0;
PCB *current = NULL;

void *get_entry(PCB *pcb, const char *filename);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    // printf("hello max_brk = %p\n", &pcb[1].max_brk);
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    // yield();
  }
}


char *argv[] = {"/bin/menu", NULL};
char *envp[] = {"PATH=/bin", NULL};
void init_proc() {
  Log("Initializing processes...");

  context_uload(&pcb[num++], "/bin/hello", NULL, NULL);
  context_uload(&pcb[num++], "/bin/nterm", argv, envp);

  switch_boot_pcb();

  // load program here
  // naive_uload(NULL, "/bin/nterm");
}

Context* schedule(Context *prev) {
  next = next % num;

  current->cp = prev;

  current = &pcb[next];

  next++;
  // printf("current cp = %p mie = %d\n", current->cp, pcb[0].cp->MPIE);

  return current->cp;
}

void context_kload(PCB *pcb, void (*entry)(void *), void *arg) {
  Area area = {pcb->stack, pcb->stack + STACK_SIZE};
  pcb->cp = kcontext(area, entry, arg);
}

int context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
  protect(&pcb->as);
  Area area = {pcb->stack, pcb->stack + STACK_SIZE};
  pcb->max_brk = 0;
  void *entry = get_entry(pcb, filename);
  if (!entry) {
    return -2;
  }
  pcb->cp = ucontext(&pcb->as, area, entry);
  // get the end of the userStack
  void *paddr_stack = new_page(8);
  void *vaddr_stack = pcb->as.area.end - 8 * PGSIZE;
  char *sp = paddr_stack + 8 * PGSIZE;
  // map the user stack
  for (int i = 0; i < 8; i++) {
    map(&pcb->as, vaddr_stack + i * PGSIZE, paddr_stack + i * PGSIZE, 0);
  }
  // record the address of the strings
  char *record[64];
  // record the number of arg
  int argc = 0;
  //record the total number of arg and env
  int count = 0;
  if (argv) {
    // save each arg
    for (int i = 0; argv[i]; i++) {
      int length = strlen(argv[i]) + 5;
      sp -= length;
      // record the address
      record[count] = sp;
      // copy the string
      int j;
      for (j = 0; argv[i][j]; j++) {
        sp[j] = argv[i][j];
      }
      sp[j] = '\0';
      argc++;
      count++;
    }
  }
  record[count++] = NULL;
  if (envp) {
    // save each envp
    for (int i = 0; envp[i]; i++) {
      int length = strlen(envp[i]) + 5;
      sp -= length;
      // record the address
      record[count] = sp;
      // copy the string
      int j;
      for (j = 0; envp[i][j]; j++) {
        sp[j] = envp[i][j];
      }
      sp[j] = '\0';
      count++;
    }
  }
  record[count++] = NULL;
  // save the addresses in the stack
  sp -= sizeof(void *) * (count + 4);
  intptr_t *sp_t = (intptr_t *) sp;
  // save argc
  sp_t[0] = argc;
  // save the addresses
  for (int i = 1; i <= count; i++) {
    sp_t[i] = (intptr_t)record[i - 1];
  }
  // store the stack address
  pcb->cp->GPRx = (uintptr_t)sp;
  return 0;
}
