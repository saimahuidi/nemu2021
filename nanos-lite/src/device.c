#include "am.h"
#include "amdev.h"
#include "klib-macros.h"
#include <common.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  yield();
  const char *string = buf;
  size_t count_t = 0;
  while (*string && count_t < len) {
    putch(*string++);
    count_t++;
  }
  return count_t;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  yield();
  AM_INPUT_KEYBRD_T keyboard;
  ioe_read(AM_INPUT_KEYBRD, &keyboard);
  // read no key
  if (!keyboard.keycode) {
    strcpy(buf, "NONE\n");
    return 5;
  }
  // write the information of key
  return sprintf(buf, "k%c %s\n", keyboard.keydown? 'd' : 'u', keyname[keyboard.keycode]);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T gc;
  ioe_read(AM_GPU_CONFIG, &gc);
  return sprintf(buf, "WIDTH:%d\nHEIGHT:%d", gc.width, gc.height);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  yield();
  AM_GPU_CONFIG_T gc;
  ioe_read(AM_GPU_CONFIG, &gc);
  int pos = offset / 4;
  int y = pos / gc.width;
  int x = pos % gc.width;
  io_write(AM_GPU_FBDRAW, x, y, buf, len, 1, true);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}

