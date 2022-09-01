#include "am.h"
#include "amdev.h"
#include "klib-macros.h"
#include <fs.h>
#include <device.h>
#include <ramdisk.h>
#include <stdio.h>


size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_EVENT]  = {"/dev/events", 0, 0, events_read, invalid_write},
  [FD_DISPINFO]  = {"/dev/dispinfo", 0, 0, dispinfo_read, invalid_write},
  [FD_FB]  = {"/dev/fb", 0, 0, invalid_read, fb_write, 0},
#include "files.h"
  {NULL},
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[FD_FB].size = io_read(AM_GPU_CONFIG).vmemsz * 4;
}
int fs_open(const char *pathname, int flags, int mode) {
  int i;
  for (i = 0; file_table[i].name; i++) {
    if (!strcmp(pathname, file_table[i].name)) {
      file_table[i].open_offset = 0;
      return i;
    }
  }

  assert(NULL);
}

size_t fs_read(int fd, void *buf, size_t len) {
  if (file_table[fd].read) {
    return file_table[fd].read(buf, 0, len);
  }
  int available = file_table[fd].size - file_table[fd].open_offset;
  len = len < available ? len : available;
  ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  if (file_table[fd].write) {
    return file_table[fd].write(buf, file_table[fd].open_offset, len);
  }
  int available = file_table[fd].size - file_table[fd].open_offset;
  len = len < available ? len : available;
  ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  return len;
}
size_t fs_lseek(int fd, size_t offset, int whence) {
  switch (whence) {
    case SEEK_SET:
      file_table[fd].open_offset = offset;
      break;
    case SEEK_CUR:
      file_table[fd].open_offset += offset;
      break;
    case SEEK_END:
      if (offset != 0) {
        assert(NULL);
      }
      file_table[fd].open_offset = file_table[fd].size;
      break;
  }
  return file_table[fd].open_offset;
}

int fs_close(int fd) {
  return 0;
}

