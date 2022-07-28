#include "common.h"
#include "cpu/decode.h"
#include <assert.h>
#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdbool.h>

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
  {"[0-9]+", 'n'},         // plus
  {"==", TK_EQ},        // equal
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
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);


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
            // set the type only
            tokens[nr_token++].type = rules[i].token_type;
            break;
          case 'n':
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
  int ret = TK_NOTYPE;
  int pos = p;
  for (int i = p; i <= q; i++) {
    int c;
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
      case '/':
        if (ret == TK_NOTYPE || ret == '*' || ret == '/') {
          ret = c;
          pos = i;
        }
        break;
      case '+':
      case '-':
        ret = c;
        pos = i;
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
    if (tokens[p].type != 'n') {
      *success = false;
      return 0;
    }
    return atoi(tokens[p].str);
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
    word_t val1 = eval(p, op - 1, success);
    word_t val2 = eval(op + 1, q, success);

    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
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
