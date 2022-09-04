#include "klib-macros.h"
#include "memory.h"
#include <proc.h>
#include <elf.h>
#include <ramdisk.h>
#include <fs.h>
#include <am.h>
#include <stdint.h>
#include <string.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#if defined(__ISA_AM_NATIVE__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_RISCV32__)
# define EXPECT_TYPE EM_RISCV  // see /usr/include/elf.h to get the right type
#else
# error Unsupported ISA
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  // load the elfheader
  Elf32_Ehdr header; 
  int fd = fs_open(filename, 0, 0);
  if (fd == -1) {
    return 0;
  }
  fs_read(fd, &header, sizeof(Elf32_Ehdr));
  assert(*(uint32_t *)header.e_ident == 0x464c457f);
  assert(header.e_machine == EXPECT_TYPE);
  // load the information of segment header
  int seg_num = header.e_phnum;
  Elf32_Phdr pheader;
  int phdr_offset = header.e_phoff;
  for (int i = 0; i < seg_num; i++) {
    // load the needed segment
    fs_lseek(fd, phdr_offset + i * sizeof(Elf32_Phdr), SEEK_SET);
    fs_read(fd, &pheader, sizeof(Elf32_Phdr));
    if (pheader.p_type == PT_LOAD) {
      uint32_t offset = pheader.p_offset;
      uint8_t *vaddr = (void *)pheader.p_vaddr;
      uint32_t fsize = pheader.p_filesz;
      uint32_t msize = pheader.p_memsz;
      uint32_t brk = (uint32_t)vaddr + msize;
      // uint8_t *vaddr_t = vaddr;
      // vaddr = (uint8_t *)ROUNDDOWN(vaddr, PGSIZE);
      brk = ROUNDUP(brk, 4096);
      pcb->max_brk = brk > pcb->max_brk ? brk : pcb->max_brk;
      // printf("vaddr = %p end = %p\n", vaddr, vaddr + msize);
      // move the content in file to mapper address
      for (int size = 0; size < msize;) {
        uint32_t downOffset = (uint32_t)vaddr - ROUNDDOWN(vaddr, PGSIZE);
        // map the page
        void *paddr = new_page(1);
        map(&pcb->as, vaddr - downOffset, paddr, 0);
        // printf("map vaddr %p to paddr %p\n", vaddr - downOffset, paddr);
        int length = (fsize >= PGSIZE ? PGSIZE : fsize);
        length = PGSIZE - downOffset > length ? length : PGSIZE - downOffset;
        if (length > 0) {
          fs_lseek(fd, offset + size, SEEK_SET);
          fs_read(fd, paddr + downOffset, length);
        }

        memset(paddr + length + downOffset, 0, PGSIZE - length - downOffset);

        vaddr += PGSIZE - downOffset;
        fsize -= length;
        size += PGSIZE - downOffset;
      }

    }
  }
  return header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

void *get_entry(PCB *pcb, const char *filename) {
  return (void *)loader(pcb, filename);
}