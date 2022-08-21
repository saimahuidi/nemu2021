#include <klib.h>
#include <klib-macros.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t count = 0;
  while (s[count]) {
    count++;
  }
  return count;
}

char *strcpy(char *dst, const char *src) {
  size_t i = 0;
  while (src[i]) {
    dst[i] = src[i];
    i++;
  }
  dst[i] = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i = 0;
  while(src[i] && i < n) {
    dst[i] = src[i];
    i++;
  }
  for(; i < n; i++) {
    dst[i] = '\0';
  }
  return dst;
}

char *strcat(char *dst, const char *src) {
  size_t i = 0;
  for(; dst[i]; i++);
  size_t j = 0;
  while (src[j]) {
    dst[i++] = src[j++];
  }
  dst[i] = src[j];
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  size_t i = 0;
  while (s1[i] && s2[i]) {
    if (s1[i] != s2[i]) {
      return s1[i] - s2[i];
    }
    i++;
  }
  return s1[i] - s2[i];
}

int strncmp(const char *s1, const char *s2, size_t n) {
  size_t i = 0;
  while (i < (n - 1) && s1[i] && s2[i]) {
    if (s1[i] != s2[i]) {
      return s1[i] - s2[i];
    }
    i++;
  }
  return s1[i] - s2[i];
}

void *memset(void *s, int c, size_t n) {
  char *dst = s;
  char tar = c;
  for (int i = 0; i < n; i++) {
    dst[i] = tar;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  char *d = dst;
  const char *s = src;
  for (int i = 0; i < n; i++) {
    d[i] = s[i];
  }
  return d;
}

void *memcpy(void *out, const void *in, size_t n) {
  return memmove(out, in, n);
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const char *c1 = s1;
  const char *c2 = s2;
  for (int i = 0; i < n; i++) {
    if (c1[i] != c2[i]) {
      return c1[i] - c2[i];
    }
  }
  return 0;
}

#endif
