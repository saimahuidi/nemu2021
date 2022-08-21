#include "common.h"
#include "cpu/decode.h"
#include <assert.h>
#include <isa.h>
#include "generated/autoconf.h"
#include "memory/paddr.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"-", '-'},         // minus
  {"\\*", '*'},         // multiply
  {"/", '/'},         // divide
  {"\\(", '('},         // left parenthese
  {"\\)", ')'},         // right parenthese
  {"0x[0-9, a-f]+", 'x'},    // hex nums
  {"\\$[$,0-9,a-z]{2,3}", 'r'},     // registers
  {"[0-9]+", 'n'},         // nums
  {"==", TK_EQ},        // equal
  {"!=", '!'},        // not equal
  {"&&", '&'},        // and
  {"\\|\\|", '|'},        // or
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
      //  char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        if (substr_len >= 31) {
          printf("The expression is too long!\n");
          return false;
        }

    //  Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
    //      i, rules[i].regex, position, substr_len, substr_len, substr_start);


        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        switch (rules[i].token_type) {
          case '+':
          case '-':
          case '*':
          case '/':
          case '(':
          case ')':
          case '!':
          case '&':
          case '|':
          case TK_EQ:
            // set the type only
            tokens[nr_token++].type = rules[i].token_type;
            break;
          case 'n':
          case 'r':
          case 'x':
            // record the str
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, e + position, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            nr_token++;
            break;
          case TK_NOTYPE:
            break;
          default: assert(0);
        }

        position += substr_len;

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}
bool check_parentheses(int p, int q, bool *success) {
  if (!(tokens[p].type == '(' && tokens[q].type == ')')) {
    return false;
  }
  else {
    int count = 1;
    for (int i = p + 1; i < q && count > 0; i++) {
      if (tokens[i].type == '(') {
        count++;
      } else if (tokens[i].type == ')') {
        count--;
      }
    }
    if (count == 0) {
      return false;
    } else {
      return true;
    }
  }
}

int get_main_oprand(int p, int q, bool *success) {
  int pos = p - 1;
  int level = 0;
  for (int i = p; i <= q; i++) {
    int c;
    int d;
    switch (c = tokens[i].type) {
      case '(':
        i++;
        int count = 1;
        for (; i <= q && count > 0; i++) {
          if (tokens[i].type == '(') {
            count++;
          } else if (tokens[i].type == ')') {
            count--;
          }
        }
        if (count > 0) {
          *success = false;
          return 0;
        }
        i--;
      break;
      case '*':
        if (i == p) {
          break;
        } else {
          d = tokens[i - 1].type;
          if (d != 'x' && d != 'n' && d != 'r') {
            break;
          }
        }
      case '/':
        if (level <= 1) {
          pos = i;
          level = 1;
        }
        break;
      case '+':
      case '-':
        if (level <= 2) {
          pos = i;
          level = 2;
        }
        break;
      case TK_EQ:
      case '!':
        if (level <= 3) {
          pos = i;
          level = 3;
        }
      case '&':
        if (level <= 4) {
          pos = i;
          level = 4;
        }
      case '|':
        if (level <= 5) {
          pos = i;
          level = 5;
        }

        break;
    }
  }
  return pos;
}

word_t eval(int p, int q, bool *success) {
  
  // unexpected condition
  if (p > q) {
    *success = false;
    return 0;
  }
  // when point to the num
  else if (p == q) {
    // expressions have errors
    if (!*success) {
      return 0;
    }
    // when the token point to the wrong type
    if (tokens[p].type == 'n') {
      return atoi(tokens[p].str);
    } else if (tokens[p].type == 'r') {
      return isa_reg_str2val(tokens[p].str + 1, success);
    } else if(tokens[p].type == 'x') {
      word_t hex_r = 0;
      int a = sscanf(tokens[p].str + 2, "%x", &hex_r);
      if (a != 1) {
        *success = false;
      }
      return hex_r;
    }

    *success = false;
    return 0;
  }
  else if (check_parentheses(p, q, success) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    // expressions have errors
    if (!*success) {
      return 0;
    }
    word_t ret = eval(p + 1, q - 1, success);
    return ret;
  }
  else {
    // expressions have errors
    if (!*success) {
      return 0;
    }
    int op = get_main_oprand(p, q, success);
    if (op == p - 1) {
      word_t val_resolve = eval(p + 1, q, success);
      return paddr_read(val_resolve, 4);
    }
    word_t val1 = eval(p, op - 1, success);
    word_t val2 = eval(op + 1, q, success);

    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      case '!': return val1 != val2;
      case '&': return val1 && val2;
      case '|': return val1 || val2;
      case TK_EQ: return val1 == val2;
      default: return 0;
    }
  }
}

word_t expr(char *e, bool *success) {
  // fail to get the tokens
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  // calculate the expressions
  *success = true;
  word_t res = eval(0, nr_token - 1, success);
  // if there is something wrong with the expressions
  if (!*success) {
    return 0;
  }
  return res;
}
