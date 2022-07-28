#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include "common.h"
#include "memory/paddr.h"
#include "sdb.h"
#include "utils.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_si(char *args) {
  int count = 0;
  if (args == NULL) {
    printf("lack some args\n");
    count = 1;
  } else {
    // get the token
    char *arg = strtok(NULL, " ");
    // calculate the number of instructions
    count = atoi(arg);
  }
  // execute the instructions
  cpu_exec(count);
  return 0;
}

static int cmd_info(char *args) {
  if (args == NULL) {
    printf("lack some args\n");
    return 0;
  }
  // get the token
  char *arg = strtok(NULL, " ");
  switch (*arg) {
    case 'r':
      isa_reg_display();
      break;
    case 'w':
      isa_reg_display();
      break;
    default:
      printf("No such subcmd!");
      break;
  }
  return 0;
}

static int cmd_x(char *args) {
  if (args == NULL) {
    printf("lack some args\n");
    return 0;
  }
  // get the args
  int count = 0;
  char exp[100];
  int a = sscanf(args, "%d %s\n", &count, exp);
  if (a != 2) {
    printf("lack some args\n");
    return 0;
  }
  // get the result of the expr
  bool success = true;
  word_t res = expr(exp, &success);
  if (!success) {
    printf("expression has error\n");
    return 0;
  }
  // print the memory
  for(int i = 0; i < count; i++) {
    word_t addr = res + i * 4;
    word_t mem0 = paddr_read(addr, 1);
    word_t mem1 = paddr_read(addr + 1, 1);
    word_t mem2 = paddr_read(addr + 2, 1);
    word_t mem3 = paddr_read(addr + 3, 1);
    printf("addr: 0x%08x\tmem: %02x %02x %02x %02x\t", addr, mem0, mem1, mem2, mem3);
    printf("\n");
  }

  return 0;
}

static int cmd_p(char *args) {
  if (args == NULL) {
    printf("lack some args\n");
    return 0;
  }
  bool success = true;
  word_t res = expr(args, &success);
  if (success) {
    printf("result = %d\n", res);
  } else {
    printf("expression has error\n");
  }
  return 0;
}

static int cmd_w(char *args) {
  return 0;
}

static int cmd_d(char *args) {
  return 0;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help"  ,     "Display informations about all supported commands",    cmd_help  },
  { "c"     ,     "Continue the execution of the program",                cmd_c     },
  { "q"     ,     "Exit NEMU",                                            cmd_q     },
  { "si"    ,     "Single Step",                                          cmd_si    },
  { "info"  ,     "Print the info",                                       cmd_info  },
  { "x"     ,     "Scan the memory",                                      cmd_x     },
  { "p"     ,     "Print the expressions",                                cmd_p     },
  { "w"     ,     "Set the watchpoints",                                  cmd_w     },
  { "d"     ,     "Delete the watchpoints",                               cmd_d     },
  /* TODO: Add more commands */
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
