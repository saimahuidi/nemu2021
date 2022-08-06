#include "common.h"
#include "utils.h"
#include "sdb.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP head, free_;
static int wp_num;

void init_wp_pool() {
  wp_num = 0;
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].enabled = false;
    wp_pool[i].next = (i == NR_WP - 1 ? &free_ : &wp_pool[i + 1]);
    wp_pool[i].prev = (i == 0         ? &free_ : &wp_pool[i - 1]);
  }
  free_.next = &wp_pool[0];
  free_.prev = &wp_pool[NR_WP - 1];
  head.next  = &head;
  head.prev  = &head;
}

WP* new_up() {
  // exceed the limit
  if(wp_num >= NR_WP) {
    return NULL;
  }
  //get the first of free list
  WP *new = free_.next;
  new->used = true;
  new->next->prev = new->prev;
  new->prev->next = new->next;
  new->next = &head;
  new->prev = head.prev;
  head.prev->next = new;
  head.prev = new;
  wp_num++;
  return new;
}

void free_up(WP *wp) {
  wp->next->prev = wp->prev;
  wp->prev->next = wp->next;
  wp->next = free_.next;
  wp->prev = &free_;
  free_.next->prev = wp;
  free_.next = wp;
  wp->used = false;
  wp_num--;
}

void free_wp(int wp_NO) {
  WP *wp_t = &wp_pool[wp_NO];
  if (!wp_t->used) {
    printf("The watchpoint is not being used.\n");
    return;
  } else if(wp_t->NO < 0 || wp_t->NO >=NR_WP) {
    printf("The watchpoint is not existed\n");
    return;
  }
  free_up(wp_t);
  printf("Delete successfully!\n");
  return;
}

void check_wp() {
  WP *tmp = head.next;
  for(int i = 0; i < wp_num; i++) {
    bool success = true;
    char *expression = tmp->expr;
    word_t new_res = expr(expression, &success); 
    if (new_res != tmp->res) {
      printf("expr: %s change from %d to %d\n", expression, tmp->res, new_res);
      nemu_state.state = NEMU_STOP;
      tmp->res = new_res;
    }
    tmp = tmp->next;
  }
}

void print_watchpoint() {
  WP *tmp = head.next;
  if (!wp_num) {
    printf("There is no watchpoint now\n");
  }
  for(int i = 0; i < wp_num; i++) {
    printf("watchpoint NO.%d\texpr:%s\tvalue:%d\n", tmp->NO, tmp->expr, tmp->res);
    tmp = tmp->next;
  }
}
