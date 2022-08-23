#include "riscv/riscv.h"
#include <am.h>
#include <nemu.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

uint16_t screen_width = 400;
void __am_gpu_init() {
  uint32_t screen = inl(VGACTL_ADDR);
  uint16_t width = screen >> 16;
  screen_width = width;
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t screen = inl(VGACTL_ADDR);
  uint16_t width = screen >> 16;
  uint16_t height = screen & 0xffff;
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = width, .height = height,
    .vmemsz = width * height
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (ctl->pixels != NULL) {
    uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
    uint32_t pos = ctl->x + ctl->y * screen_width;
    uint32_t *buf = ctl->pixels;
    for (int i = 0; i < ctl->h; i++) {
      uint32_t pos_t = pos + i * screen_width;
      uint32_t pos_b = i * ctl->w;
      for (int j = 0; j < ctl->w; j++) {
        fb[pos_t + j] = buf[pos_b + j];
      }
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
