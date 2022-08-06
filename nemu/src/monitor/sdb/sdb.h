#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(char *e, bool *success);

typedef struct watchpoint {
  char   expr[64];
  struct watchpoint *next;
  struct watchpoint *prev;
  word_t res;
  int NO;
  bool used;
  bool enabled;
} WP;

WP* new_up();

void free_wp(int wp_NO);

void check_wp();

void print_watchpoint();


#endif
