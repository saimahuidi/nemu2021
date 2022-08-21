#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <stddef.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

char numbuf[65] = {'\0'};

void sprintf_helper(int *index, char *out, const char *fmt);

char *num2str(size_t num, size_t pos);

int printf(const char *fmt, ...) {
  va_list ap;
  char c;
  char c_in_c;
  char *s_ptr;
  va_start(ap, fmt);
  size_t num;
  size_t count = 0;
  while ((c = *fmt++)) {
    if (c != '%') {
      putch(c);
      count++;
    } else {
      c = *fmt++;
      switch (c) {
        case 'c':
          c_in_c = (char)va_arg(ap, int);
          putch(c_in_c);
          count++;
          break;
        case 's':
          s_ptr = va_arg(ap, char *);
          while ((c_in_c = *s_ptr++)) {
            putch(c_in_c);
            count++;
          }
          break;
        case 'd':
          num = (size_t)va_arg(ap, int);
          count += printf(num2str(num, 10));
          break;
        case 'l':
          fmt++;
          num = (size_t)va_arg(ap, long);
          count += printf(num2str(num, 10));
          break;
        case 'p':
          putch('0');
          putch('x');
          count += 2;
          num = va_arg(ap, size_t);
          count += printf(num2str(num, 16));
          break;
      }
    }
  }
  return count;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  int index = 0;
  va_list ap;
  char c;
  char c_in_c;
  char *s_ptr;
  va_start(ap, fmt);
  size_t num;
  while ((c = *fmt++)) {
    if (c != '%') {
      out[index] = c;
      index += 1;
    } else {
      c = *fmt++;
      switch (c) {
        case 'c':
          c_in_c = (char)va_arg(ap, int);
          out[index] = c_in_c;
          index += 1;
          break;
        case 's':
          s_ptr = va_arg(ap, char *);
          while ((c_in_c = *s_ptr++)) {
            out[index] = c_in_c;
            index += 1;
          }
          break;
        case 'd':
          num = (size_t)va_arg(ap, int);
          sprintf_helper(&index, out, num2str(num, 10));
          break;
        case 'l':
          fmt++;
          num = (size_t)va_arg(ap, long);
          sprintf_helper(&index, out, num2str(num, 10));
          break;
        case 'p':
          out[index] = '0';
          index += 1;
          out[index] = 'x';
          index += 1;
          num = va_arg(ap, size_t);
          sprintf_helper(&index, out, num2str(num, 16));
          break;
      }
    }
  }
  out[index] = '\0';
  return index;
}

void sprintf_helper(int *index, char *out, const char *fmt) {
  char c;
  while ((c = *fmt++)) {
    out[*index] = c;
    *index += 1;
  }
}


int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

char *num2str(size_t num, size_t pos) {
  
  size_t tmp;
  size_t i = 63;
  do {
    tmp = num % pos;
    num /= pos;
    if ((tmp + '0') <= '9') {
      numbuf[i] = tmp + '0';
    } else {
      numbuf[i] = tmp + 87;
    }
    i--;
  }while (num != 0);
  return &numbuf[i + 1];
}

#endif
