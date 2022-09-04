#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  int argc = args[0];
  char **_args = (char **)(++args);
  while (*args++);
  char **env   = (char **)(args);
  environ = env;
  exit(main(argc, _args, env));
  assert(0);
}
