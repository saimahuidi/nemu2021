#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <syscall.h>
#include <fcntl.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_usec;
}

int NDL_PollEvent(char *buf, int len) {
  FILE *fp = fopen("/dev/events", "w+");
  assert(fp);
  char keypress[16], keyname[16];
  fscanf(fp, "%s %s\n", keypress, keyname);
  if (!strcmp(keypress, "NONE"))
    return 0;
  sprintf(buf, "%s %s", keypress, keyname);
  return 1;
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  if (*w && *w <= screen_w) {
    canvas_w = *w;
  } else {
    canvas_w = screen_w;
    *w = screen_w;
  }
  if (*h && *h <= screen_h) {
    canvas_h = *h;
  } else {
    canvas_h = screen_h;
    *h = screen_h;
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int w_offset = (screen_w - canvas_w) / 2;
  int h_offset = (screen_h - canvas_h) / 2;
  w = w == 0 ? canvas_w : w;
  h = h == 0 ? canvas_h : h;
  for (int i = 0; i < h; i++) {
    lseek(fbdev, ((y + h_offset + i) * screen_w + (w_offset + x)) * 4, SEEK_SET);
    write(fbdev, pixels + i * w, w);
  }
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  fbdev = open("/dev/fb", 0, 0);
  FILE *fp = fopen("/dev/dispinfo", "w+");
  fscanf(fp, "WIDTH:%d\nHEIGHT:%d", &screen_w, &screen_h);
  return 0;
}

void NDL_Quit() {
}
